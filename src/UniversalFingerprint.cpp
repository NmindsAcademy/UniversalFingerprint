/**
 * @file UniversalFingerprint.cpp
 * @brief Implementation of UniversalFingerprint class
 */

#include "UniversalFingerprint.h"
#include "utility/UF_MemoryManager.h"
#include "utility/UF_SensorDetector.h"
#include <Adafruit_Fingerprint.h>

// Sensor capabilities database
static const UF_SensorInfo SENSOR_DB[] = {
    // AS608
    {UF_SENSOR_AS608, "AS608", "Adafruit", 162, 128, 57600, 1, true, true},
    // R307
    {UF_SENSOR_R307, "R307", "ZHONGSEN", 1000, 256, 57600, 1, true, true},
    // GT-511C3
    {UF_SENSOR_GT511C3, "GT-511C3", "Grow", 200, 512, 9600, 1, false, false},
    // ZFM-60
    {UF_SENSOR_ZFM60, "ZFM-60", "ZHONGSEN", 300, 128, 57600, 1, true, true},
    // ZFM-20
    {UF_SENSOR_ZFM20, "ZFM-20", "ZHONGSEN", 256, 128, 57600, 1, false, true},
    // AUTO (terminator)
    {UF_SENSOR_AUTO, "AUTO", "Unknown", 0, 0, 0, 0, false, false}
};

// ==================== CONSTRUCTORS ====================

UniversalFingerprint::UniversalFingerprint(HardwareSerial* serial, UF_SensorType sensorType) 
    : _hwSerial(serial), _swSerial(nullptr), _useSoftwareSerial(false),
      _sensorType(sensorType), _capacity(0), _enrolledCount(0),
      _initialized(false), _databaseScanned(false),
      _slotOccupancy(nullptr), _memoryManager(nullptr), _sensorDetector(nullptr) {
    
    _finger = new Adafruit_Fingerprint(_hwSerial);
    
    if (_sensorType != UF_SENSOR_AUTO) {
        for (const auto& info : SENSOR_DB) {
            if (info.type == _sensorType) {
                _capacity = info.capacity;
                break;
            }
        }
    }
}

UniversalFingerprint::UniversalFingerprint(SoftwareSerial* serial, UF_SensorType sensorType)
    : _swSerial(serial), _hwSerial(nullptr), _useSoftwareSerial(true),
      _sensorType(sensorType), _capacity(0), _enrolledCount(0),
      _initialized(false), _databaseScanned(false),
      _slotOccupancy(nullptr), _memoryManager(nullptr), _sensorDetector(nullptr) {
    
    _finger = new Adafruit_Fingerprint(_swSerial);
    
    if (_sensorType != UF_SENSOR_AUTO) {
        for (const auto& info : SENSOR_DB) {
            if (info.type == _sensorType) {
                _capacity = info.capacity;
                break;
            }
        }
    }
}

UniversalFingerprint::~UniversalFingerprint() {
    delete _finger;
    delete _memoryManager;
    delete _sensorDetector;
    
    if (_slotOccupancy != nullptr) {
        delete[] _slotOccupancy;
    }
}

// ==================== INITIALIZATION ====================

bool UniversalFingerprint::begin(uint32_t password, uint32_t baudRate) {
    // Start serial communication
    if (_useSoftwareSerial) {
        _swSerial->begin(baudRate);
    } else {
        _hwSerial->begin(baudRate);
    }
    
    delay(100); // Give sensor time to initialize
    
    // Initialize Adafruit fingerprint library
    if (!_finger->begin(baudRate)) {
        return false;
    }
    
    // Verify password
    if (_finger->verifyPassword() != FINGERPRINT_OK) {
        return false;
    }
    
    // Auto-detect sensor if needed
    if (_sensorType == UF_SENSOR_AUTO) {
        _sensorDetector = new UF_SensorDetector(_finger);
        _sensorType = _sensorDetector->detect();
        
        if (_sensorType == UF_SENSOR_AUTO) {
            // Failed to detect, default to AS608
            _sensorType = UF_SENSOR_AS608;
        }
    }
    
    // Get sensor info
    for (const auto& info : SENSOR_DB) {
        if (info.type == _sensorType) {
            _capacity = info.capacity;
            break;
        }
    }
    
    if (_capacity == 0) {
        // Default to AS608 if still unknown
        _sensorType = UF_SENSOR_AS608;
        _capacity = 162;
    }
    
    // Initialize memory manager
    _memoryManager = new UF_MemoryManager(_finger, _capacity);
    
    // Allocate slot occupancy array
    _slotOccupancy = new bool[_capacity];
    memset(_slotOccupancy, 0, _capacity * sizeof(bool));
    
    // Scan database
    scanDatabase();
    
    _initialized = true;
    return true;
}

// ==================== SENSOR INFORMATION ====================

String UniversalFingerprint::getSensorModel() const {
    for (const auto& info : SENSOR_DB) {
        if (info.type == _sensorType) {
            return String(info.modelName);
        }
    }
    return "Unknown";
}

String UniversalFingerprint::getSensorVendor() const {
    for (const auto& info : SENSOR_DB) {
        if (info.type == _sensorType) {
            return String(info.vendor);
        }
    }
    return "Unknown";
}

bool UniversalFingerprint::getSensorInfo(UF_SensorInfo& info) {
    for (const auto& sensor : SENSOR_DB) {
        if (sensor.type == _sensorType) {
            info = sensor;
            return true;
        }
    }
    return false;
}

// ==================== DATABASE MANAGEMENT ====================

int16_t UniversalFingerprint::scanDatabase() {
    if (!_initialized || _slotOccupancy == nullptr) {
        return -1;
    }
    
    uint16_t count = 0;
    
    // Scan all slots
    for (uint16_t i = 0; i < _capacity; i++) {
        uint16_t id = i + 1;
        uint8_t result = _finger->getTemplateCount(id);
        
        if (result == FINGERPRINT_OK) {
            _slotOccupancy[i] = true;
            count++;
        } else {
            _slotOccupancy[i] = false;
        }
    }
    
    _enrolledCount = count;
    _databaseScanned = true;
    
    return count;
}

bool UniversalFingerprint::isSlotOccupied(uint16_t id) {
    if (!_databaseScanned || id < 1 || id > _capacity) {
        return false;
    }
    return _slotOccupancy[id - 1];
}

uint16_t UniversalFingerprint::findEmptySlot(uint16_t start) {
    if (!_databaseScanned || start < 1 || start > _capacity) {
        return 0;
    }
    
    for (uint16_t i = start - 1; i < _capacity; i++) {
        if (!_slotOccupancy[i]) {
            return i + 1;
        }
    }
    
    return 0;
}

uint16_t UniversalFingerprint::findLastEmptySlot() {
    if (!_databaseScanned) {
        return 0;
    }
    
    for (uint16_t i = _capacity; i > 0; i--) {
        if (!_slotOccupancy[i - 1]) {
            return i;
        }
    }
    
    return 0;
}

bool UniversalFingerprint::getDatabaseStats(UF_DatabaseStats& stats) {
    if (!_databaseScanned) {
        scanDatabase();
    }
    
    stats.totalSlots = _capacity;
    stats.occupiedSlots = _enrolledCount;
    stats.freeSlots = _capacity - _enrolledCount;
    stats.firstFreeSlot = findEmptySlot();
    stats.lastFreeSlot = findLastEmptySlot();
    
    if (_capacity > 0) {
        stats.usagePercentage = (static_cast<float>(_enrolledCount) / _capacity) * 100.0f;
    } else {
        stats.usagePercentage = 0.0f;
    }
    
    return true;
}

void UniversalFingerprint::printDatabaseInfo() {
    if (!_initialized) {
        Serial.println("ERROR: Sensor not initialized");
        return;
    }
    
    UF_DatabaseStats stats;
    if (!getDatabaseStats(stats)) {
        Serial.println("ERROR: Failed to get database stats");
        return;
    }
    
    Serial.println("\n=== FINGERPRINT DATABASE ===");
    Serial.print("Sensor: ");
    Serial.println(getSensorModel());
    Serial.print("Capacity: ");
    Serial.println(stats.totalSlots);
    Serial.print("Enrolled: ");
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
    
    // Show visual representation (first 100 slots max)
    uint16_t showSlots = min(_capacity, (uint16_t)100);
    Serial.println("\nSlot Map (O=Occupied, .=Empty):");
    
    for (uint16_t i = 0; i < showSlots; i++) {
        if (i % 50 == 0 && i > 0) Serial.println();
        if (i % 10 == 0) Serial.print(" ");
        
        Serial.print(_slotOccupancy[i] ? 'O' : '.');
    }
    
    if (_capacity > showSlots) {
        Serial.print(" ... (+");
        Serial.print(_capacity - showSlots);
        Serial.print(" more)");
    }
    
    Serial.println("\n============================\n");
}

// ==================== FINGERPRINT OPERATIONS ====================

UF_ErrorCode UniversalFingerprint::enroll(uint16_t id, uint8_t numScans) {
    if (!_initialized) {
        return UF_ERROR_NO_SENSOR;
    }
    
    if (numScans < 1 || numScans > 4) {
        numScans = 2; // Default to 2 scans
    }
    
    // Auto-select ID if 0
    if (id == 0) {
        id = findEmptySlot();
        if (id == 0) {
            return UF_ERROR_SLOT_FULL;
        }
    }
    
    // Validate ID
    if (!_validateID(id)) {
        return UF_ERROR_INVALID_ID;
    }
    
    // Check if slot is occupied
    if (_databaseScanned && _slotOccupancy[id - 1]) {
        return UF_ERROR_DUPLICATE_ID;
    }
    
    Serial.print("Enrolling fingerprint ID #");
    Serial.println(id);
    Serial.println("Place finger on sensor...");
    
    // Enrollment process
    UF_ErrorCode result = _enrollInternal(id);
    
    if (result == UF_OK) {
        _updateSlotOccupancy(id, true);
        _enrolledCount++;
        Serial.println("Enrollment SUCCESS!");
    } else {
        Serial.print("Enrollment FAILED: ");
        Serial.println(errorToString(result));
    }
    
    return result;
}

int16_t UniversalFingerprint::identify(uint16_t* confidence, uint8_t timeout) {
    if (!_initialized) {
        return -UF_ERROR_NO_SENSOR;
    }
    
    unsigned long startTime = millis();
    
    // Wait for finger
    while (millis() - startTime < timeout * 1000) {
        if (isFingerPresent()) {
            break;
        }
        delay(100);
    }
    
    if (!isFingerPresent()) {
        return -UF_ERROR_TIMEOUT;
    }
    
    // Get image
    uint8_t result = _finger->getImage();
    if (result != FINGERPRINT_OK) {
        return -_convertAdafruitError(result);
    }
    
    // Convert image
    result = _finger->image2Tz();
    if (result != FINGERPRINT_OK) {
        return -_convertAdafruitError(result);
    }
    
    // Search database
    result = _finger->fingerFastSearch();
    if (result == FINGERPRINT_OK) {
        if (confidence != nullptr) {
            *confidence = _finger->confidence;
        }
        return _finger->fingerID;
    } else if (result == FINGERPRINT_NOTFOUND) {
        return 0; // Not found in database
    } else {
        return -_convertAdafruitError(result);
    }
}

UF_ErrorCode UniversalFingerprint::verify(uint16_t id, uint16_t* confidence) {
    if (!_initialized) {
        return UF_ERROR_NO_SENSOR;
    }
    
    if (!_validateID(id)) {
        return UF_ERROR_INVALID_ID;
    }
    
    if (!isFingerPresent()) {
        return UF_ERROR_NO_FINGER;
    }
    
    // Get image
    uint8_t result = _finger->getImage();
    if (result != FINGERPRINT_OK) {
        return _convertAdafruitError(result);
    }
    
    // Convert image
    result = _finger->image2Tz();
    if (result != FINGERPRINT_OK) {
        return _convertAdafruitError(result);
    }
    
    // Verify against specific ID
    result = _finger->fingerSearch(id);
    if (result == FINGERPRINT_OK) {
        if (confidence != nullptr) {
            *confidence = _finger->confidence;
        }
        return UF_OK;
    } else {
        return _convertAdafruitError(result);
    }
}

UF_ErrorCode UniversalFingerprint::deleteTemplate(uint16_t id) {
    if (!_initialized) {
        return UF_ERROR_NO_SENSOR;
    }
    
    if (!_validateID(id)) {
        return UF_ERROR_INVALID_ID;
    }
    
    uint8_t result = _finger->deleteModel(id);
    
    if (result == FINGERPRINT_OK) {
        _updateSlotOccupancy(id, false);
        if (_enrolledCount > 0) {
            _enrolledCount--;
        }
        return UF_OK;
    } else {
        return _convertAdafruitError(result);
    }
}

UF_ErrorCode UniversalFingerprint::clearDatabase() {
    if (!_initialized) {
        return UF_ERROR_NO_SENSOR;
    }
    
    bool success = true;
    
    for (uint16_t i = 1; i <= _capacity; i++) {
        uint8_t result = _finger->deleteModel(i);
        if (result != FINGERPRINT_OK && result != FINGERPRINT_BADLOCATION) {
            success = false;
        }
    }
    
    // Reset database tracking
    if (_slotOccupancy != nullptr) {
        memset(_slotOccupancy, 0, _capacity * sizeof(bool));
    }
    _enrolledCount = 0;
    _databaseScanned = false;
    
    return success ? UF_OK : UF_ERROR_COMM;
}

// ==================== SENSOR CONTROL ====================

UF_ErrorCode UniversalFingerprint::setLED(bool on, UF_LEDColor color, uint8_t speed) {
    if (!_initialized) {
        return UF_ERROR_NO_SENSOR;
    }
    
    // Note: Adafruit library LEDcontrol function has different parameters
    // We'll use the basic on/off control
    uint8_t result = _finger->LEDcontrol(on, speed, color);
    return _convertAdafruitError(result);
}

UF_ErrorCode UniversalFingerprint::setSecurity(uint8_t level) {
    if (!_initialized) {
        return UF_ERROR_NO_SENSOR;
    }
    
    if (level < 1 || level > 5) {
        level = 3; // Default
    }
    
    uint8_t result = _finger->setSecurityLevel(level);
    return _convertAdafruitError(result);
}

bool UniversalFingerprint::isFingerPresent() {
    if (!_initialized) {
        return false;
    }
    
    uint8_t result = _finger->getImage();
    return (result == FINGERPRINT_OK || result == FINGERPRINT_NOFINGER);
}

UF_ErrorCode UniversalFingerprint::setBaudRate(uint32_t baudRate) {
    if (!_initialized) {
        return UF_ERROR_NO_SENSOR;
    }
    
    uint8_t result = _finger->setBaudRate(baudRate);
    return _convertAdafruitError(result);
}

UF_ErrorCode UniversalFingerprint::sleep() {
    if (!_initialized) {
        return UF_ERROR_NO_SENSOR;
    }
    
    // Some sensors support sleep mode
    // This is sensor-dependent and may not work on all models
    return UF_OK; // Placeholder
}

UF_ErrorCode UniversalFingerprint::wake() {
    if (!_initialized) {
        return UF_ERROR_NO_SENSOR;
    }
    
    // Wake from sleep
    return UF_OK; // Placeholder
}

// ==================== ADVANCED OPERATIONS ====================

int16_t UniversalFingerprint::readRawTemplate(uint16_t id, uint8_t* buffer, uint16_t bufferSize) {
    if (!_initialized) {
        return -UF_ERROR_NO_SENSOR;
    }
    
    if (!_validateID(id) || buffer == nullptr || bufferSize < 128) {
        return -UF_ERROR_INVALID_PARAM;
    }
    
    // Note: Raw template reading requires sensor-specific implementation
    // This is a placeholder that may need modification based on sensor type
    return -UF_ERROR_NOT_SUPPORTED;
}

UF_ErrorCode UniversalFingerprint::writeRawTemplate(uint16_t id, uint8_t* buffer, uint16_t size) {
    if (!_initialized) {
        return UF_ERROR_NO_SENSOR;
    }
    
    if (!_validateID(id) || buffer == nullptr || size < 128) {
        return UF_ERROR_INVALID_PARAM;
    }
    
    // Note: Raw template writing requires sensor-specific implementation
    return UF_ERROR_NOT_SUPPORTED;
}

UF_ErrorCode UniversalFingerprint::getTemplateInfo(uint16_t id, UF_TemplateInfo& info) {
    if (!_initialized) {
        return UF_ERROR_NO_SENSOR;
    }
    
    if (!_validateID(id)) {
        return UF_ERROR_INVALID_ID;
    }
    
    info.id = id;
    info.isOccupied = isSlotOccupied(id);
    info.confidence = 0; // Would need to be populated during match
    info.enrollTime = 0; // Not available from basic sensors
    
    return UF_OK;
}

// ==================== UTILITY FUNCTIONS ====================

String UniversalFingerprint::errorToString(UF_ErrorCode error) {
    switch (error) {
        case UF_OK: return "Success";
        case UF_ERROR_COMM: return "Communication error";
        case UF_ERROR_NO_SENSOR: return "No sensor found";
        case UF_ERROR_INVALID_ID: return "Invalid template ID";
        case UF_ERROR_SLOT_FULL: return "No empty slots available";
        case UF_ERROR_TIMEOUT: return "Operation timeout";
        case UF_ERROR_SENSOR_BUSY: return "Sensor is busy";
        case UF_ERROR_PACKET: return "Invalid packet";
        case UF_ERROR_NOT_ENROLLED: return "Finger not enrolled";
        case UF_ERROR_NO_FINGER: return "No finger detected";
        case UF_ERROR_IMAGE_MESS: return "Image too messy";
        case UF_ERROR_FEATURE_FAIL: return "Feature extraction failed";
        case UF_ERROR_IMAGE_FAIL: return "Image capture failed";
        case UF_ERROR_DUPLICATE_ID: return "Slot already occupied";
        case UF_ERROR_NOT_SUPPORTED: return "Feature not supported";
        case UF_ERROR_INVALID_PARAM: return "Invalid parameter";
        default: return "Unknown error";
    }
}

String UniversalFingerprint::sensorTypeToString(UF_SensorType type) {
    switch (type) {
        case UF_SENSOR_AS608: return "AS608";
        case UF_SENSOR_R307: return "R307";
        case UF_SENSOR_GT511C3: return "GT-511C3";
        case UF_SENSOR_ZFM60: return "ZFM-60";
        case UF_SENSOR_ZFM20: return "ZFM-20";
        case UF_SENSOR_AUTO: return "Auto-detect";
        default: return "Unknown";
    }
}

String UniversalFingerprint::ledColorToString(UF_LEDColor color) {
    switch (color) {
        case UF_LED_RED: return "Red";
        case UF_LED_BLUE: return "Blue";
        case UF_LED_PURPLE: return "Purple";
        case UF_LED_GREEN: return "Green";
        case UF_LED_CYAN: return "Cyan";
        case UF_LED_YELLOW: return "Yellow";
        case UF_LED_WHITE: return "White";
        default: return "Unknown";
    }
}

// ==================== PRIVATE METHODS ====================

bool UniversalFingerprint::_initSensor(uint32_t password) {
    return _finger->begin(password) == FINGERPRINT_OK;
}

bool UniversalFingerprint::_autoDetectSensor() {
    // Try to get sensor parameters
    uint32_t param = _finger->getParameters();
    
    if (param == 0) {
        return false;
    }
    
    // Try to determine sensor type by testing capacity
    // This is a heuristic approach
    for (uint16_t testID = 160; testID <= 1005; testID += 100) {
        if (_finger->getTemplateCount(testID) != FINGERPRINT_NOFINGER) {
            if (testID > 200) {
                _sensorType = UF_SENSOR_R307;
                _capacity = 1000;
            } else {
                _sensorType = UF_SENSOR_AS608;
                _capacity = 162;
            }
            return true;
        }
    }
    
    // Default to AS608
    _sensorType = UF_SENSOR_AS608;
    _capacity = 162;
    return true;
}

UF_ErrorCode UniversalFingerprint::_convertAdafruitError(uint8_t error) {
    switch (error) {
        case FINGERPRINT_OK: return UF_OK;
        case FINGERPRINT_NOFINGER: return UF_ERROR_NO_FINGER;
        case FINGERPRINT_IMAGEFAIL: return UF_ERROR_IMAGE_FAIL;
        case FINGERPRINT_IMAGEMESS: return UF_ERROR_IMAGE_MESS;
        case FINGERPRINT_PACKETRECIEVEERR: return UF_ERROR_COMM;
        case FINGERPRINT_FEATUREFAIL: return UF_ERROR_FEATURE_FAIL;
        case FINGERPRINT_INVALIDIMAGE: return UF_ERROR_IMAGE_FAIL;
        case FINGERPRINT_ENROLLMISMATCH: return UF_ERROR_FEATURE_FAIL;
        case FINGERPRINT_BADLOCATION: return UF_ERROR_INVALID_ID;
        case FINGERPRINT_DBRANGEFAIL: return UF_ERROR_INVALID_ID;
        case FINGERPRINT_UPLOADFEATUREFAIL: return UF_ERROR_FEATURE_FAIL;
        case FINGERPRINT_PACKETRESPONSEFAIL: return UF_ERROR_COMM;
        case FINGERPRINT_UPLOADFAIL: return UF_ERROR_COMM;
        case FINGERPRINT_DELETEFAIL: return UF_ERROR_COMM;
        case FINGERPRINT_DBCLEARFAIL: return UF_ERROR_COMM;
        case FINGERPRINT_PASSFAIL: return UF_ERROR_COMM;
        case FINGERPRINT_INVALIDREG: return UF_ERROR_INVALID_PARAM;
        case FINGERPRINT_FLASHERR: return UF_ERROR_COMM;
        default: return UF_ERROR_COMM;
    }
}

bool UniversalFingerprint::_validateID(uint16_t id) {
    return (id >= 1 && id <= _capacity);
}

void UniversalFingerprint::_updateSlotOccupancy(uint16_t id, bool occupied) {
    if (_slotOccupancy != nullptr && id >= 1 && id <= _capacity) {
        _slotOccupancy[id - 1] = occupied;
        _databaseScanned = true;
    }
}

bool UniversalFingerprint::_checkSensorResponse() {
    if (!_initialized) {
        return false;
    }
    
    // Simple ping to check if sensor responds
    uint8_t result = _finger->getImage();
    return (result == FINGERPRINT_OK || result == FINGERPRINT_NOFINGER);
}

UF_ErrorCode UniversalFingerprint::_enrollInternal(uint16_t id) {
    // Step 1: First scan
    Serial.println("Waiting for valid finger...");
    
    uint8_t result = _finger->getImage();
    if (result != FINGERPRINT_OK) {
        return _convertAdafruitError(result);
    }
    
    result = _finger->image2Tz(1);
    if (result != FINGERPRINT_OK) {
        return _convertAdafruitError(result);
    }
    
    Serial.println("Remove finger");
    delay(2000);
    
    // Step 2: Second scan
    Serial.println("Place same finger again...");
    
    unsigned long startTime = millis();
    while (millis() - startTime < 10000) { // 10 second timeout
        result = _finger->getImage();
        if (result == FINGERPRINT_NOFINGER) {
            continue;
        }
        if (result != FINGERPRINT_OK) {
            return _convertAdafruitError(result);
        }
        break;
    }
    
    result = _finger->image2Tz(2);
    if (result != FINGERPRINT_OK) {
        return _convertAdafruitError(result);
    }
    
    // Create model
    Serial.println("Creating model...");
    result = _finger->createModel();
    if (result != FINGERPRINT_OK) {
        return _convertAdafruitError(result);
    }
    
    // Store model
    Serial.println("Storing model...");
    result = _finger->storeModel(id);
    return _convertAdafruitError(result);
}
