# Getting Started with UniversalFingerprint

## Prerequisites

### Hardware Requirements
- Arduino board (Uno, Mega, Nano, etc.)
- Fingerprint sensor (AS608, R307, GT-511C3, etc.)
- Jumper wires
- Breadboard (optional)

### Software Requirements
- Arduino IDE 1.8.0 or later
- UniversalFingerprint library
- Adafruit Fingerprint Sensor Library

## Installation

### Step 1: Install Dependencies
1. Open Arduino IDE
2. Go to **Tools** → **Manage Libraries**
3. Search for "Adafruit Fingerprint Sensor"
4. Click Install

### Step 2: Install UniversalFingerprint
```arduino
// Method 1: Library Manager
// Search for "UniversalFingerprint" and install

// Method 2: Manual Installation
// 1. Download ZIP from GitHub
// 2. Sketch → Include Library → Add .ZIP Library
// 3. Select downloaded file
```

## Basic Wiring

### AS608/R307 Wiring
```
Sensor  ->  Arduino Uno
VCC     ->  5V
GND     ->  GND
TX      ->  D2 (RX)
RX      ->  D3 (TX)
```

### ESP32 Wiring
```
Sensor  ->  ESP32
VCC     ->  3.3V
GND     ->  GND
TX      ->  GPIO16 (RX2)
RX      ->  GPIO17 (TX2)
```

## Quick Examples

### Example 1: Basic Setup
```cpp
#include <UniversalFingerprint.h>
#include <SoftwareSerial.h>

SoftwareSerial fingerSerial(2, 3);
UniversalFingerprint finger(&fingerSerial);

void setup() {
    Serial.begin(115200);
    finger.begin();
    
    Serial.print("Sensor: ");
    Serial.println(finger.getSensorModel());
}
```

### Example 2: Enroll Fingerprint
```cpp
void enrollFinger() {
    uint16_t id = finger.findEmptySlot();
    
    if (id > 0) {
        UF_ErrorCode result = finger.enroll(id);
        
        if (result == UF_OK) {
            Serial.print("Enrolled ID #");
            Serial.println(id);
        }
    }
}
```

## Common Issues

### Issue 1: Sensor Not Detected
**Symptoms**: `begin()` returns false
**Solutions**:
1. Check power supply (3.3V vs 5V)
2. Swap TX/RX connections
3. Try different baud rate:
```cpp
finger.begin(0x00000000, 9600);
```

### Issue 2: Communication Errors
**Symptoms**: Random failures, timeout errors
**Solutions**:
1. Add 1K resistor between TX and RX
2. Use shorter cables
3. Add capacitors near sensor power pins

### Issue 3: Memory Issues
**Symptoms**: Crashes on database scan
**Solutions**:
1. Reduce database tracking size
2. Use `scanDatabase()` only when needed
3. Enable memory optimization in IDE

## Best Practices

### 1. Always Check Return Values
```cpp
UF_ErrorCode result = finger.enroll(id);
if (result != UF_OK) {
    Serial.println(finger.errorToString(result));
}
```

### 2. Use Auto-Detection
```cpp
UniversalFingerprint finger(&Serial2, UF_SENSOR_AUTO);
```

### 3. Regular Database Maintenance
```cpp
void maintainDatabase() {
    // Scan database monthly
    finger.scanDatabase();
    
    // Check for duplicates
    UF_DatabaseStats stats;
    finger.getDatabaseStats(stats);
    
    if (stats.usagePercentage > 90) {
        Serial.println("Warning: Database almost full!");
    }
}
```

### 4. Error Handling
```cpp
void handleError(UF_ErrorCode error) {
    switch (error) {
        case UF_ERROR_NO_SENSOR:
            // Try reinitializing
            finger.begin();
            break;
        case UF_ERROR_SLOT_FULL:
            // Find new empty slot
            finger.findEmptySlot();
            break;
        // Handle other errors
    }
}
```

## Advanced Configuration

### Custom Baud Rate
```cpp
// Set custom baud rate
finger.setBaudRate(115200);

// Reinitialize with new baud rate
finger.begin(0x00000000, 115200);
```

### LED Control
```cpp
// Turn on blue LED
finger.setLED(true, UF_LED_BLUE);

// Blink red LED
finger.setLED(true, UF_LED_RED, 5);

// Turn off LED
finger.setLED(false);
```

### Security Levels
```cpp
// Set security level (1-5)
// Higher = more secure but slower
finger.setSecurity(3);
```

## Performance Tips

### 1. Cache Database Results
```cpp
// Scan once and cache results
UF_DatabaseStats stats;
finger.getDatabaseStats(stats);
// Use cached stats instead of scanning each time
```

### 2. Batch Operations
```cpp
// Batch delete multiple fingerprints
for (uint16_t i = start; i <= end; i++) {
    finger.deleteTemplate(i);
}
```

### 3. Use Hardware Serial When Possible
```cpp
// Hardware serial is faster than software serial
UniversalFingerprint finger(&Serial2);
```

## Troubleshooting Checklist

- [ ] Sensor properly powered (3.3V/5V)
- [ ] TX/RX correctly connected
- [ ] Baud rate matches sensor
- [ ] No conflicting libraries
- [ ] Sufficient memory available
- [ ] Serial monitor at correct baud rate
- [ ] Sensor firmware up to date

## Next Steps

1. Try the included examples
2. Explore the API documentation
3. Join the community for support
4. Contribute to the project
```
