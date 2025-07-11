/*
 * ESP32 Matter Lock Bridge with LED Status Indicators
 * 
 * This application creates a Matter bridge that manages multiple smart locks
 * with red LED status indicators for each lock. Each lock can be dynamically
 * added and controlled through Matter protocol.
 * 
 * Features:
 * - Matter bridge protocol support
 * - Up to 8 smart locks (expandable)
 * - Red LED status indicators on GPIO pins
 * - Dynamic lock addition/removal
 * - Real-time status monitoring
 * - Serial console interface
 * 
 * Hardware Requirements:
 * - ESP32 with WiFi capability
 * - Red LEDs connected to GPIO pins (4, 5, 16, 17, 18, 19, 21, 22)
 * - 220Ω resistors for LEDs
 * - Breadboard and jumper wires
 * 
 * Build Instructions:
 * 1. Set up ESP-IDF v5.1.4
 * 2. Clone esp-matter repository
 * 3. Set target: idf.py set-target esp32
 * 4. Build: idf.py build
 * 5. Flash: idf.py flash monitor
 */

#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>
#include <driver/gpio.h>
#include <esp_log.h>
#include <esp_event.h>
#include <esp_wifi.h>
#include <nvs_flash.h>

#include <esp_matter.h>
#include <esp_matter_console.h>
#include <esp_matter_ota.h>

#include <app_priv.h>
#include <app_reset.h>
#include <app_wifi.h>
#include <app_driver.h>

using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;

static const char *TAG = "ESP32_MATTER_LOCK_BRIDGE";

// Configuration Constants
#define MAX_LOCKS 8
#define BRIDGE_ENDPOINT_ID 0
#define FIRST_LOCK_ENDPOINT_ID 1

// GPIO Pin Definitions for LED Status Indicators
static const gpio_num_t lock_led_pins[MAX_LOCKS] = {
    GPIO_NUM_4,   // Lock 1 LED
    GPIO_NUM_5,   // Lock 2 LED
    GPIO_NUM_16,  // Lock 3 LED
    GPIO_NUM_17,  // Lock 4 LED
    GPIO_NUM_18,  // Lock 5 LED
    GPIO_NUM_19,  // Lock 6 LED
    GPIO_NUM_21,  // Lock 7 LED
    GPIO_NUM_22   // Lock 8 LED
};

// Lock State Structure
typedef struct {
    uint16_t endpoint_id;
    bool is_locked;
    bool is_jammed;
    bool low_battery;
    bool enabled;
    char name[32];
    gpio_num_t led_pin;
    TimerHandle_t blink_timer;
    bool blink_state;
} lock_device_t;

// Global Variables
static node_t *node = NULL;
static lock_device_t locks[MAX_LOCKS];
static int active_lock_count = 0;
static TimerHandle_t status_timer;

// Function Prototypes
static void lock_led_init(void);
static void lock_led_update(int lock_index);
static void lock_led_blink_callback(TimerHandle_t timer);
static void status_display_timer_callback(TimerHandle_t timer);
static esp_err_t add_new_lock(const char* lock_name);
static esp_err_t remove_lock(int lock_index);
static void display_lock_status(void);
static void process_serial_commands(void);

// Matter Callbacks
static esp_err_t app_attribute_update_cb(attribute::callback_type_t type, uint16_t endpoint_id, uint32_t cluster_id,
                                        uint32_t attribute_id, esp_matter_attr_val_t *val, void *priv_data);
static esp_err_t app_identification_cb(identification::callback_type_t type, uint16_t endpoint_id, uint8_t effect_id,
                                     uint8_t effect_variant, void *priv_data);

/**
 * Initialize GPIO pins for LED status indicators
 */
static void lock_led_init(void)
{
    gpio_config_t led_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    for (int i = 0; i < MAX_LOCKS; i++) {
        led_config.pin_bit_mask = (1ULL << lock_led_pins[i]);
        gpio_config(&led_config);
        gpio_set_level(lock_led_pins[i], 0); // Start with LEDs off
        
        locks[i].led_pin = lock_led_pins[i];
        locks[i].enabled = false;
        locks[i].is_locked = false;
        locks[i].is_jammed = false;
        locks[i].low_battery = false;
        locks[i].blink_state = false;
        snprintf(locks[i].name, sizeof(locks[i].name), "Lock_%d", i + 1);
        
        // Create blink timer for each lock
        locks[i].blink_timer = xTimerCreate(
            "lock_blink_timer",
            pdMS_TO_TICKS(500), // 500ms blink period
            pdTRUE,             // Auto-reload
            (void*)(i),         // Timer ID (lock index)
            lock_led_blink_callback
        );
    }
    
    ESP_LOGI(TAG, "Lock LED GPIOs initialized");
}

/**
 * Update LED status for a specific lock
 */
static void lock_led_update(int lock_index)
{
    if (lock_index < 0 || lock_index >= MAX_LOCKS || !locks[lock_index].enabled) {
        return;
    }

    lock_device_t* lock = &locks[lock_index];
    
    if (lock->is_jammed) {
        // Jammed: Fast blinking
        if (!xTimerIsTimerActive(lock->blink_timer)) {
            xTimerChangePeriod(lock->blink_timer, pdMS_TO_TICKS(200), 0);
            xTimerStart(lock->blink_timer, 0);
        }
    } else if (lock->low_battery) {
        // Low battery: Slow blinking
        if (!xTimerIsTimerActive(lock->blink_timer)) {
            xTimerChangePeriod(lock->blink_timer, pdMS_TO_TICKS(1000), 0);
            xTimerStart(lock->blink_timer, 0);
        }
    } else {
        // Normal operation: Solid on/off
        xTimerStop(lock->blink_timer, 0);
        gpio_set_level(lock->led_pin, lock->is_locked ? 1 : 0);
    }
}

/**
 * Blink timer callback for LED status indication
 */
static void lock_led_blink_callback(TimerHandle_t timer)
{
    int lock_index = (int)pvTimerGetTimerID(timer);
    if (lock_index < 0 || lock_index >= MAX_LOCKS) {
        return;
    }
    
    lock_device_t* lock = &locks[lock_index];
    lock->blink_state = !lock->blink_state;
    gpio_set_level(lock->led_pin, lock->blink_state ? 1 : 0);
}

/**
 * Add a new lock device to the bridge
 */
static esp_err_t add_new_lock(const char* lock_name)
{
    if (active_lock_count >= MAX_LOCKS) {
        ESP_LOGW(TAG, "Maximum number of locks (%d) already reached", MAX_LOCKS);
        return ESP_ERR_NO_MEM;
    }
    
    int lock_index = active_lock_count;
    uint16_t endpoint_id = FIRST_LOCK_ENDPOINT_ID + lock_index;
    
    // Create lock endpoint
    endpoint_t *endpoint = door_lock::create(node, &(door_lock::config_t), ENDPOINT_FLAG_NONE, NULL);
    if (!endpoint) {
        ESP_LOGE(TAG, "Failed to create lock endpoint");
        return ESP_FAIL;
    }
    
    // Set endpoint ID
    endpoint::set_device_type_id(endpoint, ESP_MATTER_DOOR_LOCK_DEVICE_TYPE_ID);
    endpoint::set_device_type_revision(endpoint, 1);
    
    // Configure lock device
    locks[lock_index].endpoint_id = endpoint_id;
    locks[lock_index].enabled = true;
    locks[lock_index].is_locked = false;
    locks[lock_index].is_jammed = false;
    locks[lock_index].low_battery = false;
    
    if (lock_name) {
        strncpy(locks[lock_index].name, lock_name, sizeof(locks[lock_index].name) - 1);
        locks[lock_index].name[sizeof(locks[lock_index].name) - 1] = '\0';
    }
    
    // Update LED status
    lock_led_update(lock_index);
    
    active_lock_count++;
    ESP_LOGI(TAG, "Added lock %d: %s (Endpoint: %d)", lock_index + 1, locks[lock_index].name, endpoint_id);
    
    return ESP_OK;
}

/**
 * Remove a lock device from the bridge
 */
static esp_err_t remove_lock(int lock_index)
{
    if (lock_index < 0 || lock_index >= MAX_LOCKS || !locks[lock_index].enabled) {
        ESP_LOGW(TAG, "Invalid lock index or lock not enabled");
        return ESP_ERR_INVALID_ARG;
    }
    
    // Stop blink timer
    xTimerStop(locks[lock_index].blink_timer, 0);
    
    // Turn off LED
    gpio_set_level(locks[lock_index].led_pin, 0);
    
    // Disable lock
    locks[lock_index].enabled = false;
    locks[lock_index].is_locked = false;
    locks[lock_index].is_jammed = false;
    locks[lock_index].low_battery = false;
    
    ESP_LOGI(TAG, "Removed lock %d: %s", lock_index + 1, locks[lock_index].name);
    
    // Note: In a full implementation, you'd also remove the Matter endpoint
    // For simplicity, we're just disabling the lock here
    
    return ESP_OK;
}

/**
 * Display current status of all locks
 */
static void display_lock_status(void)
{
    printf("\n========== MATTER LOCK BRIDGE STATUS ==========\n");
    printf("Active Locks: %d / %d\n", active_lock_count, MAX_LOCKS);
    printf("Bridge Endpoint ID: %d\n", BRIDGE_ENDPOINT_ID);
    printf("------------------------------------------------\n");
    
    for (int i = 0; i < MAX_LOCKS; i++) {
        if (locks[i].enabled) {
            printf("Lock %d: %s\n", i + 1, locks[i].name);
            printf("  Endpoint ID: %d\n", locks[i].endpoint_id);
            printf("  LED Pin: GPIO_%d\n", locks[i].led_pin);
            printf("  Status: %s", locks[i].is_locked ? "LOCKED 🔒" : "UNLOCKED 🔓");
            if (locks[i].is_jammed) printf(" [JAMMED]");
            if (locks[i].low_battery) printf(" [LOW BATTERY]");
            printf("\n");
            printf("  LED Status: %s\n", gpio_get_level(locks[i].led_pin) ? "ON" : "OFF");
            printf("------------------------------------------------\n");
        }
    }
    printf("===============================================\n\n");
}

/**
 * Status display timer callback
 */
static void status_display_timer_callback(TimerHandle_t timer)
{
    display_lock_status();
}

/**
 * Process serial commands for lock control
 */
static void process_serial_commands(void)
{
    char command[100];
    if (fgets(command, sizeof(command), stdin)) {
        // Remove newline
        command[strcspn(command, "\n")] = 0;
        
        if (strncmp(command, "add ", 4) == 0) {
            char* lock_name = command + 4;
            if (add_new_lock(lock_name) == ESP_OK) {
                printf("Added lock: %s\n", lock_name);
            } else {
                printf("Failed to add lock\n");
            }
        }
        else if (strncmp(command, "remove ", 7) == 0) {
            int lock_num = atoi(command + 7);
            if (remove_lock(lock_num - 1) == ESP_OK) {
                printf("Removed lock %d\n", lock_num);
            } else {
                printf("Failed to remove lock %d\n", lock_num);
            }
        }
        else if (strncmp(command, "lock ", 5) == 0) {
            int lock_num = atoi(command + 5);
            if (lock_num > 0 && lock_num <= MAX_LOCKS && locks[lock_num - 1].enabled) {
                locks[lock_num - 1].is_locked = true;
                lock_led_update(lock_num - 1);
                printf("Locked %s\n", locks[lock_num - 1].name);
            } else {
                printf("Invalid lock number\n");
            }
        }
        else if (strncmp(command, "unlock ", 7) == 0) {
            int lock_num = atoi(command + 7);
            if (lock_num > 0 && lock_num <= MAX_LOCKS && locks[lock_num - 1].enabled) {
                locks[lock_num - 1].is_locked = false;
                locks[lock_num - 1].is_jammed = false; // Clear jam on unlock
                lock_led_update(lock_num - 1);
                printf("Unlocked %s\n", locks[lock_num - 1].name);
            } else {
                printf("Invalid lock number\n");
            }
        }
        else if (strncmp(command, "jam ", 4) == 0) {
            int lock_num = atoi(command + 4);
            if (lock_num > 0 && lock_num <= MAX_LOCKS && locks[lock_num - 1].enabled) {
                locks[lock_num - 1].is_jammed = true;
                lock_led_update(lock_num - 1);
                printf("Set jam status for %s\n", locks[lock_num - 1].name);
            } else {
                printf("Invalid lock number\n");
            }
        }
        else if (strncmp(command, "battery ", 8) == 0) {
            int lock_num = atoi(command + 8);
            if (lock_num > 0 && lock_num <= MAX_LOCKS && locks[lock_num - 1].enabled) {
                locks[lock_num - 1].low_battery = !locks[lock_num - 1].low_battery;
                lock_led_update(lock_num - 1);
                printf("Toggled low battery for %s: %s\n", locks[lock_num - 1].name, 
                       locks[lock_num - 1].low_battery ? "LOW" : "OK");
            } else {
                printf("Invalid lock number\n");
            }
        }
        else if (strcmp(command, "status") == 0) {
            display_lock_status();
        }
        else if (strcmp(command, "help") == 0) {
            printf("\n========== AVAILABLE COMMANDS ==========\n");
            printf("add <name>        - Add new lock with name\n");
            printf("remove <num>      - Remove lock by number\n");
            printf("lock <num>        - Lock specific lock\n");
            printf("unlock <num>      - Unlock specific lock\n");
            printf("jam <num>         - Set jam status for lock\n");
            printf("battery <num>     - Toggle low battery for lock\n");
            printf("status            - Display current status\n");
            printf("help              - Show this help\n");
            printf("=========================================\n\n");
        }
        else {
            printf("Unknown command. Type 'help' for available commands.\n");
        }
    }
}

/**
 * Matter attribute update callback
 */
static esp_err_t app_attribute_update_cb(attribute::callback_type_t type, uint16_t endpoint_id, uint32_t cluster_id,
                                        uint32_t attribute_id, esp_matter_attr_val_t *val, void *priv_data)
{
    ESP_LOGI(TAG, "Attribute update: endpoint: %d, cluster: %ld, attribute: %ld", endpoint_id, cluster_id, attribute_id);
    
    // Find the lock by endpoint ID
    int lock_index = -1;
    for (int i = 0; i < MAX_LOCKS; i++) {
        if (locks[i].enabled && locks[i].endpoint_id == endpoint_id) {
            lock_index = i;
            break;
        }
    }
    
    if (lock_index == -1) {
        ESP_LOGW(TAG, "Lock not found for endpoint %d", endpoint_id);
        return ESP_OK;
    }
    
    // Handle door lock cluster attributes
    if (cluster_id == chip::app::Clusters::DoorLock::Id) {
        if (attribute_id == chip::app::Clusters::DoorLock::Attributes::LockState::Id) {
            locks[lock_index].is_locked = (val->val.u8 == 1);
            lock_led_update(lock_index);
            ESP_LOGI(TAG, "Lock %d (%s) state changed to: %s", lock_index + 1, locks[lock_index].name,
                     locks[lock_index].is_locked ? "LOCKED" : "UNLOCKED");
        }
    }
    
    return ESP_OK;
}

/**
 * Matter identification callback
 */
static esp_err_t app_identification_cb(identification::callback_type_t type, uint16_t endpoint_id, uint8_t effect_id,
                                     uint8_t effect_variant, void *priv_data)
{
    ESP_LOGI(TAG, "Identification callback: endpoint: %d, effect: %d", endpoint_id, effect_id);
    
    // Find the lock by endpoint ID and blink its LED for identification
    for (int i = 0; i < MAX_LOCKS; i++) {
        if (locks[i].enabled && locks[i].endpoint_id == endpoint_id) {
            // Blink LED rapidly for 5 seconds for identification
            xTimerChangePeriod(locks[i].blink_timer, pdMS_TO_TICKS(100), 0);
            xTimerStart(locks[i].blink_timer, 0);
            
            // Stop after 5 seconds and restore normal status
            vTaskDelay(pdMS_TO_TICKS(5000));
            xTimerStop(locks[i].blink_timer, 0);
            lock_led_update(i);
            break;
        }
    }
    
    return ESP_OK;
}

/**
 * Main application entry point
 */
extern "C" void app_main()
{
    esp_err_t err = ESP_OK;

    /* Initialize the ESP NVS layer */
    nvs_flash_init();

    /* Initialize driver */
    lock_led_init();

    /* Create a Matter node and add the mandatory Root Node device type on endpoint 0 */
    node = node::create(&(node::config_t){
        .root_node = {
            .basic_information = {
                .vendor_name = "ESP32",
                .vendor_id = 0x131B,
                .product_name = "Matter Lock Bridge",
                .product_id = 0x0001,
            },
            .descriptor = {
                .device_type_id = ESP_MATTER_BRIDGE_DEVICE_TYPE_ID,
                .device_type_revision = 1,
            },
        },
    });

    if (!node) {
        ESP_LOGE(TAG, "Failed to create Matter node");
        return;
    }

    /* Set Matter bridge attributes */
    endpoint_t *root_endpoint = endpoint::get(node, BRIDGE_ENDPOINT_ID);
    cluster_t *descriptor_cluster = cluster::get(root_endpoint, chip::app::Clusters::Descriptor::Id);
    attribute_t *device_list_attribute = attribute::get(descriptor_cluster, chip::app::Clusters::Descriptor::Attributes::DeviceTypeList::Id);

    /* Add default locks */
    add_new_lock("Front Door");
    add_new_lock("Back Door");
    add_new_lock("Garage Door");
    add_new_lock("Side Gate");

    /* Matter start */
    err = esp_matter::start(app_attribute_update_cb);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Matter start failed: %s", esp_err_to_name(err));
        return;
    }

    /* Set identification callback */
    esp_matter::set_identification_callback(app_identification_cb);

    /* WiFi initialization */
    app_wifi_init();

    /* Print onboarding information */
    esp_matter::console::diagnostics_handler_register();
    esp_matter::console::wifi_handler_register();
    esp_matter::console::init();

    /* Create status display timer */
    status_timer = xTimerCreate(
        "status_timer",
        pdMS_TO_TICKS(10000), // 10 second interval
        pdTRUE,               // Auto-reload
        NULL,
        status_display_timer_callback
    );
    xTimerStart(status_timer, 0);

    ESP_LOGI(TAG, "ESP32 Matter Lock Bridge started successfully");
    printf("\n========== ESP32 MATTER LOCK BRIDGE ==========\n");
    printf("Welcome to Matter Lock Bridge!\n");
    printf("Type 'help' for available commands.\n");
    printf("==============================================\n\n");

    /* Main loop for serial command processing */
    while (1) {
        process_serial_commands();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}