# UniversalFingerprint Library

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)](https://github.com/JOYBOY-3/UniversalFingerprint)
[![Arduino](https://img.shields.io/badge/Arduino-Compatible-brightgreen)](https://www.arduino.cc)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-Compatible-brightgreen)](https://platformio.org)
[![Documentation](https://img.shields.io/badge/docs-API_reference-blue.svg)](https://github.com/JOYBOY-3/UniversalFingerprint/tree/main/extras/docs/API.md)
[![Examples](https://img.shields.io/badge/examples-5_included-orange.svg)](https://github.com/JOYBOY-3/UniversalFingerprint/tree/main/examples)

A universal Arduino fingerprint library supporting multiple sensor types with full memory utilization.

## ✨ Features

- **Multi-Sensor Support**: AS608 (162 templates), R307 (1000 templates), GT-511C3 (200 templates), ZFM series
- **Auto-Detection**: Automatically identifies connected sensor type
- **Full Memory Utilization**: Uses all available template slots
- **Database Management**: Complete database scanning and statistics
- **Cross-Platform**: Works with Arduino, ESP32, ESP8266, Raspberry Pi Pico
- **Advanced Features**: LED control, security levels, sleep mode
- **Professional API**: Clean, object-oriented interface with comprehensive error handling
- **Multiple Examples**: 5 comprehensive examples included
- **Complete Documentation**: API reference, getting started guide, sensor compatibility

## 📋 Supported Sensors

| Sensor Model | Template Capacity | Default Baud | Flash Size | LED Control | Touch Sensor |
|--------------|-------------------|--------------|------------|-------------|--------------|
| AS608        | 162 templates     | 57600 bps    | 64 KB      | ✅ RGB LED  | ❌ No        |
| R307         | 1000 templates    | 57600 bps    | 512 KB     | ✅ RGB LED  | ✅ Yes       |
| GT-511C3     | 200 templates     | 9600 bps     | 128 KB     | ⚠ Limited   | ❌ No        |
| ZFM-60       | 300 templates     | 57600 bps    | 256 KB     | ✅ RGB LED  | ✅ Yes       |
| ZFM-20       | 256 templates     | 57600 bps    | 128 KB     | ⚠ Limited   | ❌ No        |

## 🚀 Installation

### Arduino IDE
#### Method 1: Library Manager (Recommended)
1. Open Arduino IDE
2. Go to **Tools** → **Manage Libraries**
3. Search for "UniversalFingerprint"
4. Click "Install"

#### Method 2: Manual Installation
1. Download the [latest release](https://github.com/JOYBOY-3/UniversalFingerprint/releases)
2. Extract the ZIP file
3. Copy to Arduino libraries folder:
   - **Windows**: `Documents\Arduino\libraries\`
   - **Mac**: `Documents/Arduino/libraries/`
   - **Linux**: `~/Arduino/libraries/`
4. Restart Arduino IDE

### PlatformIO
Add to your `platformio.ini`:
```ini
lib_deps = 
    JOYBOY-3/UniversalFingerprint@^1.0.0
```

Or install via PlatformIO CLI:
```bash
pio lib install "UniversalFingerprint"
```

## 📦 Dependencies

- **Required**: [Adafruit Fingerprint Sensor Library](https://github.com/adafruit/Adafruit-Fingerprint-Sensor-Library) - For basic sensor communication

Install via Arduino Library Manager:
1. Open Arduino IDE
2. **Tools** → **Manage Libraries**
3. Search for "Adafruit Fingerprint Sensor"
4. Click "Install"

Or via PlatformIO:
```ini
lib_deps = 
    adafruit/Adafruit Fingerprint Sensor Library@^1.2.1
```

## 🔧 Quick Start

### Basic Wiring

#### AS608/R307 Wiring (Arduino Uno/Nano):
```
Fingerprint Sensor  ->  Arduino
VCC                ->  3.3V or 5V (check sensor specification)
GND                ->  GND
TX                 ->  D2 (RX pin for SoftwareSerial)
RX                 ->  D3 (TX pin for SoftwareSerial)
```

#### ESP32 Wiring:
```
Sensor  ->  ESP32
VCC     ->  3.3V
GND     ->  GND
TX      ->  GPIO16 (RX2)
RX      ->  GPIO17 (TX2)
```

**⚠ Important**: R307 sensors require 3.3V only! 5V will damage them.

### Basic Example Code

```cpp
#include <UniversalFingerprint.h>
#include <SoftwareSerial.h>

// Create software serial for fingerprint sensor
SoftwareSerial fingerSerial(2, 3); // RX, TX
UniversalFingerprint finger(&fingerSerial);

void setup() {
    Serial.begin(115200);
    while (!Serial); // Wait for serial port
    
    Serial.println("UniversalFingerprint Basic Example");
    
    // Initialize fingerprint sensor
    if (!finger.begin()) {
        Serial.println("Could not find fingerprint sensor!");
        Serial.println("Check wiring and power supply.");
        while (1); // Halt execution
    }
    
    Serial.print("Sensor detected: ");
    Serial.println(finger.getSensorModel());
    
    Serial.print("Capacity: ");
    Serial.print(finger.getCapacity());
    Serial.println(" templates");
    
    Serial.print("Library version: ");
    Serial.println(finger.getVersion());
    
    // Show database information
    finger.printDatabaseInfo();
    
    Serial.println("\nReady! Place finger on sensor to identify.");
}

void loop() {
    // Identify fingerprint
    int16_t id = finger.identify();
    
    if (id > 0) {
        Serial.print("Fingerprint matched! ID: #");
        Serial.println(id);
    } else if (id == 0) {
        Serial.println("Fingerprint not in database");
    } else {
        Serial.print("Identification error: ");
        Serial.println(finger.errorToString((UF_ErrorCode)(-id)));
    }
    
    delay(1000);
}
```

## 📚 API Reference

### Key Methods

#### Initialization
```cpp
bool begin(uint32_t password = 0x00000000, uint32_t baudRate = 57600);
```

#### Enrollment
```cpp
UF_ErrorCode enroll(uint16_t id = 0, uint8_t numScans = 2);
```

#### Identification
```cpp
int16_t identify(uint16_t* confidence = nullptr, uint8_t timeout = 10);
```

#### Verification
```cpp
UF_ErrorCode verify(uint16_t id, uint16_t* confidence = nullptr);
```

#### Database Management
```cpp
bool getDatabaseStats(UF_DatabaseStats& stats);
uint16_t findEmptySlot(uint16_t start = 1);
int16_t scanDatabase();
bool isSlotOccupied(uint16_t id);
```

#### Sensor Control
```cpp
UF_ErrorCode setLED(bool on, UF_LEDColor color = UF_LED_BLUE, uint8_t speed = 0);
UF_ErrorCode setSecurity(uint8_t level);
bool isFingerPresent();
UF_ErrorCode setBaudRate(uint32_t baudRate);
```

### Structures

#### Database Statistics
```cpp
struct UF_DatabaseStats {
    uint16_t totalSlots;      // Total available slots
    uint16_t occupiedSlots;   // Currently occupied slots
    uint16_t freeSlots;       // Free slots
    uint16_t firstFreeSlot;   // First free slot ID
    uint16_t lastFreeSlot;    // Last free slot ID
    float usagePercentage;    // Database usage percentage
};
```

#### Sensor Information
```cpp
struct UF_SensorInfo {
    UF_SensorType type;       // Sensor type
    const char* modelName;    // Model name
    const char* vendor;       // Vendor name
    uint16_t capacity;        // Template capacity
    uint16_t packetSize;      // Data packet size
    uint32_t defaultBaud;     // Default baud rate
    bool hasLED;              // Has RGB LED control
    bool hasTouchSensor;      // Has touch detection
};
```

### Error Codes
```cpp
enum UF_ErrorCode {
    UF_OK,                  // Operation successful
    UF_ERROR_COMM,          // Communication error
    UF_ERROR_NO_SENSOR,     // Sensor not found
    UF_ERROR_INVALID_ID,    // Invalid template ID
    UF_ERROR_SLOT_FULL,     // No empty slots available
    UF_ERROR_TIMEOUT,       // Operation timeout
    UF_ERROR_SENSOR_BUSY,   // Sensor is busy
    UF_ERROR_PACKET,        // Invalid packet received
    UF_ERROR_NOT_ENROLLED,  // Finger not enrolled
    UF_ERROR_NO_FINGER,     // No finger detected
    UF_ERROR_IMAGE_MESS,    // Image too messy
    UF_ERROR_FEATURE_FAIL,  // Feature extraction failed
    UF_ERROR_IMAGE_FAIL,    // Image capture failed
    UF_ERROR_DUPLICATE_ID,  // Slot already occupied
    UF_ERROR_NOT_SUPPORTED, // Feature not supported
    UF_ERROR_INVALID_PARAM  // Invalid parameter
};
```

For complete API documentation, see [API.md](extras/docs/API.md)

## 📊 Examples

The library includes 5 comprehensive examples:

1. **01_BasicEnrollment** - Simple enrollment and identification
2. **02_FullDatabaseManager** - Complete database management
3. **03_SensorDetector** - Auto-detect and report sensor information
4. **04_AdvancedOperations** - Raw template access and advanced features
5. **05_MultiSensorDemo** - Working with multiple sensors

### Database Visualization Example
```cpp
void printDatabase() {
    UF_DatabaseStats stats;
    finger.getDatabaseStats(stats);
    
    Serial.println("\n=== FINGERPRINT DATABASE ===");
    Serial.print("Sensor: ");
    Serial.println(finger.getSensorModel());
    Serial.print("Capacity: ");
    Serial.println(stats.totalSlots);
    Serial.print("Enrolled: ");
    Serial.println(stats.occupiedSlots);
    Serial.print("Free: ");
    Serial.println(stats.freeSlots);
    Serial.print("Usage: ");
    Serial.print(stats.usagePercentage, 1);
    Serial.println("%");
    
    Serial.println("\nSlot Map (O=Occupied, .=Empty):");
    for (uint16_t i = 1; i <= min(stats.totalSlots, (uint16_t)100); i++) {
        if (finger.isSlotOccupied(i)) {
            Serial.print('O');
        } else {
            Serial.print('.');
        }
        if (i % 50 == 0) Serial.println();
    }
    Serial.println("\n============================\n");
}
```

## ⚡ Performance

- **Database Scan**: ~2-3 seconds for full scan (R307: 1000 slots)
- **Identification**: < 1 second average
- **Enrollment**: 10-15 seconds per fingerprint
- **Memory Usage**: ~1KB RAM for database tracking
- **Library Size**: ~20KB flash memory

### Performance Tips
1. Use HardwareSerial when possible (faster than SoftwareSerial)
2. Cache database statistics instead of scanning repeatedly
3. Set appropriate security level (higher = more accurate but slower)
4. Use batch operations for multiple template deletions

## 🛠️ Troubleshooting

### Common Issues

#### Sensor Not Detected
**Symptoms**: `begin()` returns false
**Solutions**:
1. Check power supply (3.3V vs 5V)
2. Swap TX/RX connections
3. Try different baud rates:
```cpp
finger.begin(0x00000000, 9600); // Try 9600 baud
finger.begin(0x00000000, 57600); // Try 57600 baud
```

#### Communication Errors
**Symptoms**: Random failures, timeout errors
**Solutions**:
1. Add 1K-10K resistor between TX and RX lines
2. Use level shifter for 3.3V/5V conversion
3. Check for loose connections
4. Use shorter cables (< 30cm)

#### Memory Issues
**Symptoms**: Crashes on database scan
**Solutions**:
1. Reduce database tracking size
2. Use `scanDatabase()` only when needed
3. Enable memory optimization in IDE:
   - **Arduino IDE**: Tools → Optimization → "Smallest Code"

### Debug Mode
Enable serial debugging by defining before including the library:
```cpp
#define UF_DEBUG 1
#include <UniversalFingerprint.h>
```

## 🔍 Database Operations

### Finding Empty Slots
```cpp
// Find first empty slot
uint16_t emptySlot = finger.findEmptySlot();
if (emptySlot > 0) {
    Serial.print("First empty slot: #");
    Serial.println(emptySlot);
}

// Find last empty slot
uint16_t lastEmpty = finger.findLastEmptySlot();
if (lastEmpty > 0) {
    Serial.print("Last empty slot: #");
    Serial.println(lastEmpty);
}
```

### Database Statistics
```cpp
UF_DatabaseStats stats;
if (finger.getDatabaseStats(stats)) {
    Serial.print("Total slots: ");
    Serial.println(stats.totalSlots);
    Serial.print("Occupied: ");
    Serial.println(stats.occupiedSlots);
    Serial.print("Free: ");
    Serial.println(stats.freeSlots);
    Serial.print("Usage: ");
    Serial.print(stats.usagePercentage, 1);
    Serial.println("%");
}
```

### Batch Operations
```cpp
// Delete multiple templates
void deleteRange(uint16_t start, uint16_t end) {
    for (uint16_t i = start; i <= end; i++) {
        finger.deleteTemplate(i);
    }
}

// Clear entire database
UF_ErrorCode result = finger.clearDatabase();
if (result == UF_OK) {
    Serial.println("Database cleared successfully");
}
```

## 💡 Advanced Usage

### Multiple Sensors
```cpp
#include <UniversalFingerprint.h>
#include <SoftwareSerial.h>

// Multiple sensor setup
SoftwareSerial sensor1Serial(2, 3);
SoftwareSerial sensor2Serial(4, 5);

UniversalFingerprint sensor1(&sensor1Serial, UF_SENSOR_AS608);
UniversalFingerprint sensor2(&sensor2Serial, UF_SENSOR_R307);

void setup() {
    Serial.begin(115200);
    sensor1.begin();
    sensor2.begin();
    
    Serial.print("Sensor 1: ");
    Serial.println(sensor1.getSensorModel());
    Serial.print("Sensor 2: ");
    Serial.println(sensor2.getSensorModel());
}
```

### Custom Configuration
```cpp
// Advanced sensor configuration
void configureSensor() {
    // Set security level (1-5)
    finger.setSecurity(4); // High security
    
    // Configure LED
    finger.setLED(true, UF_LED_GREEN);
    delay(1000);
    finger.setLED(false);
    
    // Change baud rate
    finger.setBaudRate(115200);
    
    // Reinitialize with new baud rate
    finger.begin(0x00000000, 115200);
}
```

### Error Handling
```cpp
UF_ErrorCode result = finger.enroll(5);

if (result != UF_OK) {
    Serial.print("Error: ");
    Serial.println(finger.errorToString(result));
    
    switch (result) {
        case UF_ERROR_SLOT_FULL:
            // Handle full database
            uint16_t emptySlot = finger.findEmptySlot();
            if (emptySlot > 0) {
                Serial.print("Try slot #");
                Serial.println(emptySlot);
            }
            break;
            
        case UF_ERROR_TIMEOUT:
            // Handle timeout
            Serial.println("Please try again");
            break;
            
        case UF_ERROR_NO_FINGER:
            // Ask user to place finger
            Serial.println("Please place finger on sensor");
            break;
            
        default:
            // Handle other errors
            break;
    }
}
```

## 🤝 Contributing

Contributions are welcome! Please follow these steps:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit changes (`git commit -m 'Add AmazingFeature'`)
4. Push to branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

### Development Setup
```bash
# Clone repository
git clone https://github.com/JOYBOY-3/UniversalFingerprint.git
cd UniversalFingerprint

# For PlatformIO development
pio lib install "adafruit/Adafruit Fingerprint Sensor Library"
pio test  # Run tests

# For Arduino development
# Copy to Arduino libraries folder
```

### Code Style
- Follow Arduino library conventions
- Use clear, descriptive variable names
- Add comments for complex logic
- Maintain backward compatibility
- Write tests for new features

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- Based on [Adafruit Fingerprint Sensor Library](https://github.com/adafruit/Adafruit-Fingerprint-Sensor-Library)
- Inspired by various open-source fingerprint implementations
- Thanks to all contributors and testers
- Community support from Arduino Forum and GitHub

## 📞 Support

### Resources
- **Documentation**: [Complete API reference](extras/docs/API.md)
- **Examples**: See `examples/` folder for working code
- **Getting Started**: [Quick start guide](extras/docs/GettingStarted.md)
- **Sensor Compatibility**: [Compatibility guide](extras/docs/SensorCompatibility.md)

### Issue Reporting
1. Check existing [GitHub Issues](https://github.com/JOYBOY-3/UniversalFingerprint/issues)
2. Search the documentation
3. Try the examples
4. If issue persists, create new issue with:
   - Arduino board and sensor model
   - Library version
   - Error messages
   - Code example
   - Wiring diagram

### Community
- **GitHub Discussions**: Feature requests and questions
- **Arduino Forum**: Community support
- **Stack Overflow**: Technical questions

---

*Made with ❤️ for the Arduino community*

**Version**: 1.0.0  
**Last Updated**: 2024-01-01  
**Maintainer**: Sourabh Kumar Gupta rajuhuptaooooo@gmail.com
```
