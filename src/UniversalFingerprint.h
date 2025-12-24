/**
 * @file UniversalFingerprint.h
 * @brief Universal fingerprint library for Arduino supporting multiple sensor types
 * @version 1.0.0
 * @date 2024
 * @license MIT
 * 
 * Supports: AS608 (162 templates), R307 (1000 templates), GT-511C3 (200 templates)
 * Features: Auto-detection, full memory utilization, database management
 */

#ifndef UNIVERSALFINGERPRINT_H
#define UNIVERSALFINGERPRINT_H

#include "Arduino.h"
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include "utility/UF_Common.h"

// Forward declarations
class UF_MemoryManager;
class UF_SensorDetector;

/**
 * @brief Main class for Universal Fingerprint Library
 */
class UniversalFingerprint {
public:
    // ==================== CONSTRUCTORS ====================
    
    /**
     * @brief Constructor for HardwareSerial
     * @param serial Pointer to HardwareSerial object
     * @param sensorType Sensor type (default: auto-detect)
     */
    UniversalFingerprint(HardwareSerial* serial, UF_SensorType sensorType = UF_SENSOR_AUTO);
    
    /**
     * @brief Constructor for SoftwareSerial
     * @param serial Pointer to SoftwareSerial object
     * @param sensorType Sensor type (default: auto-detect)
     */
    UniversalFingerprint(SoftwareSerial* serial, UF_SensorType sensorType = UF_SENSOR_AUTO);
    
    /**
     * @brief Destructor
     */
    ~UniversalFingerprint();
    
    // ==================== INITIALIZATION ====================
    
    /**
     * @brief Initialize the fingerprint sensor
     * @param password Sensor password (default: 0x00000000)
     * @param baudRate Communication baud rate (default: 57600)
     * @return true if successful, false otherwise
     */
    bool begin(uint32_t password = 0x00000000, uint32_t baudRate = 57600);
    
    /**
     * @brief Check if sensor is initialized
     * @return true if initialized
     */
    bool isInitialized() const { return _initialized; }
    
    // ==================== SENSOR INFORMATION ====================
    
    /**
     * @brief Get detected sensor type
     * @return UF_SensorType enum
     */
    UF_SensorType getSensorType() const { return _sensorType; }
    
    /**
     * @brief Get sensor model name
     * @return String with model name
     */
    String getSensorModel() const;
    
    /**
     * @brief Get sensor vendor
     * @return String with vendor name
     */
    String getSensorVendor() const;
    
    /**
     * @brief Get total template capacity
     * @return Maximum number of templates
     */
    uint16_t getCapacity() const { return _capacity; }
    
    /**
     * @brief Get library version
     * @return Version string
     */
    static String getVersion() { return UF_VERSION_STRING; }
    
    /**
     * @brief Get sensor information structure
     * @param info Reference to UF_SensorInfo to fill
     * @return true if successful
     */
    bool getSensorInfo(UF_SensorInfo& info);
    
    // ==================== DATABASE MANAGEMENT ====================
    
    /**
     * @brief Scan entire database and update internal state
     * @return Number of templates found, or negative for error
     */
    int16_t scanDatabase();
    
    /**
     * @brief Get number of enrolled templates
     * @return Template count
     */
    uint16_t getEnrolledCount() const { return _enrolledCount; }
    
    /**
     * @brief Check if a slot is occupied
     * @param id Template ID (1-indexed)
     * @return true if occupied
     */
    bool isSlotOccupied(uint16_t id);
    
    /**
     * @brief Find first empty slot
     * @param start Starting ID to search from (default: 1)
     * @return ID of empty slot, or 0 if none found
     */
    uint16_t findEmptySlot(uint16_t start = 1);
    
    /**
     * @brief Find last empty slot
     * @return ID of last empty slot, or 0 if none found
     */
    uint16_t findLastEmptySlot();
    
    /**
     * @brief Get database statistics
     * @param stats Reference to UF_DatabaseStats to fill
     * @return true if successful
     */
    bool getDatabaseStats(UF_DatabaseStats& stats);
    
    /**
     * @brief Print database information to Serial
     */
    void printDatabaseInfo();
    
    // ==================== FINGERPRINT OPERATIONS ====================
    
    /**
     * @brief Enroll a new fingerprint
     * @param id Template ID (0 for auto-select)
     * @param numScans Number of fingerprint scans (default: 2)
     * @return UF_ErrorCode
     */
    UF_ErrorCode enroll(uint16_t id = 0, uint8_t numScans = 2);
    
    /**
     * @brief Identify a fingerprint
     * @param confidence Pointer to store confidence score
     * @param timeout Timeout in seconds
     * @return Template ID if found, 0 if not found, negative for error
     */
    int16_t identify(uint16_t* confidence = nullptr, uint8_t timeout = 10);
    
    /**
     * @brief Verify fingerprint against specific ID
     * @param id Template ID to verify
     * @param confidence Pointer to store confidence score
     * @return UF_ErrorCode
     */
    UF_ErrorCode verify(uint16_t id, uint16_t* confidence = nullptr);
    
    /**
     * @brief Delete a fingerprint template
     * @param id Template ID to delete
     * @return UF_ErrorCode
     */
    UF_ErrorCode deleteTemplate(uint16_t id);
    
    /**
     * @brief Delete all templates
     * @return UF_ErrorCode
     */
    UF_ErrorCode clearDatabase();
    
    // ==================== SENSOR CONTROL ====================
    
    /**
     * @brief Control sensor LED
     * @param on true to turn on, false to turn off
     * @param color LED color (default: blue)
     * @param speed Blink speed (0 = solid, 1-255 = blink)
     * @return UF_ErrorCode
     */
    UF_ErrorCode setLED(bool on, UF_LEDColor color = UF_LED_BLUE, uint8_t speed = 0);
    
    /**
     * @brief Set sensor security level
     * @param level Security level (1-5)
     * @return UF_ErrorCode
     */
    UF_ErrorCode setSecurity(uint8_t level);
    
    /**
     * @brief Check if finger is present on sensor
     * @return true if finger detected
     */
    bool isFingerPresent();
    
    /**
     * @brief Change sensor baud rate
     * @param baudRate New baud rate
     * @return UF_ErrorCode
     */
    UF_ErrorCode setBaudRate(uint32_t baudRate);
    
    /**
     * @brief Put sensor to sleep (low power)
     * @return UF_ErrorCode
     */
    UF_ErrorCode sleep();
    
    /**
     * @brief Wake sensor from sleep
     * @return UF_ErrorCode
     */
    UF_ErrorCode wake();
    
    // ==================== ADVANCED OPERATIONS ====================
    
    /**
     * @brief Read raw template data
     * @param id Template ID
     * @param buffer Buffer to store data
     * @param bufferSize Size of buffer
     * @return Number of bytes read, or negative for error
     */
    int16_t readRawTemplate(uint16_t id, uint8_t* buffer, uint16_t bufferSize);
    
    /**
     * @brief Write raw template data
     * @param id Template ID
     * @param buffer Buffer containing template data
     * @param size Size of data
     * @return UF_ErrorCode
     */
    UF_ErrorCode writeRawTemplate(uint16_t id, uint8_t* buffer, uint16_t size);
    
    /**
     * @brief Get template information
     * @param id Template ID
     * @param info Reference to UF_TemplateInfo to fill
     * @return UF_ErrorCode
     */
    UF_ErrorCode getTemplateInfo(uint16_t id, UF_TemplateInfo& info);
    
    // ==================== UTILITY FUNCTIONS ====================
    
    /**
     * @brief Convert error code to string
     * @param error Error code
     * @return Error description
     */
    static String errorToString(UF_ErrorCode error);
    
    /**
     * @brief Convert sensor type to string
     * @param type Sensor type
     * @return Sensor name
     */
    static String sensorTypeToString(UF_SensorType type);
    
    /**
     * @brief Convert LED color to string
     * @param color LED color
     * @return Color name
     */
    static String ledColorToString(UF_LEDColor color);

private:
    // Private members
    Adafruit_Fingerprint* _finger;
    UF_SensorType _sensorType;
    uint16_t _capacity;
    uint16_t _enrolledCount;
    bool _initialized;
    bool _databaseScanned;
    
    // Database tracking
    bool* _slotOccupancy;
    
    // Communication
    HardwareSerial* _hwSerial;
    SoftwareSerial* _swSerial;
    bool _useSoftwareSerial;
    
    // Helper objects
    UF_MemoryManager* _memoryManager;
    UF_SensorDetector* _sensorDetector;
    
    // Private methods
    bool _initSensor(uint32_t password);
    bool _autoDetectSensor();
    UF_ErrorCode _convertAdafruitError(uint8_t error);
    bool _validateID(uint16_t id);
    void _updateSlotOccupancy(uint16_t id, bool occupied);
    bool _checkSensorResponse();
    
    // Internal enrollment helper
    UF_ErrorCode _enrollInternal(uint16_t id);
    
    // Platform-specific methods
    #ifdef ESP32
    void _setupESP32();
    #endif
    
    // Disable copy constructor and assignment
    UniversalFingerprint(const UniversalFingerprint&) = delete;
    UniversalFingerprint& operator=(const UniversalFingerprint&) = delete;
};

#endif // UNIVERSALFINGERPRINT_H
