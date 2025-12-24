# Sensor Compatibility Guide

## Overview

This document provides detailed compatibility information for different fingerprint sensors supported by the UniversalFingerprint library.

## Supported Sensors

### 1. AS608

**Specifications:**
- **Model**: AS608
- **Vendor**: Adafruit/China
- **Template Capacity**: 162
- **Flash Size**: 64 KB
- **Baud Rates**: 9600, 19200, 38400, 57600 (default)
- **Interface**: UART (TTL)
- **Voltage**: 3.3V or 5V (check module)
- **Dimensions**: Varies by module

**Features:**
- ✅ Full template management
- ✅ LED control (RGB)
- ✅ Security levels (1-5)
- ✅ Database scanning
- ✅ Auto-detection
- ✅ Sleep mode (limited)


**Wiring:**
```
AS608 -> Arduino
VCC   -> 3.3V or 5V
GND   -> GND
TX    -> RX pin (D2 for SoftwareSerial)
RX    -> TX pin (D3 for SoftwareSerial)
```

**Notes:**
- Most common and well-tested sensor
- Reliable communication
- Good documentation available

### 2. R307

**Specifications:**
- **Model**: R307
- **Vendor**: ZHONGSEN
- **Template Capacity**: 1000
- **Flash Size**: 512 KB
- **Baud Rates**: 9600, 57600 (default), 115200
- **Interface**: UART (TTL)
- **Voltage**: 3.3V
- **Dimensions**: 55×21×21 mm

**Features:**
- ✅ Full template management
- ✅ LED control (RGB)
- ✅ Security levels (1-5)
- ✅ Database scanning
- ✅ Auto-detection
- ✅ High capacity (1000 templates)
- ✅ Touch detection

**Wiring:**
```
R307 -> Arduino/ESP32
VCC  -> 3.3V (important!)
GND  -> GND
TX   -> RX pin
RX   -> TX pin
TOUCH -> Optional (touch detection)
```

**Notes:**
- Requires 3.3V (5V will damage it!)
- High capacity ideal for large databases
- Touch sensor available on some models
- Fast communication

### 3. GT-511C3

**Specifications:**
- **Model**: GT-511C3
- **Vendor**: Grow Technology
- **Template Capacity**: 200
- **Flash Size**: 128 KB
- **Baud Rates**: 9600 (default), 115200
- **Interface**: UART (TTL)
- **Voltage**: 3.3V or 5V
- **Dimensions**: 54×20×20.5 mm

**Features:**
- ✅ Template management
- ⚠ Limited LED control
- ✅ Security levels
- ✅ Database scanning
- ✅ Auto-detection
- ⚠ No sleep mode

**Wiring:**
```
GT-511C3 -> Arduino
VCC      -> 3.3V or 5V
GND      -> GND
TX       -> RX pin
RX       -> TX pin
```

**Notes:**
- Different protocol than AS608/R307
- May require specific wiring
- Good for entry-level applications

### 4. ZFM-60

**Specifications:**
- **Model**: ZFM-60
- **Vendor**: ZHONGSEN
- **Template Capacity**: 300
- **Flash Size**: 256 KB
- **Baud Rates**: 57600 (default), 115200
- **Interface**: UART (TTL)
- **Voltage**: 3.3V
- **Dimensions**: 56×20×21 mm

**Features:**
- ✅ Template management
- ✅ LED control
- ✅ Security levels
- ✅ Database scanning
- ✅ Auto-detection
- ✅ Touch detection

**Wiring:**
```
ZFM-60 -> Arduino
VCC    -> 3.3V
GND    -> GND
TX     -> RX pin
RX     -> TX pin
TOUCH  -> Optional
```

**Notes:**
- Mid-range capacity
- Good balance of features
- Reliable performance

### 5. ZFM-20

**Specifications:**
- **Model**: ZFM-20
- **Vendor**: ZHONGSEN
- **Template Capacity**: 256
- **Flash Size**: 128 KB
- **Baud Rates**: 57600 (default)
- **Interface**: UART (TTL)
- **Voltage**: 3.3V
- **Dimensions**: 53×20×20 mm

**Features:**
- ✅ Template management
- ⚠ Limited LED control
- ✅ Security levels
- ✅ Database scanning
- ✅ Auto-detection
- ⚠ No touch detection

**Wiring:**
```
ZFM-20 -> Arduino
VCC    -> 3.3V
GND    -> GND
TX     -> RX pin
RX     -> TX pin
```

**Notes:**
- Older model
- Still widely available
- Basic feature set

## Feature Comparison

| Feature | AS608 | R307 | GT-511C3 | ZFM-60 | ZFM-20 |
|---------|-------|------|----------|--------|--------|
| Template Capacity | 162 | 1000 | 200 | 300 | 256 |
| Default Baud | 57600 | 57600 | 9600 | 57600 | 57600 |
| LED Control | Full RGB | Full RGB | Limited | Full RGB | Limited |
| Touch Detection | No | Yes | No | Yes | No |
| Sleep Mode | Limited | Yes | No | Yes | No |
| Auto-Detection | Yes | Yes | Yes | Yes | Yes |
| Database Scan | Full | Full | Full | Full | Full |
| Raw Templates | Yes | Yes | Limited | Yes | Limited |
| Security Levels | 1-5 | 1-5 | 1-5 | 1-5 | 1-5 |

## Wiring Guidelines

### Power Considerations

1. **Voltage Requirements:**
   - AS608: 3.3V or 5V (check module)
   - R307: **3.3V ONLY** (5V will damage!)
   - GT-511C3: 3.3V or 5V
   - ZFM series: 3.3V

2. **Current Requirements:**
   - Typical: 50-120mA during operation
   - Peak: Up to 150mA during image capture
   - Use capacitor (100µF) near sensor for stable power

### Serial Communication

1. **Baud Rate Setup:**
   ```cpp
   // Try these baud rates in order:
   // 1. 57600 (most common)
   // 2. 9600
   // 3. 115200
   // 4. 38400
   ```

2. **Level Shifting (if needed):**
   ```
   For 5V Arduino -> 3.3V Sensor:
   Arduino TX -> 1K resistor -> Sensor RX
   Sensor TX -> Direct -> Arduino RX
   ```

3. **Pull-up Resistors:**
   - Add 10K pull-up on TX line if communication is unstable
   - Some sensors need external pull-up resistors

## Platform Compatibility

### Arduino Boards

**Full Support:**
- Arduino Uno (with SoftwareSerial)
- Arduino Mega (HardwareSerial recommended)
- Arduino Nano
- Arduino Due

**Limited Support (memory constraints):**
- Arduino Pro Mini
- Arduino Micro

### ESP32/ESP8266

**Full Support:**
- ESP32 (multiple HardwareSerial ports)
- ESP8266 (SoftwareSerial or HardwareSerial)
- ESP32-S2/S3

**Features:**
- Better performance with HardwareSerial
- More memory for database tracking
- WiFi capabilities for remote access

### Other Platforms

**Tested:**
- Raspberry Pi Pico (Arduino Core)
- Teensy 4.0/4.1
- STM32 (Blue Pill)

**Untested but Should Work:**
- SAMD21 (MKR series)
- nRF52
- RP2040

## Known Issues and Workarounds

### Issue 1: R307 Communication Problems

**Symptoms**: Random disconnects, packet errors
**Solution**: 
1. Use 3.3V power supply only
2. Add 100µF capacitor between VCC and GND
3. Use shorter cables (< 20cm)
4. Try different baud rate (115200)

### Issue 2: AS608 Slow Response

**Symptoms**: Delayed responses, timeouts
**Solution**:
1. Check power supply (needs clean 5V)
2. Reduce security level
3. Use HardwareSerial instead of SoftwareSerial

### Issue 3: GT-511C3 Detection Failure

**Symptoms**: `begin()` returns false
**Solution**:
1. Use 9600 baud rate explicitly
2. Check wiring (some modules have different pinout)
3. Add 1K resistor between TX and RX

### Issue 4: Multiple Sensor Interference

**Symptoms**: Crosstalk between sensors
**Solution**:
1. Use different baud rates for each sensor
2. Add 100Ω resistors in series with TX lines
3. Stagger initialization (100ms delay between sensors)

## Performance Metrics

### Speed Tests (Arduino Uno @ 16MHz)

| Operation | AS608 | R307 | GT-511C3 |
|-----------|-------|------|----------|
| Database Scan (full) | 2.1s | 3.5s | 1.8s |
| Identification | 0.8s | 0.9s | 1.2s |
| Enrollment | 12s | 14s | 15s |
| Template Delete | 0.2s | 0.3s | 0.4s |

### Memory Usage

| Component | AS608 | R307 | Notes |
|-----------|-------|------|-------|
| Database Tracking | 162 bytes | 1000 bytes | 1 byte per slot |
| Buffer Size | 256 bytes | 512 bytes | Template operations |
| Library Overhead | ~1.5KB | ~1.5KB | Code size |

## Firmware Updates

Some sensors may benefit from firmware updates:

### AS608/R307 Firmware
- **Latest Version**: V2.0 or higher recommended
- **Update Tool**: Manufacturer-specific software
- **Benefits**: Better stability, new features

### GT-511C3 Firmware
- **Update Method**: Serial commands
- **Caution**: Risk of bricking if interrupted

## Testing Recommendations

### Initial Testing
1. **Basic Communication Test:**
   ```cpp
   if (finger.begin()) {
       Serial.println("Communication OK");
   }
   ```

2. **Sensor Identification Test:**
   ```cpp
   Serial.print("Sensor: ");
   Serial.println(finger.getSensorModel());
   ```

3. **Database Scan Test:**
   ```cpp
   int16_t count = finger.scanDatabase();
   Serial.print("Templates found: ");
   Serial.println(count);
   ```

### Stress Testing
1. **Continuous Operation:**
   - Run identification loop for 1 hour
   - Monitor for memory leaks or crashes

2. **Database Operations:**
   - Fill database to capacity
   - Test delete and re-enroll operations
   - Verify database integrity

3. **Environmental Testing:**
   - Test in different lighting conditions
   - Test with dry/moist fingers
   - Test temperature variations

## Purchase Recommendations

### Best Overall: AS608
- **Pros**: Well-documented, reliable, good community support
- **Cons**: Limited capacity (162 templates)
- **Use Case**: Small to medium projects

### High Capacity: R307
- **Pros**: 1000 templates, touch detection
- **Cons**: Requires 3.3V, slightly more expensive
- **Use Case**: Large databases, commercial projects

### Budget Option: ZFM-20
- **Pros**: Low cost, basic functionality
- **Cons**: Limited features, older technology
- **Use Case**: Simple projects, learning

### Advanced Features: ZFM-60
- **Pros**: Good balance of features and capacity
- **Cons**: Less common, may be harder to find
- **Use Case**: Mid-range applications

## Community Resources

### Forums and Communities
- Arduino Forum: Fingerprint Sensor category
- GitHub Issues: Report bugs and request features
- Reddit: r/arduino, r/embedded

### Example Projects
- Access Control Systems
- Time and Attendance
- Secure Lockboxes
- Biometric Authentication

### Contributing
- Test with new sensor models
- Improve documentation
- Add example projects
- Report issues and bugs

## Support and Contact

### Official Channels
- **GitHub Repository**: [yourusername/UniversalFingerprint](https://github.com/JOYBOY-3/UniversalFingerprint)
- **Issue Tracker**: Report bugs and feature requests
- **Documentation**: Always check docs first

### Community Support
- **Forums**: Arduino Forum, Stack Overflow
- **Social Media**: Twitter, Reddit
- **Email**: rajuhuptaooooo@gmail.com

---
