# UniversalFingerprint API Reference

## Table of Contents

1. [Overview](#overview)
2. [Class Reference](#class-reference)
   - [UniversalFingerprint Class](#universalfingerprint-class)
3. [Enumerations](#enumerations)
   - [UF_SensorType](#uf_sensortype)
   - [UF_ErrorCode](#uf_errorcode)
   - [UF_LEDColor](#uf_ledcolor)
4. [Structures](#structures)
   - [UF_SensorInfo](#uf_sensorinfo)
   - [UF_TemplateInfo](#uf_templateinfo)
   - [UF_DatabaseStats](#uf_databasestats)
5. [Methods](#methods)
   - [Constructors](#constructors)
   - [Initialization](#initialization)
   - [Sensor Information](#sensor-information)
   - [Database Management](#database-management)
   - [Fingerprint Operations](#fingerprint-operations)
   - [Sensor Control](#sensor-control)
   - [Advanced Operations](#advanced-operations)
   - [Utility Functions](#utility-functions)
6. [Examples](#examples)
   - [Basic Usage](#basic-usage)
   - [Database Management](#database-management-example)
   - [Multiple Sensors](#multiple-sensors-example)
   - [Error Handling](#error-handling-example)
7. [Error Handling](#error-handling-guide)
8. [Best Practices](#best-practices)
9. [Compatibility Notes](#compatibility-notes)
10. [Troubleshooting](#troubleshooting)

## Overview

The UniversalFingerprint library provides a unified interface for working with multiple fingerprint sensor models. This API reference documents all classes, structures, enumerations, and methods available in the library.

## Class Reference

### UniversalFingerprint Class

The main class for fingerprint sensor operations.

```cpp
class UniversalFingerprint {
public:
    // Constructors
    UniversalFingerprint(HardwareSerial* serial, UF_SensorType sensorType = UF_SENSOR_AUTO);
    UniversalFingerprint(SoftwareSerial* serial, UF_SensorType sensorType = UF_SENSOR_AUTO);
    
    // Destructor
    ~UniversalFingerprint();
    
    // Initialization
    bool begin(uint32_t password = 0x00000000, uint32_t baudRate = 57600);
    bool isInitialized() const;
    
    // Sensor Information
    UF_SensorType getSensorType() const;
    String getSensorModel() const;
    String getSensorVendor() const;
    uint16_t getCapacity() const;
    static String getVersion();
    bool getSensorInfo(UF_SensorInfo& info);
    
    // Database Management
    int16_t scanDatabase();
    uint16_t getEnrolledCount() const;
    bool isSlotOccupied(uint16_t id);
    uint16_t findEmptySlot(uint16_t start = 1);
    uint16_t findLastEmptySlot();
    bool getDatabaseStats(UF_DatabaseStats& stats);
    void printDatabaseInfo();
    
    // Fingerprint Operations
    UF_ErrorCode enroll(uint16_t id = 0, uint8_t numScans = 2);
    int16_t identify(uint16_t* confidence = nullptr, uint8_t timeout = 10);
    UF_ErrorCode verify(uint16_t id, uint16_t* confidence = nullptr);
    UF_ErrorCode deleteTemplate(uint16_t id);
    UF_ErrorCode clearDatabase();
    
    // Sensor Control
    UF_ErrorCode setLED(bool on, UF_LEDColor color = UF_LED_BLUE, uint8_t speed = 0);
    UF_ErrorCode setSecurity(uint8_t level);
    bool isFingerPresent();
    UF_ErrorCode setBaudRate(uint32_t baudRate);
    UF_ErrorCode sleep();
    UF_ErrorCode wake();
    
    // Advanced Operations
    int16_t readRawTemplate(uint16_t id, uint8_t* buffer, uint16_t bufferSize);
    UF_ErrorCode writeRawTemplate(uint16_t id, uint8_t* buffer, uint16_t size);
    UF_ErrorCode getTemplateInfo(uint16_t id, UF_TemplateInfo& info);
    
    // Utility Functions
    static String errorToString(UF_ErrorCode error);
    static String sensorTypeToString(UF_SensorType type);
    static String ledColorToString(UF_LEDColor color);
};
```

## Enumerations

### UF_SensorType

```cpp
enum UF_SensorType {
    UF_SENSOR_AS608,      // AS608 with 162 templates
    UF_SENSOR_R307,       // R307 with 1000 templates
    UF_SENSOR_GT511C3,    // GT-511C3 with 200 templates
    UF_SENSOR_ZFM60,      // ZFM-60 with 300 templates
    UF_SENSOR_ZFM20,      // ZFM-20 with 256 templates
    UF_SENSOR_AUTO        // Auto-detect sensor type
};
```

### UF_ErrorCode

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

### UF_LEDColor

```cpp
enum UF_LEDColor {
    UF_LED_RED,     // Red LED
    UF_LED_BLUE,    // Blue LED
    UF_LED_PURPLE,  // Purple LED
    UF_LED_GREEN,   // Green LED
    UF_LED_CYAN,    // Cyan LED
    UF_LED_YELLOW,  // Yellow LED
    UF_LED_WHITE    // White LED
};
```

## Structures

### UF_SensorInfo

Contains sensor information.

```cpp
struct UF_SensorInfo {
    UF_SensorType type;          // Sensor type
    const char* modelName;       // Model name
    const char* vendor;          // Vendor name
    uint16_t capacity;           // Template capacity
    uint16_t packetSize;         // Data packet size
    uint32_t defaultBaud;        // Default baud rate
    uint8_t address;             // I2C address (if applicable)
    bool hasLED;                 // Has RGB LED control
    bool hasTouchSensor;         // Has touch detection
};
```

### UF_TemplateInfo

Contains template information.

```cpp
struct UF_TemplateInfo {
    uint16_t id;                 // Template ID (1-indexed)
    bool isOccupied;             // Slot occupancy status
    uint16_t confidence;         // Match confidence (if available)
    uint32_t enrollTime;         // Enrollment timestamp
};
```

### UF_DatabaseStats

Contains database statistics.

```cpp
struct UF_DatabaseStats {
    uint16_t totalSlots;         // Total available slots
    uint16_t occupiedSlots;      // Currently occupied slots
    uint16_t freeSlots;          // Free slots
    uint16_t firstFreeSlot;      // First free slot ID
    uint16_t lastFreeSlot;       // Last free slot ID
    float usagePercentage;       // Database usage percentage
};
```

## Methods

### Constructors

#### `UniversalFingerprint(HardwareSerial* serial, UF_SensorType sensorType = UF_SENSOR_AUTO)`

Creates a UniversalFingerprint object using HardwareSerial.

**Parameters:**
- `serial`: Pointer to HardwareSerial object
- `sensorType`: Sensor type (default: auto-detect)

**Example:**
```cpp
// Hardware Serial (ESP32, Arduino Mega)
UniversalFingerprint finger(&Serial2);

// With specific sensor type
UniversalFingerprint finger(&Serial2, UF_SENSOR_R307);
```

#### `UniversalFingerprint(SoftwareSerial* serial, UF_SensorType sensorType = UF_SENSOR_AUTO)`

Creates a UniversalFingerprint object using SoftwareSerial.

**Parameters:**
- `serial`: Pointer to SoftwareSerial object
- `sensorType`: Sensor type (default: auto-detect)

**Example:**
```cpp
#include <SoftwareSerial.h>
SoftwareSerial fingerSerial(2, 3);
UniversalFingerprint finger(&fingerSerial);
```

### Initialization

#### `bool begin(uint32_t password = 0x00000000, uint32_t baudRate = 57600)`

Initializes the fingerprint sensor.

**Parameters:**
- `password`: Sensor password (default: 0x00000000)
- `baudRate`: Communication baud rate (default: 57600)

**Returns:** `true` if successful, `false` otherwise

**Example:**
```cpp
if (!finger.begin()) {
    Serial.println("Sensor initialization failed!");
    while(1);
}
```

#### `bool isInitialized() const`

Checks if sensor is initialized.

**Returns:** `true` if initialized

**Example:**
```cpp
if (finger.isInitialized()) {
    Serial.println("Sensor ready");
}
```

### Sensor Information

#### `UF_SensorType getSensorType() const`

Gets detected sensor type.

**Returns:** Sensor type enumeration

**Example:**
```cpp
UF_SensorType type = finger.getSensorType();
Serial.print("Sensor type: ");
Serial.println(finger.sensorTypeToString(type));
```

#### `String getSensorModel() const`

Gets sensor model name.

**Returns:** Sensor model as String

**Example:**
```cpp
String model = finger.getSensorModel();
Serial.print("Sensor model: ");
Serial.println(model);
```

#### `String getSensorVendor() const`

Gets sensor vendor.

**Returns:** Sensor vendor as String

**Example:**
```cpp
String vendor = finger.getSensorVendor();
Serial.print("Sensor vendor: ");
Serial.println(vendor);
```

#### `uint16_t getCapacity() const`

Gets total template capacity.

**Returns:** Maximum number of templates

**Example:**
```cpp
uint16_t capacity = finger.getCapacity();
Serial.print("Capacity: ");
Serial.print(capacity);
Serial.println(" templates");
```

#### `static String getVersion()`

Gets library version.

**Returns:** Library version string

**Example:**
```cpp
String version = UniversalFingerprint::getVersion();
Serial.print("Library version: ");
Serial.println(version);
```

#### `bool getSensorInfo(UF_SensorInfo& info)`

Gets sensor information structure.

**Parameters:**
- `info`: Reference to UF_SensorInfo to fill

**Returns:** `true` if successful

**Example:**
```cpp
UF_SensorInfo info;
if (finger.getSensorInfo(info)) {
    Serial.print("Model: ");
    Serial.println(info.modelName);
    Serial.print("Capacity: ");
    Serial.println(info.capacity);
}
```

### Database Management

#### `int16_t scanDatabase()`

Scans entire database and updates internal state.

**Returns:** Number of templates found, or negative error code

**Example:**
```cpp
int16_t count = finger.scanDatabase();
if (count >= 0) {
    Serial.print("Found ");
    Serial.print(count);
    Serial.println(" templates");
} else {
    Serial.print("Scan error: ");
    Serial.println(finger.errorToString((UF_ErrorCode)(-count)));
}
```

#### `uint16_t getEnrolledCount() const`

Gets number of enrolled templates.

**Returns:** Template count

**Example:**
```cpp
uint16_t enrolled = finger.getEnrolledCount();
Serial.print("Enrolled templates: ");
Serial.println(enrolled);
```

#### `bool isSlotOccupied(uint16_t id)`

Checks if a slot is occupied.

**Parameters:**
- `id`: Template ID (1-indexed)

**Returns:** `true` if occupied

**Example:**
```cpp
bool occupied = finger.isSlotOccupied(5);
if (occupied) {
    Serial.println("Slot 5 is occupied");
} else {
    Serial.println("Slot 5 is empty");
}
```

#### `uint16_t findEmptySlot(uint16_t start = 1)`

Finds first empty slot.

**Parameters:**
- `start`: Starting ID to search from (default: 1)

**Returns:** ID of empty slot, or 0 if none found

**Example:**
```cpp
uint16_t emptySlot = finger.findEmptySlot();
if (emptySlot > 0) {
    Serial.print("First empty slot: #");
    Serial.println(emptySlot);
} else {
    Serial.println("No empty slots available");
}
```

#### `uint16_t findLastEmptySlot()`

Finds last empty slot.

**Returns:** ID of last empty slot, or 0 if none found

**Example:**
```cpp
uint16_t lastEmpty = finger.findLastEmptySlot();
if (lastEmpty > 0) {
    Serial.print("Last empty slot: #");
    Serial.println(lastEmpty);
}
```

#### `bool getDatabaseStats(UF_DatabaseStats& stats)`

Gets database statistics.

**Parameters:**
- `stats`: Reference to UF_DatabaseStats to fill

**Returns:** `true` if successful

**Example:**
```cpp
UF_DatabaseStats stats;
if (finger.getDatabaseStats(stats)) {
    Serial.print("Total slots: ");
    Serial.println(stats.totalSlots);
    Serial.print("Occupied: ");
    Serial.println(stats.occupiedSlots);
    Serial.print("Usage: ");
    Serial.print(stats.usagePercentage, 1);
    Serial.println("%");
}
```

#### `void printDatabaseInfo()`

Prints database information to Serial.

**Example:**
```cpp
finger.printDatabaseInfo();
```

### Fingerprint Operations

#### `UF_ErrorCode enroll(uint16_t id = 0, uint8_t numScans = 2)`

Enrolls a new fingerprint.

**Parameters:**
- `id`: Template ID (0 for auto-select)
- `numScans`: Number of fingerprint scans (default: 2)

**Returns:** Error code

**Example:**
```cpp
// Auto-select ID
UF_ErrorCode result = finger.enroll();
if (result == UF_OK) {
    Serial.println("Enrollment successful");
} else {
    Serial.print("Enrollment failed: ");
    Serial.println(finger.errorToString(result));
}

// Specific ID with 3 scans
result = finger.enroll(5, 3);
```

#### `int16_t identify(uint16_t* confidence = nullptr, uint8_t timeout = 10)`

Identifies a fingerprint.

**Parameters:**
- `confidence`: Pointer to store confidence score (optional)
- `timeout`: Timeout in seconds (default: 10)

**Returns:**
- `> 0`: Template ID found
- `0`: Fingerprint not in database
- `< 0`: Negative error code

**Example:**
```cpp
uint16_t confidence;
int16_t id = finger.identify(&confidence);

if (id > 0) {
    Serial.print("Found ID #");
    Serial.print(id);
    Serial.print(" (Confidence: ");
    Serial.print(confidence);
    Serial.println(")");
} else if (id == 0) {
    Serial.println("Fingerprint not in database");
} else {
    Serial.print("Identification error: ");
    Serial.println(finger.errorToString((UF_ErrorCode)(-id)));
}
```

#### `UF_ErrorCode verify(uint16_t id, uint16_t* confidence = nullptr)`

Verifies fingerprint against specific ID.

**Parameters:**
- `id`: Template ID to verify
- `confidence`: Pointer to store confidence score (optional)

**Returns:** Error code

**Example:**
```cpp
uint16_t confidence;
UF_ErrorCode result = finger.verify(5, &confidence);

if (result == UF_OK) {
    Serial.print("Verified! Confidence: ");
    Serial.println(confidence);
} else {
    Serial.print("Verification failed: ");
    Serial.println(finger.errorToString(result));
}
```

#### `UF_ErrorCode deleteTemplate(uint16_t id)`

Deletes a fingerprint template.

**Parameters:**
- `id`: Template ID to delete

**Returns:** Error code

**Example:**
```cpp
UF_ErrorCode result = finger.deleteTemplate(5);
if (result == UF_OK) {
    Serial.println("Template deleted successfully");
} else {
    Serial.print("Deletion failed: ");
    Serial.println(finger.errorToString(result));
}
```

#### `UF_ErrorCode clearDatabase()`

Deletes all templates.

**Returns:** Error code

**Example:**
```cpp
Serial.println("This will delete ALL fingerprints!");
Serial.print("Continue? (y/n): ");
// Wait for user confirmation

UF_ErrorCode result = finger.clearDatabase();
if (result == UF_OK) {
    Serial.println("All fingerprints deleted");
}
```

### Sensor Control

#### `UF_ErrorCode setLED(bool on, UF_LEDColor color = UF_LED_BLUE, uint8_t speed = 0)`

Controls sensor LED.

**Parameters:**
- `on`: `true` to turn on, `false` to turn off
- `color`: LED color (default: blue)
- `speed`: Blink speed (0 = solid, 1-255 = blink)

**Returns:** Error code

**Example:**
```cpp
// Turn on red LED
finger.setLED(true, UF_LED_RED);

// Blink blue LED
finger.setLED(true, UF_LED_BLUE, 5);

// Turn off LED
finger.setLED(false);
```

#### `UF_ErrorCode setSecurity(uint8_t level)`

Sets sensor security level.

**Parameters:**
- `level`: Security level (1-5, higher = more secure but slower)

**Returns:** Error code

**Example:**
```cpp
// Set high security
finger.setSecurity(5);

// Set medium security (recommended)
finger.setSecurity(3);

// Set low security for faster response
finger.setSecurity(1);
```

#### `bool isFingerPresent()`

Checks if finger is present on sensor.

**Returns:** `true` if finger detected

**Example:**
```cpp
if (finger.isFingerPresent()) {
    Serial.println("Finger detected");
} else {
    Serial.println("No finger detected");
}
```

#### `UF_ErrorCode setBaudRate(uint32_t baudRate)`

Changes sensor baud rate.

**Parameters:**
- `baudRate`: New baud rate

**Returns:** Error code

**Example:**
```cpp
// Change to 115200 baud
UF_ErrorCode result = finger.setBaudRate(115200);
if (result == UF_OK) {
    // Reinitialize with new baud rate
    finger.begin(0x00000000, 115200);
    Serial.println("Baud rate changed to 115200");
}
```

#### `UF_ErrorCode sleep()`

Puts sensor to sleep (low power).

**Returns:** Error code

**Example:**
```cpp
finger.sleep();
Serial.println("Sensor sleeping");
delay(5000);
finger.wake();
Serial.println("Sensor awake");
```

#### `UF_ErrorCode wake()`

Wakes sensor from sleep.

**Returns:** Error code

**Example:**
```cpp
finger.wake();
delay(100); // Give sensor time to wake
```

### Advanced Operations

#### `int16_t readRawTemplate(uint16_t id, uint8_t* buffer, uint16_t bufferSize)`

Reads raw template data.

**Parameters:**
- `id`: Template ID
- `buffer`: Buffer to store data
- `bufferSize`: Size of buffer

**Returns:** Number of bytes read, or negative error code

**Example:**
```cpp
uint8_t buffer[512];
int16_t bytesRead = finger.readRawTemplate(1, buffer, 512);
if (bytesRead > 0) {
    Serial.print("Read ");
    Serial.print(bytesRead);
    Serial.println(" bytes");
}
```

#### `UF_ErrorCode writeRawTemplate(uint16_t id, uint8_t* buffer, uint16_t size)`

Writes raw template data.

**Parameters:**
- `id`: Template ID
- `buffer`: Buffer containing template data
- `size`: Size of data

**Returns:** Error code

**Example:**
```cpp
uint8_t templateData[512] = { /* template data */ };
UF_ErrorCode result = finger.writeRawTemplate(10, templateData, 512);
if (result == UF_OK) {
    Serial.println("Template written successfully");
}
```

#### `UF_ErrorCode getTemplateInfo(uint16_t id, UF_TemplateInfo& info)`

Gets template information.

**Parameters:**
- `id`: Template ID
- `info`: Reference to UF_TemplateInfo to fill

**Returns:** Error code

**Example:**
```cpp
UF_TemplateInfo info;
UF_ErrorCode result = finger.getTemplateInfo(5, info);
if (result == UF_OK) {
    Serial.print("Template ID: ");
    Serial.println(info.id);
    Serial.print("Occupied: ");
    Serial.println(info.isOccupied ? "Yes" : "No");
}
```

### Utility Functions

#### `static String errorToString(UF_ErrorCode error)`

Converts error code to string.

**Parameters:**
- `error`: Error code

**Returns:** Error description

**Example:**
```cpp
UF_ErrorCode result = finger.enroll(5);
if (result != UF_OK) {
    Serial.print("Error: ");
    Serial.println(UniversalFingerprint::errorToString(result));
}
```

#### `static String sensorTypeToString(UF_SensorType type)`

Converts sensor type to string.

**Parameters:**
- `type`: Sensor type

**Returns:** Sensor name

**Example:**
```cpp
UF_SensorType type = finger.getSensorType();
String typeName = UniversalFingerprint::sensorTypeToString(type);
Serial.print("Sensor: ");
Serial.println(typeName);
```

#### `static String ledColorToString(UF_LEDColor color)`

Converts LED color to string.

**Parameters:**
- `color`: LED color

**Returns:** Color name

**Example:**
```cpp
String colorName = UniversalFingerprint::ledColorToString(UF_LED_BLUE);
Serial.print("LED color: ");
Serial.println(colorName);
```

## Examples

### Basic Usage

```cpp
#include <UniversalFingerprint.h>
#include <SoftwareSerial.h>

SoftwareSerial fingerSerial(2, 3);
UniversalFingerprint finger(&fingerSerial);

void setup() {
    Serial.begin(115200);
    
    // Initialize sensor
    if (!finger.begin()) {
        Serial.println("Sensor initialization failed!");
        while(1);
    }
    
    Serial.print("Sensor: ");
    Serial.println(finger.getSensorModel());
    Serial.print("Capacity: ");
    Serial.println(finger.getCapacity());
}

void loop() {
    // Identify fingerprint
    int16_t id = finger.identify();
    
    if (id > 0) {
        Serial.print("Welcome, user #");
        Serial.println(id);
    } else if (id == 0) {
        Serial.println("Access denied");
    } else {
        Serial.print("Error: ");
        Serial.println(finger.errorToString((UF_ErrorCode)(-id)));
    }
    
    delay(1000);
}
```

### Database Management Example

```cpp
void manageDatabase() {
    // Get database statistics
    UF_DatabaseStats stats;
    if (finger.getDatabaseStats(stats)) {
        Serial.println("\n=== DATABASE STATISTICS ===");
        Serial.print("Total slots: ");
        Serial.println(stats.totalSlots);
        Serial.print("Occupied: ");
        Serial.println(stats.occupiedSlots);
        Serial.print("Free: ");
        Serial.println(stats.freeSlots);
        Serial.print("Usage: ");
        Serial.print(stats.usagePercentage, 1);
        Serial.println("%");
        
        if (stats.firstFreeSlot > 0) {
            Serial.print("First free slot: #");
            Serial.println(stats.firstFreeSlot);
        }
        
        if (stats.lastFreeSlot > 0) {
            Serial.print("Last free slot: #");
            Serial.println(stats.lastFreeSlot);
        }
    }
    
    // Scan for empty slots
    Serial.println("\nScanning for empty slots...");
    uint16_t emptySlots[10];
    uint16_t found = 0;
    
    for (uint16_t i = 1; i <= finger.getCapacity() && found < 10; i++) {
        if (!finger.isSlotOccupied(i)) {
            emptySlots[found] = i;
            found++;
        }
    }
    
    if (found > 0) {
        Serial.print("First 10 empty slots: ");
        for (uint16_t i = 0; i < found; i++) {
            Serial.print(emptySlots[i]);
            if (i < found - 1) Serial.print(", ");
        }
        Serial.println();
    }
}
```

### Multiple Sensors Example

```cpp
#include <UniversalFingerprint.h>
#include <SoftwareSerial.h>

// Multiple sensors
SoftwareSerial sensor1Serial(2, 3);
SoftwareSerial sensor2Serial(4, 5);

UniversalFingerprint sensor1(&sensor1Serial, UF_SENSOR_AS608);
UniversalFingerprint sensor2(&sensor2Serial, UF_SENSOR_R307);

void setup() {
    Serial.begin(115200);
    
    // Initialize both sensors
    sensor1.begin();
    sensor2.begin();
    
    Serial.println("=== MULTI-SENSOR SYSTEM ===");
    Serial.print("Sensor 1: ");
    Serial.print(sensor1.getSensorModel());
    Serial.print(" (");
    Serial.print(sensor1.getCapacity());
    Serial.println(" templates)");
    
    Serial.print("Sensor 2: ");
    Serial.print(sensor2.getSensorModel());
    Serial.print(" (");
    Serial.print(sensor2.getCapacity());
    Serial.println(" templates)");
}

void loop() {
    // Check both sensors for fingerprints
    int16_t id1 = sensor1.identify();
    int16_t id2 = sensor2.identify();
    
    if (id1 > 0) {
        Serial.print("Sensor 1: User #");
        Serial.println(id1);
    }
    
    if (id2 > 0) {
        Serial.print("Sensor 2: User #");
        Serial.println(id2);
    }
    
    delay(500);
}
```

### Error Handling Example

```cpp
void enrollWithErrorHandling() {
    Serial.println("\n=== ENROLLMENT WITH ERROR HANDLING ===");
    
    // Find empty slot
    uint16_t emptySlot = finger.findEmptySlot();
    if (emptySlot == 0) {
        Serial.println("ERROR: No empty slots available!");
        return;
    }
    
    Serial.print("Using slot #");
    Serial.println(emptySlot);
    Serial.println("Place finger on sensor...");
    
    // Attempt enrollment
    UF_ErrorCode result = finger.enroll(emptySlot);
    
    // Handle result
    switch (result) {
        case UF_OK:
            Serial.println("SUCCESS: Fingerprint enrolled!");
            break;
            
        case UF_ERROR_NO_FINGER:
            Serial.println("ERROR: No finger detected");
            Serial.println("Please place finger on sensor");
            break;
            
        case UF_ERROR_IMAGE_MESS:
            Serial.println("ERROR: Fingerprint image too messy");
            Serial.println("Please clean the sensor and try again");
            break;
            
        case UF_ERROR_TIMEOUT:
            Serial.println("ERROR: Enrollment timeout");
            Serial.println("Please try again");
            break;
            
        case UF_ERROR_SLOT_FULL:
            Serial.println("ERROR: Slot already occupied");
            Serial.print("Try slot #");
            Serial.println(finger.findEmptySlot());
            break;
            
        default:
            Serial.print("ERROR: ");
            Serial.println(finger.errorToString(result));
            break;
    }
}
```

## Error Handling Guide

### Always Check Return Values

```cpp
// ❌ Bad practice - ignoring errors
finger.enroll(5);

// ✅ Good practice - checking errors
UF_ErrorCode result = finger.enroll(5);
if (result != UF_OK) {
    Serial.print("Error: ");
    Serial.println(finger.errorToString(result));
    // Handle error appropriately
}
```

### Common Error Scenarios

#### Communication Errors
```cpp
UF_ErrorCode result = finger.begin();
if (result != UF_OK) {
    Serial.println("Check wiring, power, and baud rate");
    // Possible solutions:
    // 1. Check TX/RX connections
    // 2. Verify power supply (3.3V vs 5V)
    // 3. Try different baud rate
    // 4. Check for loose connections
}
```

#### Database Full
```cpp
UF_ErrorCode result = finger.enroll();
if (result == UF_ERROR_SLOT_FULL) {
    Serial.println("Database is full!");
    // Solutions:
    // 1. Delete unused templates
    // 2. Use larger capacity sensor
    // 3. Implement template rotation
}
```

#### Finger Not Detected
```cpp
if (!finger.isFingerPresent()) {
    Serial.println("Please place finger on sensor");
    // Wait for finger with timeout
    unsigned long startTime = millis();
    while (!finger.isFingerPresent() && millis() - startTime < 10000) {
        delay(100);
    }
}
```

### Recovery Strategies

#### Temporary Communication Failure
```cpp
int16_t id = finger.identify();
if (id < 0 && id == -UF_ERROR_COMM) {
    Serial.println("Communication error, attempting recovery...");
    
    // Try to reinitialize
    if (finger.begin()) {
        Serial.println("Recovery successful");
    } else {
        Serial.println("Recovery failed, check hardware");
    }
}
```

#### Database Corruption
```cpp
int16_t count = finger.scanDatabase();
if (count < 0) {
    Serial.println("Database may be corrupted");
    
    // Option 1: Clear and rebuild
    finger.clearDatabase();
    
    // Option 2: Backup before clearing
    // (Implement backup mechanism)
}
```

## Best Practices

### 1. Always Initialize Properly

```cpp
// ✅ Good practice
if (!finger.begin()) {
    Serial.println("Sensor initialization failed!");
    // Handle failure appropriately
    while(1); // Or implement recovery
}

// Check initialization status
if (finger.isInitialized()) {
    // Safe to use sensor
}
```

### 2. Use Auto-Detection for Flexibility

```cpp
// Hardware serial with auto-detection
UniversalFingerprint finger(&Serial2, UF_SENSOR_AUTO);

// Software serial with auto-detection
SoftwareSerial fingerSerial(2, 3);
UniversalFingerprint finger(&fingerSerial);
```

### 3. Regular Database Maintenance

```cpp
// Periodic database scan
void maintainDatabase() {
    static unsigned long lastScan = 0;
    const unsigned long SCAN_INTERVAL = 3600000; // 1 hour
    
    if (millis() - lastScan > SCAN_INTERVAL) {
        finger.scanDatabase();
        lastScan = millis();
        
        // Check database health
        UF_DatabaseStats stats;
        if (finger.getDatabaseStats(stats)) {
            if (stats.usagePercentage > 90) {
                Serial.println("WARNING: Database nearly full!");
            }
        }
    }
}
```

### 4. Handle Memory Constraints

```cpp
// Adjust for memory-constrained devices
#if defined(__AVR_ATmega328P__) // Arduino Uno
    const uint16_t BUFFER_SIZE = 256;
    const uint16_t MAX_SCAN_SLOTS = 100; // Scan in batches
#elif defined(ESP32)
    const uint16_t BUFFER_SIZE = 1024;
    const uint16_t MAX_SCAN_SLOTS = 1000; // Full scan
#else
    const uint16_t BUFFER_SIZE = 512;
    const uint16_t MAX_SCAN_SLOTS = 500;
#endif
```

### 5. Implement Timeouts

```cpp
// Custom timeout for operations
int16_t identifyWithTimeout(uint8_t timeoutSeconds) {
    return finger.identify(nullptr, timeoutSeconds);
}

// Enrollment with extended timeout
UF_ErrorCode enrollWithTimeout(uint16_t id, uint8_t timeoutSeconds) {
    // Implementation depends on sensor capabilities
    return finger.enroll(id);
}
```

### 6. Security Level Selection

```cpp
void setAppropriateSecurity() {
    // High security for authentication
    finger.setSecurity(5); // Slow but accurate
    
    // Medium security for general use
    finger.setSecurity(3); // Balanced
    
    // Low security for fast response
    finger.setSecurity(1); // Fast but less accurate
}
```

### 7. Backup and Restore

```cpp
// Template backup system
void backupDatabase() {
    // Implement backup to EEPROM, SD card, or cloud
    // Store template IDs and metadata
    // Consider encryption for sensitive applications
}

void restoreDatabase() {
    // Restore from backup
    // Verify integrity after restoration
}
```

## Compatibility Notes

### Sensor-Specific Features

#### AS608/R307 (Recommended)
- Full feature support
- RGB LED control
- Database management
- Good documentation

#### GT-511C3
- Limited LED control
- Different protocol
- May require specific initialization

#### ZFM Series
- Similar to AS608/R307
- Good feature set
- Reliable performance

### Platform Compatibility

#### Arduino Boards
- **Full Support**: Uno, Mega, Nano, Due
- **Limited Support**: Pro Mini, Micro (memory constraints)
- **Recommended**: Use HardwareSerial on Mega/Due

#### ESP32/ESP8266
- Excellent support
- Multiple HardwareSerial ports
- More memory for database tracking
- WiFi capabilities for remote access

#### Other Platforms
- Raspberry Pi Pico: Works with Arduino Core
- Teensy: Full support
- STM32: Should work with proper serial configuration

### Baud Rate Compatibility

| Sensor | Recommended Baud | Alternative Baud |
|--------|-----------------|------------------|
| AS608  | 57600           | 9600, 115200     |
| R307   | 57600           | 9600, 115200     |
| GT-511C3 | 9600           | 115200           |
| ZFM-60 | 57600           | 115200           |
| ZFM-20 | 57600           | 115200           |

## Troubleshooting

### Common Issues and Solutions

#### 1. Sensor Not Detected
**Problem**: `begin()` returns false
**Solutions**:
1. Check power supply voltage (3.3V vs 5V)
2. Verify TX/RX connections (swap if needed)
3. Try different baud rates
4. Check for faulty wiring or connections

#### 2. Communication Errors
**Problem**: Random packet errors or timeouts
**Solutions**:
1. Add 1K-10K resistor between TX and RX
2. Use level shifter for 3.3V/5V conversion
3. Shorten cable length (< 30cm)
4. Add capacitors near sensor power pins

#### 3. Memory Issues
**Problem**: Crashes or weird behavior
**Solutions**:
1. Reduce buffer sizes
2. Scan database in batches
3. Enable memory optimization in IDE
4. Use smaller sensor (AS608 instead of R307)

#### 4. Performance Problems
**Problem**: Slow response times
**Solutions**:
1. Use HardwareSerial instead of SoftwareSerial
2. Lower security level
3. Cache database statistics
4. Optimize code (remove unnecessary delays)

### Debugging Tips

#### Enable Debug Output
```cpp
#define UF_DEBUG 1
#include <UniversalFingerprint.h>

// Debug messages will be printed to Serial
```

#### Test Individual Components
```cpp
void testSensorComponents() {
    // Test communication
    Serial.print("Sensor responding: ");
    Serial.println(finger.isFingerPresent() ? "Yes" : "No");
    
    // Test LED
    Serial.print("LED control: ");
    if (finger.setLED(true, UF_LED_RED) == UF_OK) {
        Serial.println("OK");
        delay(500);
        finger.setLED(false, UF_LED_RED);
    } else {
        Serial.println("Failed");
    }
    
    // Test database
    Serial.print("Database scan: ");
    int16_t count = finger.scanDatabase();
    if (count >= 0) {
        Serial.print("OK (");
        Serial.print(count);
        Serial.println(" templates)");
    } else {
        Serial.println("Failed");
    }
}
```

### Getting Help

1. **Check Examples**: Try the included examples first
2. **Review Documentation**: Read this API reference
3. **Search Issues**: Check GitHub for similar problems
4. **Community Support**: Arduino Forum, Stack Overflow
5. **Create Issue**: Provide detailed information: