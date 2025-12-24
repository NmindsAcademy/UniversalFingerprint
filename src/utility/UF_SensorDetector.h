/**
 * @file UF_SensorDetector.h
 * @brief Auto-detection of fingerprint sensor type
 */

#ifndef UF_SENSORDETECTOR_H
#define UF_SENSORDETECTOR_H

#include <Adafruit_Fingerprint.h>
#include "UF_Common.h"

class UF_SensorDetector {
public:
    UF_SensorDetector(Adafruit_Fingerprint* finger);
    
    UF_SensorType detect();
    UF_SensorType detectByCapacity();
    UF_SensorType detectByParameters();
    
    const char* getSensorName() const;
    uint16_t getDetectedCapacity() const { return _detectedCapacity; }
    
private:
    Adafruit_Fingerprint* _finger;
    UF_SensorType _detectedType;
    uint16_t _detectedCapacity;
    
    bool _testSlot(uint16_t id);
    uint16_t _findMaxCapacity();
};

#endif // UF_SENSORDETECTOR_H