/**
 * UniversalFingerprint Library - Core Tests
 * 
 * These tests verify the core functionality of the library.
 * Run with PlatformIO test system or Arduino Unit Testing framework.
 */

#include <Arduino.h>
#include <unity.h>
#include <UniversalFingerprint.h>
#include "mock/Adafruit_Fingerprint_Mock.h"

// Mock serial for testing
HardwareSerial MockSerial;
UniversalFingerprint finger(&MockSerial);

// Test data
const uint16_t TEST_CAPACITY = 162;  // AS608
const uint16_t TEST_ID = 1;
const uint32_t TEST_PASSWORD = 0x00000000;

void setUp() {
    // Reset mocks before each test
    AdafruitFingerprintMock::reset();
    MockSerial.clear();
}

void tearDown() {
    // Clean up after each test
}

// Test 1: Library Initialization
void test_library_initialization() {
    Serial.println("Test 1: Library Initialization");
    
    // Mock successful sensor initialization
    AdafruitFingerprintMock::setVerifyPasswordResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setBeginResult(true);
    
    bool result = finger.begin(TEST_PASSWORD, 57600);
    
    TEST_ASSERT_TRUE_MESSAGE(result, "Sensor should initialize successfully");
    TEST_ASSERT_TRUE_MESSAGE(finger.isInitialized(), "Sensor should be marked as initialized");
    TEST_ASSERT_EQUAL_MESSAGE(UF_SENSOR_AS608, finger.getSensorType(), "Should detect AS608 sensor");
}

// Test 2: Sensor Information
void test_sensor_information() {
    Serial.println("Test 2: Sensor Information");
    
    finger.begin(TEST_PASSWORD, 57600);
    
    // Test sensor model
    String model = finger.getSensorModel();
    TEST_ASSERT_EQUAL_STRING_MESSAGE("AS608", model.c_str(), "Should return correct sensor model");
    
    // Test capacity
    uint16_t capacity = finger.getCapacity();
    TEST_ASSERT_EQUAL_MESSAGE(TEST_CAPACITY, capacity, "Should return correct capacity");
    
    // Test version
    String version = finger.getVersion();
    TEST_ASSERT_TRUE_MESSAGE(version.length() > 0, "Should return version string");
}

// Test 3: Database Management
void test_database_management() {
    Serial.println("Test 3: Database Management");
    
    finger.begin(TEST_PASSWORD, 57600);
    
    // Mock empty database
    AdafruitFingerprintMock::setGetTemplateCountResult(FINGERPRINT_OK);
    
    // Test database scan
    int16_t count = finger.scanDatabase();
    TEST_ASSERT_EQUAL_MESSAGE(0, count, "Should find 0 templates in empty database");
    
    // Test get enrolled count
    uint16_t enrolled = finger.getEnrolledCount();
    TEST_ASSERT_EQUAL_MESSAGE(0, enrolled, "Should return 0 enrolled templates");
    
    // Test slot occupancy
    bool occupied = finger.isSlotOccupied(1);
    TEST_ASSERT_FALSE_MESSAGE(occupied, "Slot 1 should be empty");
    
    // Test find empty slot
    uint16_t emptySlot = finger.findEmptySlot();
    TEST_ASSERT_EQUAL_MESSAGE(1, emptySlot, "Should find slot 1 as empty");
    
    // Test database stats
    UF_DatabaseStats stats;
    bool gotStats = finger.getDatabaseStats(stats);
    TEST_ASSERT_TRUE_MESSAGE(gotStats, "Should get database statistics");
    
    if (gotStats) {
        TEST_ASSERT_EQUAL_MESSAGE(TEST_CAPACITY, stats.totalSlots, "Total slots should match capacity");
        TEST_ASSERT_EQUAL_MESSAGE(0, stats.occupiedSlots, "Occupied slots should be 0");
        TEST_ASSERT_EQUAL_MESSAGE(TEST_CAPACITY, stats.freeSlots, "Free slots should match capacity");
        TEST_ASSERT_EQUAL_MESSAGE(1, stats.firstFreeSlot, "First free slot should be 1");
        TEST_ASSERT_EQUAL_MESSAGE(TEST_CAPACITY, stats.lastFreeSlot, "Last free slot should be capacity");
        TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0.0, stats.usagePercentage, "Usage should be 0%");
    }
}

// Test 4: Fingerprint Enrollment
void test_fingerprint_enrollment() {
    Serial.println("Test 4: Fingerprint Enrollment");
    
    finger.begin(TEST_PASSWORD, 57600);
    
    // Mock successful enrollment process
    AdafruitFingerprintMock::setGetImageResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setImage2TzResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setCreateModelResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setStoreModelResult(FINGERPRINT_OK);
    
    // Test enrollment with auto ID selection
    UF_ErrorCode result = finger.enroll();
    
    TEST_ASSERT_EQUAL_MESSAGE(UF_OK, result, "Enrollment should succeed");
    
    // Verify slot is now marked as occupied
    bool occupied = finger.isSlotOccupied(1);
    TEST_ASSERT_TRUE_MESSAGE(occupied, "Slot 1 should be occupied after enrollment");
    
    // Test enrollment with specific ID
    result = finger.enroll(5);
    TEST_ASSERT_EQUAL_MESSAGE(UF_OK, result, "Enrollment with specific ID should succeed");
}

// Test 5: Fingerprint Identification
void test_fingerprint_identification() {
    Serial.println("Test 5: Fingerprint Identification");
    
    finger.begin(TEST_PASSWORD, 57600);
    
    // Mock identification process
    AdafruitFingerprintMock::setGetImageResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setImage2TzResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setFingerFastSearchResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setFingerID(1);
    AdafruitFingerprintMock::setConfidence(85);
    
    uint16_t confidence = 0;
    int16_t id = finger.identify(&confidence);
    
    TEST_ASSERT_EQUAL_MESSAGE(1, id, "Should identify fingerprint with ID 1");
    TEST_ASSERT_EQUAL_MESSAGE(85, confidence, "Should return correct confidence");
    
    // Test not found scenario
    AdafruitFingerprintMock::setFingerFastSearchResult(FINGERPRINT_NOTFOUND);
    id = finger.identify();
    TEST_ASSERT_EQUAL_MESSAGE(0, id, "Should return 0 when fingerprint not found");
}

// Test 6: Fingerprint Verification
void test_fingerprint_verification() {
    Serial.println("Test 6: Fingerprint Verification");
    
    finger.begin(TEST_PASSWORD, 57600);
    
    // Mock verification process
    AdafruitFingerprintMock::setGetImageResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setImage2TzResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setFingerSearchResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setConfidence(90);
    
    uint16_t confidence = 0;
    UF_ErrorCode result = finger.verify(1, &confidence);
    
    TEST_ASSERT_EQUAL_MESSAGE(UF_OK, result, "Verification should succeed");
    TEST_ASSERT_EQUAL_MESSAGE(90, confidence, "Should return correct confidence");
}

// Test 7: Template Deletion
void test_template_deletion() {
    Serial.println("Test 7: Template Deletion");
    
    finger.begin(TEST_PASSWORD, 57600);
    
    // First enroll a template
    AdafruitFingerprintMock::setGetImageResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setImage2TzResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setCreateModelResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setStoreModelResult(FINGERPRINT_OK);
    finger.enroll(1);
    
    // Mock successful deletion
    AdafruitFingerprintMock::setDeleteModelResult(FINGERPRINT_OK);
    
    UF_ErrorCode result = finger.deleteTemplate(1);
    
    TEST_ASSERT_EQUAL_MESSAGE(UF_OK, result, "Deletion should succeed");
    
    // Verify slot is now empty
    bool occupied = finger.isSlotOccupied(1);
    TEST_ASSERT_FALSE_MESSAGE(occupied, "Slot 1 should be empty after deletion");
}

// Test 8: Database Clearing
void test_database_clearing() {
    Serial.println("Test 8: Database Clearing");
    
    finger.begin(TEST_PASSWORD, 57600);
    
    // Mock successful clear
    AdafruitFingerprintMock::setDeleteModelResult(FINGERPRINT_OK);
    
    UF_ErrorCode result = finger.clearDatabase();
    
    TEST_ASSERT_EQUAL_MESSAGE(UF_OK, result, "Database clear should succeed");
    
    // Verify no templates are enrolled
    uint16_t enrolled = finger.getEnrolledCount();
    TEST_ASSERT_EQUAL_MESSAGE(0, enrolled, "Should have 0 templates after clear");
}

// Test 9: LED Control
void test_led_control() {
    Serial.println("Test 9: LED Control");
    
    finger.begin(TEST_PASSWORD, 57600);
    
    // Mock successful LED control
    AdafruitFingerprintMock::setLEDcontrolResult(FINGERPRINT_OK);
    
    UF_ErrorCode result = finger.setLED(true, UF_LED_BLUE);
    TEST_ASSERT_EQUAL_MESSAGE(UF_OK, result, "Turning on LED should succeed");
    
    result = finger.setLED(false, UF_LED_BLUE);
    TEST_ASSERT_EQUAL_MESSAGE(UF_OK, result, "Turning off LED should succeed");
}

// Test 10: Security Level
void test_security_level() {
    Serial.println("Test 10: Security Level");
    
    finger.begin(TEST_PASSWORD, 57600);
    
    // Mock successful security level change
    AdafruitFingerprintMock::setSetSecurityLevelResult(FINGERPRINT_OK);
    
    UF_ErrorCode result = finger.setSecurity(3);
    TEST_ASSERT_EQUAL_MESSAGE(UF_OK, result, "Setting security level should succeed");
}

// Test 11: Error Handling
void test_error_handling() {
    Serial.println("Test 11: Error Handling");
    
    finger.begin(TEST_PASSWORD, 57600);
    
    // Test various error conditions
    
    // Communication error
    AdafruitFingerprintMock::setGetImageResult(FINGERPRINT_PACKETRECIEVEERR);
    int16_t id = finger.identify();
    TEST_ASSERT_LESS_THAN_MESSAGE(0, id, "Should return negative error code");
    
    // Convert error to string
    String errorStr = finger.errorToString(UF_ERROR_COMM);
    TEST_ASSERT_TRUE_MESSAGE(errorStr.length() > 0, "Should return error description");
    
    // Test invalid ID
    UF_ErrorCode result = finger.deleteTemplate(999);
    TEST_ASSERT_EQUAL_MESSAGE(UF_ERROR_INVALID_ID, result, "Should reject invalid ID");
}

// Test 12: Sensor Detection
void test_sensor_detection() {
    Serial.println("Test 12: Sensor Detection");
    
    // Test with auto-detection
    UniversalFingerprint autoFinger(&MockSerial, UF_SENSOR_AUTO);
    
    // Mock sensor as R307 (higher capacity)
    AdafruitFingerprintMock::setVerifyPasswordResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setBeginResult(true);
    // Simulate R307 by allowing higher slot access
    AdafruitFingerprintMock::setGetTemplateCountCustom([](uint16_t id) {
        return (id <= 1000) ? FINGERPRINT_OK : FINGERPRINT_BADLOCATION;
    });
    
    bool result = autoFinger.begin();
    TEST_ASSERT_TRUE_MESSAGE(result, "Should initialize with auto-detection");
    
    UF_SensorType detectedType = autoFinger.getSensorType();
    TEST_ASSERT_EQUAL_MESSAGE(UF_SENSOR_R307, detectedType, "Should auto-detect as R307");
    
    uint16_t capacity = autoFinger.getCapacity();
    TEST_ASSERT_EQUAL_MESSAGE(1000, capacity, "Should detect R307 capacity (1000)");
}

// Test 13: Performance Metrics
void test_performance_metrics() {
    Serial.println("Test 13: Performance Metrics");
    
    finger.begin(TEST_PASSWORD, 57600);
    
    // Mock fast responses for performance testing
    AdafruitFingerprintMock::setGetImageResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setGetTemplateCountResult(FINGERPRINT_OK);
    
    // Time database scan
    unsigned long startTime = millis();
    finger.scanDatabase();
    unsigned long endTime = millis();
    unsigned long scanTime = endTime - startTime;
    
    TEST_ASSERT_LESS_THAN_MESSAGE(5000, scanTime, "Database scan should complete in reasonable time");
    
    // Test multiple identifications
    AdafruitFingerprintMock::setImage2TzResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setFingerFastSearchResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setFingerID(1);
    
    startTime = millis();
    for (int i = 0; i < 10; i++) {
        finger.identify();
    }
    endTime = millis();
    unsigned long totalTime = endTime - startTime;
    
    TEST_ASSERT_LESS_THAN_MESSAGE(10000, totalTime, "10 identifications should complete in reasonable time");
}

// Test 14: Memory Management
void test_memory_management() {
    Serial.println("Test 14: Memory Management");
    
    // Test with different sensor capacities
    UniversalFingerprint smallSensor(&MockSerial, UF_SENSOR_AS608);
    UniversalFingerprint largeSensor(&MockSerial, UF_SENSOR_R307);
    
    smallSensor.begin();
    largeSensor.begin();
    
    // Get database stats to verify memory allocation
    UF_DatabaseStats smallStats, largeStats;
    smallSensor.getDatabaseStats(smallStats);
    largeSensor.getDatabaseStats(largeStats);
    
    TEST_ASSERT_EQUAL_MESSAGE(162, smallStats.totalSlots, "AS608 should have 162 slots");
    TEST_ASSERT_EQUAL_MESSAGE(1000, largeStats.totalSlots, "R307 should have 1000 slots");
}

// Test 15: Utility Functions
void test_utility_functions() {
    Serial.println("Test 15: Utility Functions");
    
    // Test sensor type to string
    String sensorStr = finger.sensorTypeToString(UF_SENSOR_AS608);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("AS608", sensorStr.c_str(), "Should convert sensor type to string");
    
    // Test LED color to string
    String colorStr = finger.ledColorToString(UF_LED_BLUE);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Blue", colorStr.c_str(), "Should convert LED color to string");
    
    // Test error to string for all error codes
    for (int i = 0; i <= UF_ERROR_INVALID_PARAM; i++) {
        String errorStr = finger.errorToString((UF_ErrorCode)i);
        TEST_ASSERT_TRUE_MESSAGE(errorStr.length() > 0, "Should return error description for all error codes");
    }
}

// Test 16: Multiple Operations Sequence
void test_multiple_operations_sequence() {
    Serial.println("Test 16: Multiple Operations Sequence");
    
    finger.begin(TEST_PASSWORD, 57600);
    
    // Mock all operations to succeed
    AdafruitFingerprintMock::setGetImageResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setImage2TzResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setCreateModelResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setStoreModelResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setFingerFastSearchResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setDeleteModelResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setLEDcontrolResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setSetSecurityLevelResult(FINGERPRINT_OK);
    
    // Perform sequence of operations
    UF_ErrorCode result;
    
    // 1. Set security level
    result = finger.setSecurity(3);
    TEST_ASSERT_EQUAL_MESSAGE(UF_OK, result, "Set security should succeed");
    
    // 2. Turn on LED
    result = finger.setLED(true, UF_LED_GREEN);
    TEST_ASSERT_EQUAL_MESSAGE(UF_OK, result, "Turn on LED should succeed");
    
    // 3. Enroll fingerprint
    result = finger.enroll(1);
    TEST_ASSERT_EQUAL_MESSAGE(UF_OK, result, "Enrollment should succeed");
    
    // 4. Identify fingerprint
    int16_t id = finger.identify();
    TEST_ASSERT_GREATER_THAN_MESSAGE(0, id, "Identification should succeed");
    
    // 5. Verify fingerprint
    uint16_t confidence;
    result = finger.verify(1, &confidence);
    TEST_ASSERT_EQUAL_MESSAGE(UF_OK, result, "Verification should succeed");
    
    // 6. Delete fingerprint
    result = finger.deleteTemplate(1);
    TEST_ASSERT_EQUAL_MESSAGE(UF_OK, result, "Deletion should succeed");
    
    // 7. Turn off LED
    result = finger.setLED(false, UF_LED_GREEN);
    TEST_ASSERT_EQUAL_MESSAGE(UF_OK, result, "Turn off LED should succeed");
}

// Test 17: Edge Cases
void test_edge_cases() {
    Serial.println("Test 17: Edge Cases");
    
    finger.begin(TEST_PASSWORD, 57600);
    
    // Test with invalid parameters
    
    // Invalid slot ID (0)
    bool occupied = finger.isSlotOccupied(0);
    TEST_ASSERT_FALSE_MESSAGE(occupied, "Slot 0 should be invalid");
    
    // Invalid slot ID (beyond capacity)
    occupied = finger.isSlotOccupied(1000);
    TEST_ASSERT_FALSE_MESSAGE(occupied, "Slot beyond capacity should be invalid");
    
    // Test empty database operations
    uint16_t emptySlot = finger.findEmptySlot();
    TEST_ASSERT_EQUAL_MESSAGE(1, emptySlot, "Should find first slot empty");
    
    // Test database stats on empty database
    UF_DatabaseStats stats;
    finger.getDatabaseStats(stats);
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0.0, stats.usagePercentage, "Empty database should have 0% usage");
}

// Test 18: Stress Test Simulation
void test_stress_test_simulation() {
    Serial.println("Test 18: Stress Test Simulation");
    
    finger.begin(TEST_PASSWORD, 57600);
    
    // Mock all operations to succeed for stress test
    AdafruitFingerprintMock::setGetImageResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setImage2TzResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setCreateModelResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setStoreModelResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setDeleteModelResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setGetTemplateCountResult(FINGERPRINT_OK);
    
    // Simulate multiple operations
    int operations = 0;
    int maxOperations = 50;
    
    for (int i = 0; i < maxOperations; i++) {
        // Alternate between different operations
        UF_ErrorCode result;
        
        switch (i % 5) {
            case 0:
                // LED control
                result = finger.setLED((i % 2) == 0, UF_LED_BLUE);
                TEST_ASSERT_EQUAL_MESSAGE(UF_OK, result, "LED control should succeed");
                operations++;
                break;
                
            case 1:
                // Database scan
                int16_t count = finger.scanDatabase();
                TEST_ASSERT_GREATER_OR_EQUAL_MESSAGE(0, count, "Database scan should succeed");
                operations++;
                break;
                
            case 2:
                // Check slot
                bool occupied = finger.isSlotOccupied(1);
                // Result depends on mock state, just verify no crash
                operations++;
                break;
                
            case 3:
                // Find empty slot
                uint16_t slot = finger.findEmptySlot();
                TEST_ASSERT_GREATER_OR_EQUAL_MESSAGE(1, slot, "Should find valid slot");
                operations++;
                break;
                
            case 4:
                // Get stats
                UF_DatabaseStats stats;
                bool gotStats = finger.getDatabaseStats(stats);
                TEST_ASSERT_TRUE_MESSAGE(gotStats, "Should get stats");
                operations++;
                break;
        }
    }
    
    TEST_ASSERT_EQUAL_MESSAGE(maxOperations, operations, "Should complete all operations");
}

// Test 19: Recovery from Errors
void test_recovery_from_errors() {
    Serial.println("Test 19: Recovery from Errors");
    
    finger.begin(TEST_PASSWORD, 57600);
    
    // Simulate temporary communication failure
    AdafruitFingerprintMock::setGetImageResult(FINGERPRINT_PACKETRECIEVEERR);
    
    // First operation fails
    int16_t id = finger.identify();
    TEST_ASSERT_LESS_THAN_MESSAGE(0, id, "Should fail with communication error");
    
    // Recovery: sensor responds again
    AdafruitFingerprintMock::setGetImageResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setImage2TzResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setFingerFastSearchResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setFingerID(1);
    
    // Subsequent operation should succeed
    id = finger.identify();
    TEST_ASSERT_EQUAL_MESSAGE(1, id, "Should recover and succeed after error");
}

// Test 20: Integration Test
void test_integration_test() {
    Serial.println("Test 20: Integration Test");
    
    // This test simulates a complete use case
    
    finger.begin(TEST_PASSWORD, 57600);
    
    // Mock all operations
    AdafruitFingerprintMock::setGetImageResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setImage2TzResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setCreateModelResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setStoreModelResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setFingerFastSearchResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setDeleteModelResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setLEDcontrolResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setSetSecurityLevelResult(FINGERPRINT_OK);
    AdafruitFingerprintMock::setGetTemplateCountResult(FINGERPRINT_OK);
    
    // Scenario: Access Control System
    
    // 1. Initialize system
    finger.setSecurity(4);  // High security
    finger.setLED(true, UF_LED_BLUE);  // System ready indicator
    
    // 2. Admin enrolls users
    UF_ErrorCode result;
    uint16_t enrolled = 0;
    
    for (uint16_t id = 1; id <= 5; id++) {
        result = finger.enroll(id);
        if (result == UF_OK) {
            enrolled++;
            Serial.print("Enrolled user #");
            Serial.println(id);
        }
    }
    
    TEST_ASSERT_EQUAL_MESSAGE(5, enrolled, "Should enroll 5 users");
    
    // 3. System operation
    int successfulAccess = 0;
    int failedAccess = 0;
    
    // Simulate access attempts
    for (int attempt = 0; attempt < 10; attempt++) {
        AdafruitFingerprintMock::setFingerID((attempt % 5) + 1);  // Cycle through users
        
        int16_t userId = finger.identify();
        
        if (userId > 0) {
            successfulAccess++;
            Serial.print("Access granted to user #");
            Serial.println(userId);
            
            // Verify the user
            uint16_t confidence;
            result = finger.verify(userId, &confidence);
            TEST_ASSERT_EQUAL_MESSAGE(UF_OK, result, "Verification should succeed");
            
        } else {
            failedAccess++;
            Serial.println("Access denied");
        }
    }
    
    TEST_ASSERT_GREATER_THAN_MESSAGE(0, successfulAccess, "Should have some successful access");
    
    // 4. Maintenance: Remove user 3
    result = finger.deleteTemplate(3);
    TEST_ASSERT_EQUAL_MESSAGE(UF_OK, result, "Should delete user 3");
    
    // 5. System shutdown
    finger.setLED(false, UF_LED_BLUE);
    
    // Final check
    UF_DatabaseStats stats;
    finger.getDatabaseStats(stats);
    TEST_ASSERT_EQUAL_MESSAGE(4, stats.occupiedSlots, "Should have 4 users remaining");
}

void setup() {
    // Wait for serial connection
    delay(2000);
    
    UNITY_BEGIN();
    
    // Run tests
    RUN_TEST(test_library_initialization);
    RUN_TEST(test_sensor_information);
    RUN_TEST(test_database_management);
    RUN_TEST(test_fingerprint_enrollment);
    RUN_TEST(test_fingerprint_identification);
    RUN_TEST(test_fingerprint_verification);
    RUN_TEST(test_template_deletion);
    RUN_TEST(test_database_clearing);
    RUN_TEST(test_led_control);
    RUN_TEST(test_security_level);
    RUN_TEST(test_error_handling);
    RUN_TEST(test_sensor_detection);
    RUN_TEST(test_performance_metrics);
    RUN_TEST(test_memory_management);
    RUN_TEST(test_utility_functions);
    RUN_TEST(test_multiple_operations_sequence);
    RUN_TEST(test_edge_cases);
    RUN_TEST(test_stress_test_simulation);
    RUN_TEST(test_recovery_from_errors);
    RUN_TEST(test_integration_test);
    
    UNITY_END();
}

void loop() {
    // Empty for testing
}