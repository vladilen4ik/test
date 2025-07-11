# 🔌 Hardware Wiring Guide - ESP32 Matter Lock Bridge

This guide provides detailed wiring instructions for connecting LED status indicators to your ESP32 development board.

## 📋 Components List

### Required Components
| Component | Quantity | Specifications | Notes |
|-----------|----------|----------------|-------|
| ESP32 Development Board | 1 | Any ESP32 variant | ESP32, ESP32-S2, ESP32-S3, ESP32-C3, ESP32-C6 |
| Red LEDs | 8 | 5mm, 20mA max | Standard red LEDs, 2V forward voltage |
| Resistors | 8 | 220Ω, 1/4W | Current limiting resistors |
| Breadboard | 1 | Half or full size | For prototyping connections |
| Jumper Wires | 20+ | Male-to-male | For connections |
| USB Cable | 1 | Micro USB or USB-C | Depends on ESP32 board type |

### Optional Components
| Component | Quantity | Purpose |
|-----------|----------|---------|
| LED holders | 8 | Mount LEDs in enclosure |
| PCB | 1 | Permanent installation |
| Enclosure | 1 | Professional housing |

## 🔧 Wiring Connections

### GPIO Pin Assignments
```
Lock 1: GPIO 4  → LED 1
Lock 2: GPIO 5  → LED 2  
Lock 3: GPIO 16 → LED 3
Lock 4: GPIO 17 → LED 4
Lock 5: GPIO 18 → LED 5
Lock 6: GPIO 19 → LED 6
Lock 7: GPIO 21 → LED 7
Lock 8: GPIO 22 → LED 8
```

### Circuit Diagram (Per LED)
```
ESP32 GPIO Pin → 220Ω Resistor → LED Anode (+) → LED Cathode (-) → ESP32 GND

Example for Lock 1:
GPIO 4 ──[220Ω]──|>|── GND
                  LED
```

### Complete Wiring Table
| Lock # | ESP32 GPIO | Resistor | LED Anode | LED Cathode | Ground |
|--------|------------|----------|-----------|-------------|---------|
| 1 | GPIO 4 | 220Ω | Red Wire | Black Wire | GND |
| 2 | GPIO 5 | 220Ω | Red Wire | Black Wire | GND |
| 3 | GPIO 16 | 220Ω | Red Wire | Black Wire | GND |
| 4 | GPIO 17 | 220Ω | Red Wire | Black Wire | GND |
| 5 | GPIO 18 | 220Ω | Red Wire | Black Wire | GND |
| 6 | GPIO 19 | 220Ω | Red Wire | Black Wire | GND |
| 7 | GPIO 21 | 220Ω | Red Wire | Black Wire | GND |
| 8 | GPIO 22 | 220Ω | Red Wire | Black Wire | GND |

## 🛠️ Step-by-Step Assembly

### Step 1: Prepare Components
1. **Identify LED polarity**:
   - **Longer leg** = Anode (+)
   - **Shorter leg** = Cathode (-)
   - **Flat edge** on LED body = Cathode side

2. **Test LEDs** (optional):
   - Use 3V battery + 220Ω resistor to test each LED
   - Ensure all LEDs light up properly

### Step 2: Breadboard Layout
```
Power Rails:
Top Rail: +3.3V (from ESP32)
Bottom Rail: GND (from ESP32)

LED Placement:
Row 1: LEDs 1-4
Row 2: LEDs 5-8

Resistor Placement:
Between GPIO pins and LED anodes
```

### Step 3: Wire Connections

#### Connect Power Rails
```bash
ESP32 3.3V → Breadboard + rail
ESP32 GND → Breadboard - rail (ground rail)
```

#### Connect Each LED Circuit
For each lock (example for Lock 1):
```bash
1. Insert LED 1 into breadboard
   - Anode (long leg) → Row A
   - Cathode (short leg) → Ground rail

2. Insert 220Ω resistor
   - One end → Row A (same as LED anode)
   - Other end → New row (e.g., Row B)

3. Connect jumper wire
   - ESP32 GPIO 4 → Row B (resistor end)

4. Repeat for all 8 LEDs
```

### Step 4: Verify Connections
1. **Visual inspection**:
   - Check all connections are secure
   - Verify LED polarity
   - Ensure no short circuits

2. **Continuity test** (with multimeter):
   - GPIO to LED anode (through resistor)
   - LED cathode to GND

## ⚡ Electrical Specifications

### Current Calculations
```
LED Forward Voltage: ~2.0V (red LED)
ESP32 GPIO Voltage: 3.3V
Resistor: 220Ω

Current per LED = (3.3V - 2.0V) / 220Ω = 5.9mA
Total Current (8 LEDs on) = 8 × 5.9mA = 47.2mA
```

### Power Requirements
| Component | Voltage | Current | Power |
|-----------|---------|---------|-------|
| ESP32 Core | 3.3V | ~120mA | ~400mW |
| Single LED | 2.0V | 5.9mA | 12mW |
| All LEDs (8) | - | 47.2mA | 96mW |
| **Total System** | **5V USB** | **~200mA** | **~1W** |

## 🔍 Alternative Wiring Options

### Option 1: Direct GPIO Connection (Current Implementation)
```
ESP32 GPIO → 220Ω → LED → GND
```
**Pros**: Simple, direct control
**Cons**: Uses 8 GPIO pins

### Option 2: Shift Register (74HC595)
```
ESP32 (3 pins) → 74HC595 → 8 LEDs
```
**Pros**: Only uses 3 GPIO pins
**Cons**: More complex, additional IC required

### Option 3: I2C LED Driver (PCA9685)
```
ESP32 (2 pins) → PCA9685 → 16 LEDs
```
**Pros**: Only 2 GPIO pins, supports PWM dimming
**Cons**: More expensive, requires I2C setup

## 🛡️ Safety Considerations

### Electrical Safety
- **Never exceed GPIO current limits** (40mA per pin max)
- **Always use current limiting resistors**
- **Check polarity** before powering on
- **Avoid short circuits** between power and ground

### Resistor Selection
| LED Color | Forward Voltage | Recommended Resistor |
|-----------|----------------|---------------------|
| Red | 1.8-2.2V | 220Ω |
| Green | 2.0-2.4V | 180Ω |
| Blue | 2.8-3.4V | 100Ω |
| White | 2.8-3.6V | 82Ω |

### Heat Dissipation
- **220Ω resistors** dissipate ~8mW each (minimal heat)
- **Ensure good ventilation** in enclosed designs
- **Monitor temperature** during extended operation

## 🔧 Troubleshooting

### LEDs Not Lighting
1. **Check power supply**:
   ```bash
   # Measure voltages
   ESP32 3.3V rail: Should be ~3.3V
   ESP32 GND: Should be 0V
   ```

2. **Verify GPIO output**:
   ```bash
   # In serial monitor, test GPIO
   lock 1    # Should turn on LED 1
   unlock 1  # Should turn off LED 1
   ```

3. **Test LED independently**:
   ```bash
   # Connect LED directly to 3.3V through 220Ω resistor
   3.3V → 220Ω → LED → GND
   ```

### Dim LEDs
- **Check resistor value** (should be 220Ω)
- **Verify supply voltage** (should be 3.3V)
- **Check for loose connections**

### Flickering LEDs
- **Check breadboard connections**
- **Verify stable power supply**
- **Look for electromagnetic interference**

### LEDs Stay On
- **Check for GPIO pin conflicts** in code
- **Verify proper GPIO initialization**
- **Test with minimal code** (LED blink example)

## 📐 PCB Design Considerations

For permanent installations:

### PCB Layout Tips
1. **Keep traces short** between GPIO and resistors
2. **Use ground plane** for better signal integrity  
3. **Add test points** for debugging
4. **Include pull-down resistors** (10kΩ) on GPIO pins
5. **Add bypass capacitors** (100nF) near ESP32

### Component Placement
```
ESP32 Module → Resistor Array → LED Array
     ↓
   Ground plane (bottom layer)
```

### Connector Options
- **Terminal blocks** for field connections
- **JST connectors** for removable LEDs
- **Pin headers** for development/testing

## 📱 Testing Procedures

### Initial Testing
1. **Power on ESP32** without LEDs connected
2. **Verify all GPIO pins** output 0V initially  
3. **Connect one LED** and test
4. **Gradually add remaining LEDs**

### Functional Testing
```bash
# Test each lock individually
lock 1; unlock 1    # Test Lock 1 LED
lock 2; unlock 2    # Test Lock 2 LED
# ... repeat for all locks

# Test special states
jam 1              # Test fast blink
battery 1          # Test slow blink
```

### Performance Testing
```bash
# Test rapid state changes
lock 1; unlock 1; lock 1; unlock 1

# Test multiple locks simultaneously  
lock 1; lock 2; lock 3; lock 4
```

## 🎨 Enclosure Design

### LED Mounting
- **5mm LED holders** for professional appearance
- **Light pipes** to extend LED visibility
- **Diffusers** for softer light indication
- **Labels** for each lock identification

### Ventilation
- **Cooling slots** for ESP32 heat dissipation
- **Cable management** for clean installation
- **Access ports** for USB programming

### Material Selection
- **ABS plastic** for durability
- **Transparent/translucent** areas for LED visibility
- **Non-conductive** materials for safety

---

## 📞 Support

For wiring questions:
- Double-check the pin assignments in the code
- Verify component specifications
- Test each circuit individually before connecting all
- Use a multimeter to verify connections

**Remember**: Always disconnect power when making wiring changes!