/*
 * ESP32 HomeKit Lock Monitor
 * 
 * This sketch creates a HomeKit bridge that can discover and monitor
 * multiple lock accessories. It provides status display for each lock
 * and can handle multiple lock services.
 * 
 * Required Library: HomeSpan (install via Arduino Library Manager)
 * 
 * Setup Instructions:
 * 1. Install HomeSpan library
 * 2. Set WiFi credentials below
 * 3. Upload to ESP32
 * 4. Open Serial Monitor (115200 baud)
 * 5. Follow pairing instructions
 * 
 * HomeKit Setup Code: 466-37-726
 */

#include "HomeSpan.h"
#include "LockService.h"

// WiFi credentials - UPDATE THESE
const char* wifi_ssid = "YOUR_WIFI_SSID";
const char* wifi_password = "YOUR_WIFI_PASSWORD";

// Lock configuration
#define MAX_LOCKS 4
#define STATUS_LED_PIN 2
#define CONTROL_BUTTON_PIN 0

// Global variables for lock status tracking
bool lockStates[MAX_LOCKS] = {false, false, false, false}; // false = unlocked, true = locked
String lockNames[MAX_LOCKS] = {"Front Door", "Back Door", "Garage", "Side Gate"};
unsigned long lastStatusUpdate = 0;
const unsigned long STATUS_UPDATE_INTERVAL = 5000; // 5 seconds

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=== ESP32 HomeKit Lock Monitor ===");
  Serial.println("Initializing HomeSpan...");

  // Initialize HomeSpan
  homeSpan.begin(Category::Bridges, "ESP32 Lock Bridge");
  
  // Set WiFi credentials
  homeSpan.setWifiCredentials(wifi_ssid, wifi_password);
  
  // Configure HomeKit settings
  homeSpan.setSetupCode("466-37-726");
  homeSpan.setQRID("LOCK");
  homeSpan.setHostNameSuffix("-locks");
  
  // Set control pin for HomeSpan (optional)
  homeSpan.setControlPin(CONTROL_BUTTON_PIN);
  
  // Set status LED pin (optional)
  homeSpan.setStatusPin(STATUS_LED_PIN);
  
  // Enable logging
  homeSpan.setLogLevel(1);

  // Create Bridge Accessory (required for multiple accessories)
  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify();
      new Characteristic::Manufacturer("HomeSpan");
      new Characteristic::SerialNumber("HS-LOCK-001");
      new Characteristic::Model("ESP32 Lock Bridge");
      new Characteristic::FirmwareRevision("1.0.0");

  // Create Lock Accessories
  for(int i = 0; i < MAX_LOCKS; i++) {
    createLockAccessory(i, lockNames[i]);
  }

  Serial.println("HomeSpan configuration complete!");
  Serial.println("Starting HomeKit service...");
  Serial.print("Setup Code: ");
  Serial.println("466-37-726");
  Serial.println("\nWaiting for WiFi connection...");
  
  // Initialize status display
  displayLockStatus();
}

void loop() {
  homeSpan.poll();
  
  // Update lock status display periodically
  if(millis() - lastStatusUpdate > STATUS_UPDATE_INTERVAL) {
    displayLockStatus();
    lastStatusUpdate = millis();
  }
  
  // Check for serial commands
  handleSerialCommands();
  
  delay(10);
}

void createLockAccessory(int lockIndex, String lockName) {
  Serial.print("Creating lock accessory: ");
  Serial.println(lockName);
  
  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify();
      new Characteristic::Manufacturer("HomeSpan");
      new Characteristic::SerialNumber(("HS-LOCK-" + String(lockIndex + 2)).c_str());
      new Characteristic::Model("Smart Lock");
      new Characteristic::FirmwareRevision("1.0.0");
      new Characteristic::Name(lockName.c_str());

    new LockService(lockIndex);
}

void displayLockStatus() {
  Serial.println("\n========== LOCK STATUS ==========");
  Serial.print("Time: ");
  Serial.print(millis() / 1000);
  Serial.println(" seconds");
  Serial.print("WiFi Status: ");
  Serial.println(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected");
  Serial.print("HomeKit Paired: ");
  Serial.println(homeSpan.connected() ? "Yes" : "No");
  Serial.println("---------------------------------");
  
  for(int i = 0; i < MAX_LOCKS; i++) {
    Serial.print("Lock ");
    Serial.print(i + 1);
    Serial.print(" (");
    Serial.print(lockNames[i]);
    Serial.print("): ");
    Serial.println(lockStates[i] ? "LOCKED 🔒" : "UNLOCKED 🔓");
  }
  Serial.println("=================================\n");
}

void handleSerialCommands() {
  if(Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toLowerCase();
    
    if(command == "status") {
      displayLockStatus();
    }
    else if(command == "help") {
      displayHelp();
    }
    else if(command.startsWith("lock ")) {
      int lockNum = command.substring(5).toInt();
      if(lockNum >= 1 && lockNum <= MAX_LOCKS) {
        lockStates[lockNum - 1] = true;
        Serial.print("Locked ");
        Serial.println(lockNames[lockNum - 1]);
        displayLockStatus();
      }
    }
    else if(command.startsWith("unlock ")) {
      int lockNum = command.substring(7).toInt();
      if(lockNum >= 1 && lockNum <= MAX_LOCKS) {
        lockStates[lockNum - 1] = false;
        Serial.print("Unlocked ");
        Serial.println(lockNames[lockNum - 1]);
        displayLockStatus();
      }
    }
    else if(command == "reset") {
      Serial.println("Resetting HomeSpan...");
      homeSpan.processSerialCommand("R");
    }
    else if(command == "wifi") {
      Serial.println("WiFi configuration mode...");
      homeSpan.processSerialCommand("W");
    }
    else {
      Serial.println("Unknown command. Type 'help' for available commands.");
    }
  }
}

void displayHelp() {
  Serial.println("\n========== AVAILABLE COMMANDS ==========");
  Serial.println("status      - Display current lock status");
  Serial.println("lock [1-4]  - Lock specific door (e.g., 'lock 1')");
  Serial.println("unlock [1-4]- Unlock specific door (e.g., 'unlock 1')");
  Serial.println("reset       - Reset HomeSpan configuration");
  Serial.println("wifi        - Enter WiFi configuration mode");
  Serial.println("help        - Show this help menu");
  Serial.println("========================================\n");
}

// Callback function for lock state updates
void updateLockState(int lockIndex, bool isLocked) {
  if(lockIndex >= 0 && lockIndex < MAX_LOCKS) {
    lockStates[lockIndex] = isLocked;
    Serial.print("Lock ");
    Serial.print(lockIndex + 1);
    Serial.print(" (");
    Serial.print(lockNames[lockIndex]);
    Serial.print(") is now ");
    Serial.println(isLocked ? "LOCKED" : "UNLOCKED");
  }
}