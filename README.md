# ESP32 HomeKit Lock Monitor

A comprehensive ESP32-based HomeKit compatible system that can find, monitor, and control multiple smart locks. This project creates a HomeKit bridge that manages up to 4 lock accessories and provides real-time status monitoring.

## Features

✅ **HomeKit Compatible** - Full Apple HomeKit integration  
✅ **Multiple Lock Support** - Monitor up to 4 locks simultaneously  
✅ **Real-time Status Display** - Continuous lock status monitoring  
✅ **Serial Command Interface** - Manual control via Serial Monitor  
✅ **Status Indicators** - Battery, fault, and jam detection  
✅ **WiFi Configuration** - Easy WiFi setup and management  
✅ **HomeKit Bridge Mode** - Single device manages multiple accessories  
✅ **Realistic Operation Simulation** - Simulates actual lock operation timing  

## Hardware Requirements

- **ESP32 Development Board** (ESP32, ESP32-S2, ESP32-S3, ESP32-C3, or ESP32-C6)
- **LED** (optional) - Status indicator (Pin 2)
- **Push Button** (optional) - Control button (Pin 0)
- **USB Cable** - For programming and power
- **WiFi Network** - For HomeKit connectivity

## Software Requirements

- **Arduino IDE** (1.8.0 or later)
- **ESP32 Board Package** (v3.0.2 to v3.2.0)
- **HomeSpan Library** (v2.1.2 or later)

## Installation

### 1. Install Arduino IDE and ESP32 Support

1. Download and install [Arduino IDE](https://www.arduino.cc/en/software)
2. Open Arduino IDE → Preferences
3. Add ESP32 board manager URL:
   ```
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```
4. Go to Tools → Board → Boards Manager
5. Search for "esp32" and install "ESP32 by Espressif Systems"

### 2. Install HomeSpan Library

1. Open Arduino IDE
2. Go to Tools → Manage Libraries
3. Search for "HomeSpan"
4. Install "HomeSpan by HomeSpan" (latest version)

### 3. Upload the Code

1. Download all files to the same folder:
   - `ESP32_HomeKit_Lock_Monitor.ino`
   - `LockService.h`
2. Open `ESP32_HomeKit_Lock_Monitor.ino` in Arduino IDE
3. Update WiFi credentials:
   ```cpp
   const char* wifi_ssid = "YOUR_WIFI_SSID";
   const char* wifi_password = "YOUR_WIFI_PASSWORD";
   ```
4. Select your ESP32 board (Tools → Board)
5. Select the correct port (Tools → Port)
6. Click Upload

## HomeKit Setup

### Pairing with HomeKit

1. Open the **Home** app on your iPhone/iPad
2. Tap the **+** icon and select "Add Accessory"
3. When prompted, tap "More options..."
4. Look for "ESP32 Lock Bridge" in the nearby accessories list
5. Enter the setup code: **466-37-726**
6. Follow the on-screen instructions to complete pairing

### Alternative: QR Code Setup

You can generate a QR code for easier pairing. The HomeKit setup information:
- **Setup Code**: 466-37-726
- **Setup ID**: LOCK
- **Category**: Bridges (2)

## Usage

### Serial Monitor Commands

Open the Serial Monitor (115200 baud) and use these commands:

| Command | Description | Example |
|---------|-------------|---------|
| `status` | Display current lock status | `status` |
| `lock [1-4]` | Lock specific door | `lock 1` |
| `unlock [1-4]` | Unlock specific door | `unlock 2` |
| `help` | Show available commands | `help` |
| `reset` | Reset HomeSpan configuration | `reset` |
| `wifi` | Enter WiFi configuration mode | `wifi` |

### Lock Configuration

The system comes pre-configured with 4 locks:

1. **Lock 1**: Front Door
2. **Lock 2**: Back Door  
3. **Lock 3**: Garage
4. **Lock 4**: Side Gate

You can modify the lock names in the code:
```cpp
String lockNames[MAX_LOCKS] = {"Front Door", "Back Door", "Garage", "Side Gate"};
```

### Status Display Example

```
========== LOCK STATUS ==========
Time: 125 seconds
WiFi Status: Connected
HomeKit Paired: Yes
---------------------------------
Lock 1 (Front Door): LOCKED 🔒
Lock 2 (Back Door): UNLOCKED 🔓
Lock 3 (Garage): LOCKED 🔒
Lock 4 (Side Gate): UNLOCKED 🔓
=================================
```

## HomeKit Integration

### Available Lock Controls

Each lock appears as a separate accessory in HomeKit with:

- **Lock/Unlock Control** - Toggle lock state
- **Current State Display** - Shows current lock status
- **Battery Status** - Low battery warnings (simulated)
- **Fault Detection** - Reports lock faults
- **Jam Detection** - Reports jammed locks

### Siri Integration

Once paired, you can control locks with Siri:

- "Hey Siri, lock the front door"
- "Hey Siri, unlock the garage"
- "Hey Siri, what's the status of the back door?"
- "Hey Siri, are all the doors locked?"

### HomeKit Automations

Create automations in the Home app:

- **Departure Automation**: Lock all doors when leaving home
- **Arrival Automation**: Unlock specific doors when arriving
- **Bedtime Automation**: Ensure all doors are locked at night
- **Security Automation**: Get notifications if doors unlock unexpectedly

## Advanced Configuration

### Changing the Number of Locks

To modify the number of locks, update these values:

```cpp
#define MAX_LOCKS 4  // Change to desired number (1-8 recommended)
```

And update the names array:
```cpp
String lockNames[MAX_LOCKS] = {"Lock 1", "Lock 2", "Lock 3", "Lock 4"};
```

### WiFi Configuration Mode

If you need to change WiFi settings after upload:

1. Press and hold the control button (Pin 0) for 10 seconds
2. ESP32 will create a WiFi hotspot named "HomeSpan-Setup"
3. Connect to this network with your phone
4. Open a web browser and go to 192.168.4.1
5. Enter new WiFi credentials

### Custom Setup Code

To change the HomeKit setup code, modify:

```cpp
homeSpan.setSetupCode("466-37-726");  // Change to your preferred code
```

Format: XXX-XX-XXX (8 digits total)

## Troubleshooting

### Common Issues

**Q: ESP32 won't connect to WiFi**
- A: Check SSID and password spelling
- A: Ensure 2.4GHz WiFi (5GHz not supported)
- A: Try WiFi configuration mode

**Q: HomeKit won't find the device**
- A: Ensure ESP32 and iPhone are on same network
- A: Check Serial Monitor for error messages
- A: Try restarting the ESP32

**Q: Locks don't respond in HomeKit**
- A: Check HomeKit pairing status
- A: Verify Serial Monitor shows lock operations
- A: Try unpairing and re-pairing

**Q: Serial Monitor shows errors**
- A: Check baud rate is set to 115200
- A: Ensure HomeSpan library is properly installed
- A: Verify ESP32 board selection

### Diagnostic Commands

Use these Serial Monitor commands for troubleshooting:

```
status          # Check system status
reset           # Reset HomeSpan
wifi            # Reconfigure WiFi
help            # Show all commands
```

## Technical Details

### HomeKit Services Used

- **Bridge Service** - Main accessory coordinator
- **Lock Management Service** - Individual lock control
- **Accessory Information Service** - Device identification

### HomeKit Characteristics

Each lock implements:
- `LockCurrentState` - Current lock status (read-only)
- `LockTargetState` - Desired lock status (read/write)
- `StatusLowBattery` - Battery status indicator
- `StatusFault` - Fault detection
- `StatusJammed` - Jam detection

### Performance Specifications

- **Lock Operation Time**: 1.5-3 seconds (simulated)
- **Status Update Interval**: 5 seconds
- **Battery Check Interval**: 30 seconds
- **WiFi Reconnection**: Automatic
- **Memory Usage**: ~45% of ESP32 flash

## Contributing

Feel free to contribute improvements:

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## License

This project is released under the MIT License. See LICENSE file for details.

## Disclaimer

This is a demonstration/educational project. For actual security applications, please use certified commercial lock systems. The simulated fault detection and battery monitoring are for demonstration purposes only.

## Support

For questions and support:
- Check the troubleshooting section above
- Review HomeSpan documentation
- Open an issue on the project repository

---

**Made with ❤️ for the HomeKit community**