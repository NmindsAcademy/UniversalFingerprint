/**
 * @file UF_SensorDetector.cpp
 * @brief Implementation of sensor detection
 */

#include "UF_SensorDetector.h"

UF_SensorDetector::UF_SensorDetector(Adafruit_Fingerprint* finger)
    : _finger(finger), _detectedType(UF_SENSOR_AUTO), _detectedCapacity(0) {
}

UF_SensorType UF_SensorDetector::detect() {
    // Try multiple detection methods
    _detectedType = detectByParameters();
    
    if (_detectedType == UF_SENSOR_AUTO) {
        _detectedType = detectByCapacity();
    }
    
    // Set capacity based on detected type
    switch (_detectedType) {
        case UF_SENSOR_AS608:
            _detectedCapacity = 162;
            break;
        case UF_SENSOR_R307:
            _detectedCapacity = 1000;
            break;
        case UF_SENSOR_GT511C3:
            _detectedCapacity = 200;
            break;
        case UF_SENSOR_ZFM60:
            _detectedCapacity = 300;
            break;
        case UF_SENSOR_ZFM20:
            _detectedCapacity = 256;
            break;
        default:
            _detectedCapacity = 162; // Default to AS608
            break;
    }
    
    return _detectedType;
}

UF_SensorType UF_SensorDetector::detectByCapacity() {
    // Test known capacity boundaries
    if (_testSlot(1000)) {
        // Can access slot 1000, probably R307
        return UF_SENSOR_R307;
    } else if (_testSlot(300)) {
        // Can access slot 300, probably ZFM-60
        return UF_SENSOR_ZFM60;
    } else if (_testSlot(256)) {
        // Can access slot 256, probably ZFM-20
        return UF_SENSOR_ZFM20;
    } else if (_testSlot(200)) {
        // Can access slot 200, probably GT-511C3
        return UF_SENSOR_GT511C3;
    } else if (_testSlot(162)) {
        // Can access slot 162, probably AS608
        return UF_SENSOR_AS608;
    }
    
    return UF_SENSOR_AUTO;
}

UF_SensorType UF_SensorDetector::detectByParameters() {
    // Try to read sensor parameters
    uint32_t param = _finger->getParameters();
    
    if (param == 0) {
        return UF_SENSOR_AUTO;
    }
    
    // Different sensors may have different parameter responses
    // This would require sensor-specific knowledge
    
    return UF_SENSOR_AUTO;
}

const char* UF_SensorDetector::getSensorName() const {
    switch (_detectedType) {
        case UF_SENSOR_AS608: return "AS608";
        case UF_SENSOR_R307: return "R307";
        case UF_SENSOR_GT511C3: return "GT-511C3";
        case UF_SENSOR_ZFM60: return "ZFM-60";
        case UF_SENSOR_ZFM20: return "ZFM-20";
        default: return "Unknown";
    }
}

bool UF_SensorDetector::_testSlot(uint16_t id) {
    // Try to read template count at specific slot
    uint8_t result = _finger->getTemplateCount(id);
    return (result == FINGERPRINT_OK);
}

uint16_t UF_SensorDetector::_findMaxCapacity() {
    // Binary search for max capacity
    uint16_t low = 1;
    uint16_t high = 1000;
    uint16_t maxFound = 0;
    
    while (low <= high) {
        uint16_t mid = (low + high) / 2;
        
        if (_testSlot(mid)) {
            maxFound = mid;
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    
    return maxFound;
}