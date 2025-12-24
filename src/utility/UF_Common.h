/**
 * @file UF_Common.h
 * @brief Common definitions for UniversalFingerprint library
 */

#ifndef UF_COMMON_H
#define UF_COMMON_H

#include "Arduino.h"

// Library version
#define UF_VERSION_MAJOR 1
#define UF_VERSION_MINOR 0
#define UF_VERSION_PATCH 0
#define UF_VERSION_STRING "1.0.0"

/**
 * @brief Supported fingerprint sensor types
 */
enum UF_SensorType {
    UF_SENSOR_AS608    = 608,   ///< AS608 with 162 templates
    UF_SENSOR_R307     = 307,   ///< R307 with 1000 templates
    UF_SENSOR_GT511C3  = 511,   ///< GT-511C3 with 200 templates
    UF_SENSOR_ZFM60    = 60,    ///< ZFM-60 with 300 templates
    UF_SENSOR_ZFM20    = 20,    ///< ZFM-20 with 256 templates
    UF_SENSOR_AUTO     = 0      ///< Auto-detect sensor type
};

/**
 * @brief Library error codes
 */
enum UF_ErrorCode {
    UF_OK                  = 0x00,  ///< Operation successful
    UF_ERROR_COMM          = 0x01,  ///< Communication error
    UF_ERROR_NO_SENSOR     = 0x02,  ///< Sensor not found
    UF_ERROR_INVALID_ID    = 0x03,  ///< Invalid template ID
    UF_ERROR_SLOT_FULL     = 0x04,  ///< No empty slots available
    UF_ERROR_TIMEOUT       = 0x05,  ///< Operation timeout
    UF_ERROR_SENSOR_BUSY   = 0x06,  ///< Sensor is busy
    UF_ERROR_PACKET        = 0x07,  ///< Invalid packet received
    UF_ERROR_NOT_ENROLLED  = 0x08,  ///< Finger not enrolled
    UF_ERROR_NO_FINGER     = 0x09,  ///< No finger detected
    UF_ERROR_IMAGE_MESS    = 0x0A,  ///< Image too messy
    UF_ERROR_FEATURE_FAIL  = 0x0B,  ///< Feature extraction failed
    UF_ERROR_IMAGE_FAIL    = 0x0C,  ///< Image capture failed
    UF_ERROR_DUPLICATE_ID  = 0x0D,  ///< Slot already occupied
    UF_ERROR_NOT_SUPPORTED = 0x0E,  ///< Feature not supported
    UF_ERROR_INVALID_PARAM = 0x0F   ///< Invalid parameter
};

/**
 * @brief LED colors for sensor LED control
 */
enum UF_LEDColor {
    UF_LED_RED     = 1,  ///< Red LED
    UF_LED_BLUE    = 2,  ///< Blue LED
    UF_LED_PURPLE  = 3,  ///< Purple LED
    UF_LED_GREEN   = 4,  ///< Green LED
    UF_LED_CYAN    = 5,  ///< Cyan LED
    UF_LED_YELLOW  = 6,  ///< Yellow LED
    UF_LED_WHITE   = 7   ///< White LED
};

/**
 * @brief Sensor information structure
 */
struct UF_SensorInfo {
    UF_SensorType type;          ///< Sensor type
    const char* modelName;       ///< Sensor model name
    const char* vendor;          ///< Sensor vendor
    uint16_t capacity;           ///< Maximum template capacity
    uint16_t packetSize;         ///< Data packet size
    uint32_t defaultBaud;        ///< Default baud rate
    uint8_t address;             ///< I2C address (if applicable)
    bool hasLED;                 ///< Has RGB LED control
    bool hasTouchSensor;         ///< Has touch detection
};

/**
 * @brief Template information structure
 */
struct UF_TemplateInfo {
    uint16_t id;                 ///< Template ID (1-indexed)
    bool isOccupied;             ///< Slot occupancy status
    uint16_t confidence;         ///< Match confidence (if available)
    uint32_t enrollTime;         ///< Enrollment timestamp
};

/**
 * @brief Database statistics
 */
struct UF_DatabaseStats {
    uint16_t totalSlots;         ///< Total available slots
    uint16_t occupiedSlots;      ///< Currently occupied slots
    uint16_t freeSlots;          ///< Free slots
    uint16_t firstFreeSlot;      ///< First free slot ID
    uint16_t lastFreeSlot;       ///< Last free slot ID
    float usagePercentage;       ///< Database usage percentage
};

#endif // UF_COMMON_H