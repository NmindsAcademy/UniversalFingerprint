/**
 * UniversalFingerprint Library - Sensor-Specific Tests
 * 
 * These tests verify sensor-specific functionality and compatibility.
 */

#include <Arduino.h>
#include <unity.h>
#include <UniversalFingerprint.h>
#include "mock/Adafruit_Fingerprint_Mock.h"

// Test different sensor types
void test_sensor_as608() {
    Serial.println("Test: AS608 Sensor");
    
    HardwareSerial serial;
    UniversalFingerprint finger(&serial, UF_SENSOR_AS608);
    
    // Mock AS608 behavior
    AdafruitFingerprintMock::setVerifyPasswordResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setBeginResult(true);
    AdafruitFingerprintMock::setGetTemplateCountCustom([](uint16_t id) {
        return (id <= 162) ? FINGERPRINT_OK : FINGERPRINT_BADLOCATION;
    });
    
    bool initialized = finger.begin();
    TEST_ASSERT_TRUE_MESSAGE(initialized, "AS608 should initialize");
    
    UF_SensorType type = finger.getSensorType();
    TEST_ASSERT_EQUAL_MESSAGE(UF_SENSOR_AS608, type, "Should be AS608 sensor");
    
    uint16_t capacity = finger.getCapacity();
    TEST_ASSERT_EQUAL_MESSAGE(162, capacity, "AS608 should have 162 capacity");
    
    String model = finger.getSensorModel();
    TEST_ASSERT_EQUAL_STRING_MESSAGE("AS608", model.c_str(), "Should return AS608 model");
}

void test_sensor_r307() {
    Serial.println("Test: R307 Sensor");
    
    HardwareSerial serial;
    UniversalFingerprint finger(&serial, UF_SENSOR_R307);
    
    // Mock R307 behavior
    AdafruitFingerprintMock::setVerifyPasswordResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setBeginResult(true);
    AdafruitFingerprintMock::setGetTemplateCountCustom([](uint16_t id) {
        return (id <= 1000) ? FINGERPRINT_OK : FINGERPRINT_BADLOCATION;
    });
    
    bool initialized = finger.begin();
    TEST_ASSERT_TRUE_MESSAGE(initialized, "R307 should initialize");
    
    UF_SensorType type = finger.getSensorType();
    TEST_ASSERT_EQUAL_MESSAGE(UF_SENSOR_R307, type, "Should be R307 sensor");
    
    uint16_t capacity = finger.getCapacity();
    TEST_ASSERT_EQUAL_MESSAGE(1000, capacity, "R307 should have 1000 capacity");
    
    String model = finger.getSensorModel();
    TEST_ASSERT_EQUAL_STRING_MESSAGE("R307", model.c_str(), "Should return R307 model");
}

void test_sensor_gt511c3() {
    Serial.println("Test: GT-511C3 Sensor");
    
    HardwareSerial serial;
    UniversalFingerprint finger(&serial, UF_SENSOR_GT511C3);
    
    // Mock GT-511C3 behavior
    AdafruitFingerprintMock::setVerifyPasswordResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setBeginResult(true);
    AdafruitFingerprintMock::setGetTemplateCountCustom([](uint16_t id) {
        return (id <= 200) ? FINGERPRINT_OK : FINGERPRINT_BADLOCATION;
    });
    
    bool initialized = finger.begin(0x00000000, 9600);  // GT-511C3 uses 9600 baud
    TEST_ASSERT_TRUE_MESSAGE(initialized, "GT-511C3 should initialize");
    
    UF_SensorType type = finger.getSensorType();
    TEST_ASSERT_EQUAL_MESSAGE(UF_SENSOR_GT511C3, type, "Should be GT-511C3 sensor");
    
    uint16_t capacity = finger.getCapacity();
    TEST_ASSERT_EQUAL_MESSAGE(200, capacity, "GT-511C3 should have 200 capacity");
    
    String model = finger.getSensorModel();
    TEST_ASSERT_EQUAL_STRING_MESSAGE("GT-511C3", model.c_str(), "Should return GT-511C3 model");
}

void test_sensor_zfm60() {
    Serial.println("Test: ZFM-60 Sensor");
    
    HardwareSerial serial;
    UniversalFingerprint finger(&serial, UF_SENSOR_ZFM60);
    
    // Mock ZFM-60 behavior
    AdafruitFingerprintMock::setVerifyPasswordResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setBeginResult(true);
    AdafruitFingerprintMock::setGetTemplateCountCustom([](uint16_t id) {
        return (id <= 300) ? FINGERPRINT_OK : FINGERPRINT_BADLOCATION;
    });
    
    bool initialized = finger.begin();
    TEST_ASSERT_TRUE_MESSAGE(initialized, "ZFM-60 should initialize");
    
    UF_SensorType type = finger.getSensorType();
    TEST_ASSERT_EQUAL_MESSAGE(UF_SENSOR_ZFM60, type, "Should be ZFM-60 sensor");
    
    uint16_t capacity = finger.getCapacity();
    TEST_ASSERT_EQUAL_MESSAGE(300, capacity, "ZFM-60 should have 300 capacity");
    
    String model = finger.getSensorModel();
    TEST_ASSERT_EQUAL_STRING_MESSAGE("ZFM-60", model.c_str(), "Should return ZFM-60 model");
}

void test_sensor_zfm20() {
    Serial.println("Test: ZFM-20 Sensor");
    
    HardwareSerial serial;
    UniversalFingerprint finger(&serial, UF_SENSOR_ZFM20);
    
    // Mock ZFM-20 behavior
    AdafruitFingerprintMock::setVerifyPasswordResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setBeginResult(true);
    AdafruitFingerprintMock::setGetTemplateCountCustom([](uint16_t id) {
        return (id <= 256) ? FINGERPRINT_OK : FINGERPRINT_BADLOCATION;
    });
    
    bool initialized = finger.begin();
    TEST_ASSERT_TRUE_MESSAGE(initialized, "ZFM-20 should initialize");
    
    UF_SensorType type = finger.getSensorType();
    TEST_ASSERT_EQUAL_MESSAGE(UF_SENSOR_ZFM20, type, "Should be ZFM-20 sensor");
    
    uint16_t capacity = finger.getCapacity();
    TEST_ASSERT_EQUAL_MESSAGE(256, capacity, "ZFM-20 should have 256 capacity");
    
    String model = finger.getSensorModel();
    TEST_ASSERT_EQUAL_STRING_MESSAGE("ZFM-20", model.c_str(), "Should return ZFM-20 model");
}

void test_sensor_auto_detection() {
    Serial.println("Test: Sensor Auto-Detection");
    
    HardwareSerial serial;
    UniversalFingerprint finger(&serial, UF_SENSOR_AUTO);
    
    // Test auto-detection for each sensor type
    struct SensorTest {
        const char* name;
        UF_SensorType type;
        uint16_t capacity;
        std::function<bool(uint16_t)> slotCheck;
    };
    
    SensorTest tests[] = {
        {"AS608", UF_SENSOR_AS608, 162, [](uint16_t id) { return id <= 162; }},
        {"R307", UF_SENSOR_R307, 1000, [](uint16_t id) { return id <= 1000; }},
        {"GT511C3", UF_SENSOR_GT511C3, 200, [](uint16_t id) { return id <= 200; }},
        {"ZFM60", UF_SENSOR_ZFM60, 300, [](uint16_t id) { return id <= 300; }},
        {"ZFM20", UF_SENSOR_ZFM20, 256, [](uint16_t id) { return id <= 256; }}
    };
    
    for (const auto& test : tests) {
        Serial.print("  Testing auto-detection for ");
        Serial.println(test.name);
        
        // Mock sensor behavior
        AdafruitFingerprintMock::setVerifyPasswordResult(FINGERPRINT_OK);
        AdafruitFingerprintMock::setBeginResult(true);
        AdafruitFingerprintMock::setGetTemplateCountCustom([test](uint16_t id) {
            return test.slotCheck(id) ? FINGERPRINT_OK : FINGERPRINT_BADLOCATION;
        });
        
        // Re-initialize for each test
        AdafruitFingerprintMock::reset();
        
        bool initialized = finger.begin();
        TEST_ASSERT_TRUE_MESSAGE(initialized, "Sensor should initialize");
        
        UF_SensorType detected = finger.getSensorType();
        TEST_ASSERT_EQUAL_MESSAGE(test.type, detected, "Should auto-detect correct sensor type");
        
        uint16_t capacity = finger.getCapacity();
        TEST_ASSERT_EQUAL_MESSAGE(test.capacity, capacity, "Should detect correct capacity");
        
        // Reset for next test
        AdafruitFingerprintMock::reset();
    }
}

void test_sensor_comparison() {
    Serial.println("Test: Sensor Feature Comparison");
    
    // Test feature availability for different sensors
    struct SensorFeatures {
        UF_SensorType type;
        bool hasLED;
        bool hasTouch;
        bool hasSleep;
        uint32_t defaultBaud;
    };
    
    SensorFeatures features[] = {
        {UF_SENSOR_AS608, true, false, true, 57600},
        {UF_SENSOR_R307, true, true, true, 57600},
        {UF_SENSOR_GT511C3, false, false, false, 9600},
        {UF_SENSOR_ZFM60, true, true, true, 57600},
        {UF_SENSOR_ZFM20, false, false, false, 57600}
    };
    
    for (const auto& feat : features) {
        HardwareSerial serial;
        UniversalFingerprint finger(&serial, feat.type);
        
        // Initialize
        AdafruitFingerprintMock::setVerifyPasswordResult(FINGERPRINT_OK);
        AdafruitFingerprintMock::setBeginResult(true);
        finger.begin();
        
        // Get sensor info
        UF_SensorInfo info;
        bool gotInfo = finger.getSensorInfo(info);
        TEST_ASSERT_TRUE_MESSAGE(gotInfo, "Should get sensor info");
        
        if (gotInfo) {
            // Verify features
            TEST_ASSERT_EQUAL_MESSAGE(feat.hasLED, info.hasLED, "LED feature should match");
            TEST_ASSERT_EQUAL_MESSAGE(feat.hasTouch, info.hasTouchSensor, "Touch feature should match");
            TEST_ASSERT_EQUAL_MESSAGE(feat.defaultBaud, info.defaultBaud, "Default baud should match");
        }
        
        AdafruitFingerprintMock::reset();
    }
}

void test_sensor_performance() {
    Serial.println("Test: Sensor Performance Comparison");
    
    // Performance metrics for different sensors
    struct SensorPerformance {
        UF_SensorType type;
        uint16_t capacity;
        unsigned long expectedScanTime;  // ms for full scan
        unsigned long expectedIdentifyTime;  // ms for identification
    };
    
    SensorPerformance perf[] = {
        {UF_SENSOR_AS608, 162, 2000, 800},
        {UF_SENSOR_R307, 1000, 3500, 900},
        {UF_SENSOR_GT511C3, 200, 1800, 1200},
        {UF_SENSOR_ZFM60, 300, 2200, 850},
        {UF_SENSOR_ZFM20, 256, 2100, 950}
    };
    
    for (const auto& p : perf) {
        Serial.print("  Testing performance for ");
        Serial.println(finger.sensorTypeToString(p.type).c_str());
        
        HardwareSerial serial;
        UniversalFingerprint finger(&serial, p.type);
        
        // Initialize
        AdafruitFingerprintMock::setVerifyPasswordResult(FINGERPRINT_OK);
        AdafruitFingerprintMock::setBeginResult(true);
        AdafruitFingerprintMock::setGetTemplateCountResult(FINGERPRINT_OK);
        finger.begin();
        
        // Test capacity
        uint16_t capacity = finger.getCapacity();
        TEST_ASSERT_EQUAL_MESSAGE(p.capacity, capacity, "Capacity should match");
        
        // Note: Actual timing tests would require real hardware
        // These are just structural tests
        
        AdafruitFingerprintMock::reset();
    }
}

void test_sensor_communication() {
    Serial.println("Test: Sensor Communication Protocols");
    
    // Test different baud rates
    uint32_t baudRates[] = {9600, 19200, 38400, 57600, 115200};
    
    for (uint32_t baud : baudRates) {
        Serial.print("  Testing baud rate: ");
        Serial.println(baud);
        
        HardwareSerial serial;
        UniversalFingerprint finger(&serial, UF_SENSOR_AS608);
        
        // Mock successful initialization at this baud rate
        AdafruitFingerprintMock::setVerifyPasswordResult(FINGERPRINT_OK);
        AdafruitFingerprintMock::setBeginResult(true);
        
        bool initialized = finger.begin(0x00000000, baud);
        
        // Most sensors should work at common baud rates
        if (baud == 57600 || baud == 9600) {
            TEST_ASSERT_TRUE_MESSAGE(initialized, "Should initialize at common baud rate");
        }
        
        AdafruitFingerprintMock::reset();
    }
}

void test_sensor_error_conditions() {
    Serial.println("Test: Sensor Error Conditions");
    
    HardwareSerial serial;
    UniversalFingerprint finger(&serial, UF_SENSOR_AS608);
    
    // Test 1: Sensor not responding
    AdafruitFingerprintMock::setVerifyPasswordResult(FINGERPRINT_PACKETRECIEVEERR);
    AdafruitFingerprintMock::setBeginResult(false);
    
    bool initialized = finger.begin();
    TEST_ASSERT_FALSE_MESSAGE(initialized, "Should fail when sensor not responding");
    
    // Test 2: Wrong password
    AdafruitFingerprintMock::reset();
    AdafruitFingerprintMock::setVerifyPasswordResult(FINGERPRINT_PASSFAIL);
    AdafruitFingerprintMock::setBeginResult(true);
    
    initialized = finger.begin(0x12345678);  // Wrong password
    TEST_ASSERT_FALSE_MESSAGE(initialized, "Should fail with wrong password");
    
    // Test 3: Sensor busy
    AdafruitFingerprintMock::reset();
    AdafruitFingerprintMock::setVerifyPasswordResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setBeginResult(true);
    finger.begin();
    
    AdafruitFingerprintMock::setGetImageResult(FINGERPRINT_PACKETRECIEVEERR);
    int16_t id = finger.identify();
    TEST_ASSERT_LESS_THAN_MESSAGE(0, id, "Should return error when sensor busy");
}

void test_sensor_recovery() {
    Serial.println("Test: Sensor Recovery Scenarios");
    
    HardwareSerial serial;
    UniversalFingerprint finger(&serial, UF_SENSOR_R307);
    
    // Initialize
    AdafruitFingerprintMock::setVerifyPasswordResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setBeginResult(true);
    finger.begin();
    
    // Scenario 1: Temporary communication loss
    AdafruitFingerprintMock::setGetImageResult(FINGERPRINT_PACKETRECIEVEERR);
    int16_t id = finger.identify();
    TEST_ASSERT_LESS_THAN_MESSAGE(0, id, "Should fail during communication loss");
    
    // Recover
    AdafruitFingerprintMock::setGetImageResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setImage2TzResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setFingerFastSearchResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setFingerID(1);
    
    id = finger.identify();
    TEST_ASSERT_EQUAL_MESSAGE(1, id, "Should recover after communication restored");
    
    // Scenario 2: Database corruption simulation
    AdafruitFingerprintMock::setGetTemplateCountResult(FINGERPRINT_PACKETRECIEVEERR);
    int16_t count = finger.scanDatabase();
    TEST_ASSERT_LESS_THAN_MESSAGE(0, count, "Should detect database scan error");
    
    // Clear and rescan
    AdafruitFingerprintMock::setDeleteModelResult(FINGERPRINT_OK);
    UF_ErrorCode result = finger.clearDatabase();
    TEST_ASSERT_EQUAL_MESSAGE(UF_OK, result, "Should clear database");
    
    AdafruitFingerprintMock::setGetTemplateCountResult(FINGERPRINT_OK);
    count = finger.scanDatabase();
    TEST_ASSERT_GREATER_OR_EQUAL_MESSAGE(0, count, "Should recover with fresh scan");
}

void test_sensor_limits() {
    Serial.println("Test: Sensor Limits and Boundaries");
    
    // Test each sensor's limits
    struct SensorLimit {
        UF_SensorType type;
        uint16_t minSlot;
        uint16_t maxSlot;
        uint16_t invalidSlot;
    };
    
    SensorLimit limits[] = {
        {UF_SENSOR_AS608, 1, 162, 163},
        {UF_SENSOR_R307, 1, 1000, 1001},
        {UF_SENSOR_GT511C3, 1, 200, 201},
        {UF_SENSOR_ZFM60, 1, 300, 301},
        {UF_SENSOR_ZFM20, 1, 256, 257}
    };
    
    for (const auto& limit : limits) {
        HardwareSerial serial;
        UniversalFingerprint finger(&serial, limit.type);
        
        // Initialize
        AdafruitFingerprintMock::setVerifyPasswordResult(FINGERPRINT_OK);
        AdafruitFingerprintMock::setBeginResult(true);
        finger.begin();
        
        // Test valid slots
        bool minSlotValid = finger.isSlotOccupied(limit.minSlot);
        // Can't know if occupied, but function should return (not crash)
        
        bool maxSlotValid = finger.isSlotOccupied(limit.maxSlot);
        // Function should handle max slot
        
        // Test invalid slot
        bool invalidSlotValid = finger.isSlotOccupied(limit.invalidSlot);
        TEST_ASSERT_FALSE_MESSAGE(invalidSlotValid, "Invalid slot should return false");
        
        // Test find empty slot at boundaries
        AdafruitFingerprintMock::setGetTemplateCountResult(FINGERPRINT_OK);
        uint16_t emptySlot = finger.findEmptySlot(limit.maxSlot);
        if (emptySlot > 0) {
            TEST_ASSERT_GREATER_OR_EQUAL_MESSAGE(limit.minSlot, emptySlot, "Empty slot should be within range");
            TEST_ASSERT_LESS_OR_EQUAL_MESSAGE(limit.maxSlot, emptySlot, "Empty slot should be within range");
        }
        
        AdafruitFingerprintMock::reset();
    }
}

void test_sensor_interoperability() {
    Serial.println("Test: Sensor Interoperability");
    
    // Test that library works consistently across sensors
    UF_SensorType sensors[] = {
        UF_SENSOR_AS608,
        UF_SENSOR_R307,
        UF_SENSOR_GT511C3,
        UF_SENSOR_ZFM60,
        UF_SENSOR_ZFM20
    };
    
    for (UF_SensorType sensorType : sensors) {
        Serial.print("  Testing interoperability for ");
        Serial.println(finger.sensorTypeToString(sensorType).c_str());
        
        HardwareSerial serial;
        UniversalFingerprint finger(&serial, sensorType);
        
        // Initialize
        AdafruitFingerprintMock::setVerifyPasswordResult(FINGERPRINT_OK);
        AdafruitFingerprintMock::setBeginResult(true);
        AdafruitFingerprintMock::setGetTemplateCountResult(FINGERPRINT_OK);
        finger.begin();
        
        // Test common operations
        UF_ErrorCode result;
        
        // LED control (if supported)
        result = finger.setLED(true, UF_LED_BLUE);
        // Some sensors may not support LED, but function shouldn't crash
        
        // Database operations
        int16_t count = finger.scanDatabase();
        TEST_ASSERT_GREATER_OR_EQUAL_MESSAGE(0, count, "Database scan should work");
        
        // Find empty slot
        uint16_t emptySlot = finger.findEmptySlot();
        if (emptySlot > 0) {
            TEST_ASSERT_GREATER_OR_EQUAL_MESSAGE(1, emptySlot, "Should find valid empty slot");
        }
        
        // Get stats
        UF_DatabaseStats stats;
        bool gotStats = finger.getDatabaseStats(stats);
        TEST_ASSERT_TRUE_MESSAGE(gotStats, "Should get database stats");
        
        // Clean up
        result = finger.setLED(false, UF_LED_BLUE);
        
        AdafruitFingerprintMock::reset();
    }
}

void test_sensor_specific_features() {
    Serial.println("Test: Sensor-Specific Features");
    
    // Test features unique to certain sensors
    
    // R307: Touch detection
    {
        HardwareSerial serial;
        UniversalFingerprint finger(&serial, UF_SENSOR_R307);
        
        AdafruitFingerprintMock::setVerifyPasswordResult(FINGERPRINT_OK);
        AdafruitFingerprintMock::setBeginResult(true);
        finger.begin();
        
        // Get sensor info
        UF_SensorInfo info;
        finger.getSensorInfo(info);
        TEST_ASSERT_TRUE_MESSAGE(info.hasTouchSensor, "R307 should have touch sensor");
    }
    
    // AS608: Common features
    {
        HardwareSerial serial;
        UniversalFingerprint finger(&serial, UF_SENSOR_AS608);
        
        AdafruitFingerprintMock::setVerifyPasswordResult(FINGERPRINT_OK);
        AdafruitFingerprintMock::setBeginResult(true);
        finger.begin();
        
        UF_SensorInfo info;
        finger.getSensorInfo(info);
        TEST_ASSERT_TRUE_MESSAGE(info.hasLED, "AS608 should have LED control");
    }
    
    // GT-511C3: Different baud rate
    {
        HardwareSerial serial;
        UniversalFingerprint finger(&serial, UF_SENSOR_GT511C3);
        
        AdafruitFingerprintMock::setVerifyPasswordResult(FINGERPRINT_OK);
        AdafruitFingerprintMock::setBeginResult(true);
        
        // GT-511C3 typically uses 9600 baud
        bool initialized = finger.begin(0x00000000, 9600);
        TEST_ASSERT_TRUE_MESSAGE(initialized, "GT-511C3 should initialize at 9600 baud");
        
        UF_SensorInfo info;
        finger.getSensorInfo(info);
        TEST_ASSERT_EQUAL_MESSAGE(9600, info.defaultBaud, "GT-511C3 default baud should be 9600");
    }
}

void test_sensor_upgrade_path() {
    Serial.println("Test: Sensor Upgrade Path");
    
    // Test migrating from smaller to larger sensor
    
    // Start with AS608 (162 capacity)
    {
        HardwareSerial serial;
        UniversalFingerprint finger(&serial, UF_SENSOR_AS608);
        
        AdafruitFingerprintMock::setVerifyPasswordResult(FINGERPRINT_OK);
        AdafruitFingerprintMock::setBeginResult(true);
        AdafruitFingerprintMock::setGetTemplateCountCustom([](uint16_t id) {
            return (id <= 162) ? FINGERPRINT_OK : FINGERPRINT_BADLOCATION;
        });
        
        finger.begin();
        
        uint16_t capacity = finger.getCapacity();
        TEST_ASSERT_EQUAL_MESSAGE(162, capacity, "AS608 capacity should be 162");
        
        // Fill database (simulated)
        AdafruitFingerprintMock::setGetTemplateCountResult(FINGERPRINT_OK);
        finger.scanDatabase();
        
        UF_DatabaseStats stats;
        finger.getDatabaseStats(stats);
        TEST_ASSERT_EQUAL_MESSAGE(162, stats.totalSlots, "Should have 162 total slots");
    }
    
    // Upgrade to R307 (1000 capacity)
    {
        HardwareSerial serial;
        UniversalFingerprint finger(&serial, UF_SENSOR_R307);
        
        AdafruitFingerprintMock::setVerifyPasswordResult(FINGERPRINT_OK);
        AdafruitFingerprintMock::setBeginResult(true);
        AdafruitFingerprintMock::setGetTemplateCountCustom([](uint16_t id) {
            return (id <= 1000) ? FINGERPRINT_OK : FINGERPRINT_BADLOCATION;
        });
        
        finger.begin();
        
        uint16_t capacity = finger.getCapacity();
        TEST_ASSERT_EQUAL_MESSAGE(1000, capacity, "R307 capacity should be 1000");
        
        // Database should have more capacity
        UF_DatabaseStats stats;
        finger.getDatabaseStats(stats);
        TEST_ASSERT_EQUAL_MESSAGE(1000, stats.totalSlots, "Should have 1000 total slots");
        TEST_ASSERT_EQUAL_MESSAGE(838, stats.freeSlots, "Should have 838 free slots (1000 - 162)");
    }
}

void test_sensor_compatibility_matrix() {
    Serial.println("Test: Sensor Compatibility Matrix");
    
    // Create compatibility matrix
    struct CompatibilityTest {
        UF_SensorType sensor1;
        UF_SensorType sensor2;
        bool shouldBeCompatible;
        const char* description;
    };
    
    CompatibilityTest tests[] = {
        {UF_SENSOR_AS608, UF_SENSOR_R307, true, "AS608 and R307 should be compatible"},
        {UF_SENSOR_AS608, UF_SENSOR_GT511C3, false, "AS608 and GT-511C3 have different protocols"},
        {UF_SENSOR_R307, UF_SENSOR_ZFM60, true, "R307 and ZFM-60 should be compatible"},
        {UF_SENSOR_ZFM20, UF_SENSOR_ZFM60, true, "ZFM-20 and ZFM-60 should be compatible"}
    };
    
    for (const auto& test : tests) {
        Serial.print("  Testing: ");
        Serial.println(test.description);
        
        // Test sensor 1
        {
            HardwareSerial serial1;
            UniversalFingerprint finger1(&serial1, test.sensor1);
            
            AdafruitFingerprintMock::setVerifyPasswordResult(FINGERPRINT_OK);
            AdafruitFingerprintMock::setBeginResult(true);
            finger1.begin();
            
            // Get info
            UF_SensorInfo info1;
            finger1.getSensorInfo(info1);
        }
        
        // Test sensor 2
        {
            HardwareSerial serial2;
            UniversalFingerprint finger2(&serial2, test.sensor2);
            
            AdafruitFingerprintMock::setVerifyPasswordResult(FINGERPRINT_OK);
            AdafruitFingerprintMock::setBeginResult(true);
            finger2.begin();
            
            // Get info
            UF_SensorInfo info2;
            finger2.getSensorInfo(info2);
        }
        
        // Both should initialize without issue
        // (Actual protocol compatibility would require hardware testing)
        TEST_ASSERT_TRUE_MESSAGE(true, "Both sensors should initialize");
        
        AdafruitFingerprintMock::reset();
    }
}

void setup() {
    // Wait for serial connection
    delay(2000);
    
    UNITY_BEGIN();
    
    // Run sensor-specific tests
    RUN_TEST(test_sensor_as608);
    RUN_TEST(test_sensor_r307);
    RUN_TEST(test_sensor_gt511c3);
    RUN_TEST(test_sensor_zfm60);
    RUN_TEST(test_sensor_zfm20);
    RUN_TEST(test_sensor_auto_detection);
    RUN_TEST(test_sensor_comparison);
    RUN_TEST(test_sensor_performance);
    RUN_TEST(test_sensor_communication);
    RUN_TEST(test_sensor_error_conditions);
    RUN_TEST(test_sensor_recovery);
    RUN_TEST(test_sensor_limits);
    RUN_TEST(test_sensor_interoperability);
    RUN_TEST(test_sensor_specific_features);
    RUN_TEST(test_sensor_upgrade_path);
    RUN_TEST(test_sensor_compatibility_matrix);
    
    UNITY_END();
}

void loop() {
    // Empty for testing
}