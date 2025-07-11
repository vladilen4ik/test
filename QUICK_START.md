# 🚀 Quick Start Guide - ESP32 HomeKit Lock Monitor

Get your ESP32 HomeKit Lock Monitor running in **5 minutes**!

## 📋 What You Need

- ✅ ESP32 development board
- ✅ USB cable
- ✅ Computer with Arduino IDE
- ✅ WiFi network (2.4GHz)
- ✅ iPhone/iPad with Home app

## ⚡ 5-Minute Setup

### Step 1: Install Software (2 minutes)

1. **Download Arduino IDE** from [arduino.cc](https://www.arduino.cc/en/software)

2. **Add ESP32 support:**
   - Open Arduino IDE
   - Go to `File → Preferences`
   - Add this URL in "Additional Boards Manager URLs":
     ```
     https://espressif.github.io/arduino-esp32/package_esp32_index.json
     ```
   - Go to `Tools → Board → Boards Manager`
   - Search "ESP32" and install "ESP32 by Espressif Systems"

3. **Install HomeSpan library:**
   - Go to `Tools → Manage Libraries`
   - Search "HomeSpan" and install "HomeSpan by HomeSpan"

### Step 2: Upload Code (2 minutes)

1. **Download the files:**
   - `ESP32_HomeKit_Lock_Monitor.ino`
   - `LockService.h`
   - Put both files in the same folder

2. **Configure WiFi:**
   - Open `ESP32_HomeKit_Lock_Monitor.ino`
   - Change these lines:
     ```cpp
     const char* wifi_ssid = "YOUR_WIFI_SSID";        // ← Your WiFi name
     const char* wifi_password = "YOUR_WIFI_PASSWORD"; // ← Your WiFi password
     ```

3. **Upload to ESP32:**
   - Connect ESP32 to computer via USB
   - Select `Tools → Board → ESP32 Dev Module`
   - Select your USB port in `Tools → Port`
   - Click **Upload** button ⬆️

### Step 3: Pair with HomeKit (1 minute)

1. **Open Serial Monitor** (Tools → Serial Monitor, set to 115200 baud)
2. **Wait for "HomeKit ready"** message
3. **On your iPhone:**
   - Open **Home** app
   - Tap **+** → "Add Accessory"
   - Tap "More options..."
   - Select "ESP32 Lock Bridge"
   - Enter setup code: **466-37-726**
   - Complete setup

## 🎉 You're Done!

Your ESP32 now appears in HomeKit with 4 locks:
- 🚪 Front Door
- 🚪 Back Door  
- 🏠 Garage
- 🚪 Side Gate

## 🎮 Quick Commands

Open Serial Monitor and try:
- `status` - See all lock states
- `lock 1` - Lock front door
- `unlock 2` - Unlock back door
- `help` - See all commands

## 📱 Control with Siri

Say to your iPhone:
- "Hey Siri, lock the front door"
- "Hey Siri, unlock the garage"
- "Hey Siri, are all doors locked?"

## 🔧 Troubleshooting

**ESP32 won't upload?**
- Hold BOOT button while clicking upload
- Try different USB cable
- Check USB port selection

**WiFi won't connect?**
- Double-check WiFi name and password
- Make sure it's 2.4GHz WiFi (not 5GHz)
- Check WiFi signal strength

**HomeKit won't pair?**
- Restart ESP32 (unplug and plug back in)
- Make sure iPhone and ESP32 are on same WiFi
- Check Serial Monitor for error messages

**Need help?**
- Type `help` in Serial Monitor
- Check the full README.md for detailed troubleshooting

## 🎯 Next Steps

- **Customize lock names** in the code
- **Add more locks** (up to 8 supported)
- **Create HomeKit automations** in Home app
- **Connect real lock hardware** to GPIO pins

## 💡 Pro Tips

- **Status LED**: Pin 2 shows HomeKit connection status
- **Control button**: Pin 0 can reset WiFi settings
- **Update remotely**: Use HomeSpan's built-in OTA updates
- **Monitor logs**: Serial Monitor shows real-time activity

## 🛡️ Security Note

This is a **demonstration project**. For real security applications, use certified commercial smart locks. The code simulates lock operations and is perfect for learning HomeKit development.

---

**🎊 Congratulations! You now have a working ESP32 HomeKit Lock Monitor!**

*Questions? Check the full documentation in README.md*