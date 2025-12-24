/*
  SensorDetector.ino
  UniversalFingerprint Library - Sensor Detection Example
  
  Demonstrates sensor auto-detection and information retrieval.
  Can test multiple baud rates and sensor types.
  
  Connections:
  - Fingerprint Sensor TX -> Arduino D2
  - Fingerprint Sensor RX -> Arduino D3
  - Fingerprint Sensor VCC -> 3.3V or 5V
  - Fingerprint Sensor GND -> GND
  
  Created: 2024
  Version: 1.0.0
*/

#include <UniversalFingerprint.h>
#include <SoftwareSerial.h>

// Create software serial for fingerprint sensor
SoftwareSerial fingerSerial(2, 3); // RX, TX

// Create fingerprint object with auto-detection
UniversalFingerprint finger(&fingerSerial, UF_SENSOR_AUTO);

void setup() {
    Serial.begin(115200);
    while (!Serial); // Wait for serial port
    
    Serial.println(F("\n=== UniversalFingerprint Sensor Detector ==="));
    Serial.println(F("Library Version: ") + finger.getVersion());
    Serial.println(F("This example demonstrates sensor auto-detection"));
    Serial.println(F("and information retrieval."));
    Serial.println(F("=============================================\n"));
    
    Serial.println(F("Starting sensor detection process..."));
    Serial.println(F("Trying different baud rates and detection methods."));
    Serial.println();
    
    // Try to detect sensor with different methods
    detectSensor();
    
    Serial.println(F("\nDetection process completed."));
    Serial.println(F("Type 'h' for available commands."));
    printMenu();
}

void loop() {
    if (Serial.available()) {
        char command = Serial.read();
        
        switch (command) {
            case '1': detectSensor(); break;
            case '2': testBaudRates(); break;
            case '3': testCapacities(); break;
            case '4': sensorDetailedInfo(); break;
            case '5': communicationTest(); break;
            case '6': performanceTest(); break;
            case '7': testLEDs(); break;
            case '8': testSecurityLevels(); break;
            case '9': testAllFeatures(); break;
            case 's': finger.scanDatabase(); break;
            case 'i': sensorInfo(); break;
            case 'h': printMenu(); break;
            case 'r': resetAndDetect(); break;
            case 't': stressTest(); break;
            case '\n': case '\r': break;
            default:
                Serial.println(F("Unknown command. Type 'h' for help."));
                break;
        }
    }
}

void printMenu() {
    Serial.println(F("\n=== SENSOR DETECTOR MENU ==="));
    Serial.println(F("1) Detect sensor"));
    Serial.println(F("2) Test baud rates"));
    Serial.println(F("3) Test capacities"));
    Serial.println(F("4) Detailed sensor info"));
    Serial.println(F("5) Communication test"));
    Serial.println(F("6) Performance test"));
    Serial.println(F("7) Test LEDs"));
    Serial.println(F("8) Test security levels"));
    Serial.println(F("9) Test all features"));
    Serial.println(F("s) Scan database"));
    Serial.println(F("i) Basic sensor info"));
    Serial.println(F("r) Reset and detect"));
    Serial.println(F("t) Stress test"));
    Serial.println(F("h) Show this menu"));
    Serial.println(F("============================"));
}

void detectSensor() {
    Serial.println(F("\n--- SENSOR DETECTION ---"));
    Serial.println(F("Starting auto-detection process..."));
    
    // List of baud rates to try
    uint32_t baudRates[] = {57600, 9600, 115200, 38400, 19200, 14400};
    const char* baudNames[] = {"57600", "9600", "115200", "38400", "19200", "14400"};
    uint8_t numBaudRates = 6;
    
    bool sensorFound = false;
    
    for (uint8_t i = 0; i < numBaudRates; i++) {
        Serial.print(F("Trying baud rate: "));
        Serial.print(baudNames[i]);
        Serial.print(F("... "));
        
        // Re-initialize with different baud rate
        if (finger.begin(0x00000000, baudRates[i])) {
            sensorFound = true;
            Serial.println(F("SUCCESS"));
            break;
        } else {
            Serial.println(F("FAILED"));
        }
        
        delay(200); // Brief pause between attempts
    }
    
    if (!sensorFound) {
        Serial.println(F("\nERROR: Could not detect fingerprint sensor!"));
        Serial.println(F("Possible issues:"));
        Serial.println(F("1. Incorrect wiring (TX/RX swapped?)"));
        Serial.println(F("2. Wrong voltage (3.3V vs 5V)"));
        Serial.println(F("3. Sensor not powered"));
        Serial.println(F("4. Sensor malfunction"));
        return;
    }
    
    // Get sensor information
    sensorInfo();
    
    // Try to determine sensor capacity
    testCapacities();
    
    // Test communication
    communicationTest();
}

void sensorInfo() {
    Serial.println(F("\n--- SENSOR INFORMATION ---"));
    
    UF_SensorInfo info;
    if (finger.getSensorInfo(info)) {
        Serial.print(F("Model: "));
        Serial.println(info.modelName);
        
        Serial.print(F("Vendor: "));
        Serial.println(info.vendor);
        
        Serial.print(F("Type: "));
        Serial.println(finger.sensorTypeToString(finger.getSensorType()));
        
        Serial.print(F("Capacity: "));
        Serial.print(info.capacity);
        Serial.println(F(" templates"));
        
        Serial.print(F("Default baud rate: "));
        Serial.println(info.defaultBaud);
        
        Serial.print(F("Packet size: "));
        Serial.println(info.packetSize);
        
        Serial.print(F("Has LED control: "));
        Serial.println(info.hasLED ? "Yes" : "No");
        
        Serial.print(F("Has touch sensor: "));
        Serial.println(info.hasTouchSensor ? "Yes" : "No");
        
        // Library version
        Serial.print(F("Library version: "));
        Serial.println(finger.getVersion());
        
        // Additional calculated info
        Serial.print(F("Database slots: "));
        Serial.print(finger.getCapacity());
        Serial.println(F(" (detected)"));
        
        // Check if sensor is responding
        Serial.print(F("Sensor responding: "));
        Serial.println(finger.isFingerPresent() ? "Yes" : "No (or no finger)"));
    } else {
        Serial.println(F("ERROR: Could not get sensor information"));
    }
}

void testBaudRates() {
    Serial.println(F("\n--- BAUD RATE TEST ---"));
    Serial.println(F("Testing communication at different baud rates..."));
    
    uint32_t baudRates[] = {9600, 14400, 19200, 38400, 57600, 115200, 128000, 256000};
    const char* baudNames[] = {"9600", "14400", "19200", "38400", "57600", "115200", "128000", "256000"};
    uint8_t numRates = 8;
    
    Serial.println(F("Baud Rate\tStatus\t\tTime (ms)"));
    Serial.println(F("--------------------------------------"));
    
    for (uint8_t i = 0; i < numRates; i++) {
        Serial.print(baudNames[i]);
        Serial.print(F("\t\t"));
        
        unsigned long startTime = millis();
        
        // Try to initialize at this baud rate
        if (finger.begin(0x00000000, baudRates[i])) {
            unsigned long endTime = millis();
            Serial.print(F("OK\t\t"));
            Serial.println(endTime - startTime);
            
            // Quick test to verify communication works
            if (finger.isFingerPresent()) {
                Serial.println(F("\t  Finger detection works"));
            }
        } else {
            Serial.println(F("FAILED\t\t--"));
        }
        
        delay(100); // Brief pause
    }
    
    // Return to default baud rate
    finger.begin();
    Serial.println(F("\nReturned to default baud rate."));
}

void testCapacities() {
    Serial.println(F("\n--- CAPACITY TEST ---"));
    Serial.println(F("Testing different capacity boundaries..."));
    
    // Known sensor capacities
    struct CapacityTest {
        uint16_t slot;
        const char* sensorType;
        bool expected;
    };
    
    CapacityTest tests[] = {
        {162, "AS608", true},
        {200, "GT-511C3", true},
        {256, "ZFM-20", true},
        {300, "ZFM-60", true},
        {1000, "R307", true},
        {1500, "Unknown", false}
    };
    
    uint8_t numTests = 6;
    uint16_t detectedCapacity = 0;
    
    Serial.println(F("Slot\tExpected\tTest\tResult"));
    Serial.println(F("--------------------------------------"));
    
    for (uint8_t i = 0; i < numTests; i++) {
        Serial.print(tests[i].slot);
        Serial.print(F("\t"));
        Serial.print(tests[i].sensorType);
        Serial.print(F("\t\t"));
        
        // Try to access the slot
        // Note: This is a simplified test - actual capacity detection
        // would involve more sophisticated methods
        
        // For demonstration, we'll check if we can scan up to this slot
        bool canAccess = (tests[i].slot <= finger.getCapacity());
        
        Serial.print(canAccess ? "OK" : "N/A");
        Serial.print(F("\t"));
        Serial.println(canAccess == tests[i].expected ? "PASS" : "FAIL");
        
        if (canAccess && tests[i].slot > detectedCapacity) {
            detectedCapacity = tests[i].slot;
        }
    }
    
    Serial.print(F("\nDetected capacity: "));
    Serial.print(finger.getCapacity());
    Serial.println(F(" templates"));
    
    Serial.print(F("Maximum testable: "));
    Serial.print(detectedCapacity);
    Serial.println(F(" templates"));
}

void sensorDetailedInfo() {
    Serial.println(F("\n--- DETAILED SENSOR INFO ---"));
    
    // Test multiple parameters
    Serial.println(F("Testing sensor parameters..."));
    
    // 1. Check if sensor responds
    Serial.print(F("1. Sensor response: "));
    if (finger.isFingerPresent()) {
        Serial.println(F("OK (finger detected or sensor ready)"));
    } else {
        // Try a different method
        Serial.println(F("Testing alternative method..."));
        delay(100);
        if (finger.setLED(true, UF_LED_BLUE) == UF_OK) {
            Serial.println(F("OK (LED control works)"));
            delay(500);
            finger.setLED(false, UF_LED_BLUE);
        } else {
            Serial.println(F("FAILED - Sensor not responding"));
        }
    }
    
    // 2. Test LED functionality
    Serial.print(F("2. LED functionality: "));
    UF_ErrorCode ledResult = finger.setLED(true, UF_LED_RED);
    if (ledResult == UF_OK) {
        Serial.println(F("OK"));
        delay(300);
        finger.setLED(false, UF_LED_RED);
    } else {
        Serial.print(F("Limited or not available ("));
        Serial.print(finger.errorToString(ledResult));
        Serial.println(F(")"));
    }
    
    // 3. Test database operations
    Serial.print(F("3. Database operations: "));
    int16_t scanResult = finger.scanDatabase();
    if (scanResult >= 0) {
        Serial.print(F("OK - Found "));
        Serial.print(scanResult);
        Serial.println(F(" templates"));
    } else {
        Serial.print(F("Failed - "));
        Serial.println(finger.errorToString((UF_ErrorCode)(-scanResult)));
    }
    
    // 4. Test template operations
    Serial.print(F("4. Template operations: "));
    uint16_t emptySlot = finger.findEmptySlot();
    if (emptySlot > 0) {
        Serial.print(F("OK - Empty slot found at #"));
        Serial.println(emptySlot);
    } else {
        UF_DatabaseStats stats;
        if (finger.getDatabaseStats(stats)) {
            if (stats.occupiedSlots == stats.totalSlots) {
                Serial.println(F("OK - Database full"));
            } else {
                Serial.println(F("Failed to find empty slot"));
            }
        } else {
            Serial.println(F("Failed to get database stats"));
        }
    }
    
    // 5. Test security levels
    Serial.print(F("5. Security level setting: "));
    UF_ErrorCode securityResult = finger.setSecurity(3);
    if (securityResult == UF_OK) {
        Serial.println(F("OK"));
    } else {
        Serial.print(F("Limited - "));
        Serial.println(finger.errorToString(securityResult));
    }
    
    // 6. Test baud rate change
    Serial.print(F("6. Baud rate change: "));
    UF_ErrorCode baudResult = finger.setBaudRate(57600);
    if (baudResult == UF_OK) {
        Serial.println(F("OK"));
    } else {
        Serial.print(F("Not supported - "));
        Serial.println(finger.errorToString(baudResult));
    }
    
    Serial.println(F("\nDetailed testing completed."));
}

void communicationTest() {
    Serial.println(F("\n--- COMMUNICATION TEST ---"));
    Serial.println(F("Testing sensor communication reliability..."));
    
    const uint8_t TEST_COUNT = 10;
    uint8_t successes = 0;
    uint8_t failures = 0;
    unsigned long totalTime = 0;
    
    Serial.println(F("Running ") + String(TEST_COUNT) + F(" communication tests:"));
    
    for (uint8_t i = 1; i <= TEST_COUNT; i++) {
        Serial.print(F("Test "));
        Serial.print(i);
        Serial.print(F(": "));
        
        unsigned long startTime = millis();
        
        // Try a simple operation
        UF_ErrorCode result = finger.setLED((i % 2) == 0, UF_LED_BLUE);
        
        unsigned long endTime = millis();
        unsigned long duration = endTime - startTime;
        
        if (result == UF_OK) {
            Serial.print(F("OK ("));
            Serial.print(duration);
            Serial.println(F(" ms)"));
            successes++;
            totalTime += duration;
        } else {
            Serial.print(F("FAILED - "));
            Serial.println(finger.errorToString(result));
            failures++;
        }
        
        delay(100); // Brief pause
    }
    
    // Turn off LED
    finger.setLED(false, UF_LED_BLUE);
    
    // Calculate statistics
    Serial.println(F("\n--- TEST RESULTS ---"));
    Serial.print(F("Successes: "));
    Serial.println(successes);
    
    Serial.print(F("Failures: "));
    Serial.println(failures);
    
    Serial.print(F("Success rate: "));
    Serial.print((successes * 100.0) / TEST_COUNT, 1);
    Serial.println(F("%"));
    
    if (successes > 0) {
        Serial.print(F("Average response time: "));
        Serial.print(totalTime / successes);
        Serial.println(F(" ms"));
    }
    
    // Reliability assessment
    Serial.print(F("\nReliability assessment: "));
    if (successes == TEST_COUNT) {
        Serial.println(F("EXCELLENT - All tests passed"));
    } else if (successes >= TEST_COUNT * 0.8) {
        Serial.println(F("GOOD - Most tests passed"));
    } else if (successes >= TEST_COUNT * 0.5) {
        Serial.println(F("FAIR - Some communication issues"));
    } else {
        Serial.println(F("POOR - Significant communication problems"));
        Serial.println(F("Check wiring, power supply, and baud rate."));
    }
}

void performanceTest() {
    Serial.println(F("\n--- PERFORMANCE TEST ---"));
    Serial.println(F("Testing sensor performance metrics..."));
    
    // Test 1: Response time
    Serial.println(F("1. Response Time Test:"));
    
    const uint8_t RESPONSE_TESTS = 5;
    unsigned long responseTimes[RESPONSE_TESTS];
    
    for (uint8_t i = 0; i < RESPONSE_TESTS; i++) {
        unsigned long startTime = micros();
        finger.isFingerPresent();
        unsigned long endTime = micros();
        responseTimes[i] = endTime - startTime;
        
        Serial.print(F("  Test "));
        Serial.print(i + 1);
        Serial.print(F(": "));
        Serial.print(responseTimes[i]);
        Serial.println(F(" µs"));
        delay(50);
    }
    
    // Calculate average
    unsigned long total = 0;
    for (uint8_t i = 0; i < RESPONSE_TESTS; i++) {
        total += responseTimes[i];
    }
    Serial.print(F("  Average: "));
    Serial.print(total / RESPONSE_TESTS);
    Serial.println(F(" µs"));
    
    // Test 2: Database scan speed
    Serial.println(F("\n2. Database Scan Speed:"));
    
    UF_DatabaseStats stats;
    if (finger.getDatabaseStats(stats)) {
        Serial.print(F("  Total slots: "));
        Serial.println(stats.totalSlots);
        
        unsigned long startTime = millis();
        finger.scanDatabase();
        unsigned long endTime = millis();
        
        Serial.print(F("  Scan time: "));
        Serial.print(endTime - startTime);
        Serial.println(F(" ms"));
        
        Serial.print(F("  Time per slot: "));
        Serial.print((endTime - startTime) * 1000.0 / stats.totalSlots, 1);
        Serial.println(F(" µs"));
    }
    
    // Test 3: Template operations speed
    Serial.println(F("\n3. Template Operation Speed:"));
    
    uint16_t emptySlot = finger.findEmptySlot();
    if (emptySlot > 0) {
        Serial.print(F("  Testing with empty slot #"));
        Serial.println(emptySlot);
        
        // Check slot status speed
        unsigned long startTime = micros();
        finger.isSlotOccupied(emptySlot);
        unsigned long endTime = micros();
        
        Serial.print(F("  Slot check: "));
        Serial.print(endTime - startTime);
        Serial.println(F(" µs"));
    }
    
    // Test 4: Communication throughput
    Serial.println(F("\n4. Communication Throughput:"));
    Serial.println(F("  (This would test data transfer rates)"));
    Serial.println(F("  Note: Throughput depends on baud rate and sensor."));
    
    // Summary
    Serial.println(F("\n--- PERFORMANCE SUMMARY ---"));
    Serial.println(F("Compare these results with sensor specifications."));
    Serial.println(F("Slower than expected performance may indicate:"));
    Serial.println(F("1. Low baud rate"));
    Serial.println(F("2. Poor connection quality"));
    Serial.println(F("3. Sensor limitations"));
    Serial.println(F("4. Interference or noise"));
}

void testLEDs() {
    Serial.println(F("\n--- LED TEST ---"));
    Serial.println(F("Testing all available LED colors..."));
    
    UF_LEDColor colors[] = {
        UF_LED_RED, UF_LED_BLUE, UF_LED_PURPLE,
        UF_LED_GREEN, UF_LED_CYAN, UF_LED_YELLOW, UF_LED_WHITE
    };
    
    const char* colorNames[] = {
        "Red", "Blue", "Purple", "Green", "Cyan", "Yellow", "White"
    };
    
    uint8_t numColors = 7;
    
    for (uint8_t i = 0; i < numColors; i++) {
        Serial.print(F("Testing "));
        Serial.print(colorNames[i]);
        Serial.print(F(" LED... "));
        
        UF_ErrorCode result = finger.setLED(true, colors[i]);
        
        if (result == UF_OK) {
            Serial.println(F("OK"));
            delay(500); // Keep LED on for 500ms
            finger.setLED(false, colors[i]);
            delay(200); // Brief pause
        } else {
            Serial.print(F("FAILED - "));
            Serial.println(finger.errorToString(result));
        }
    }
    
    // Test blinking
    Serial.println(F("\nTesting LED blinking..."));
    for (uint8_t speed = 1; speed <= 5; speed++) {
        Serial.print(F("Blink speed "));
        Serial.print(speed);
        Serial.print(F("... "));
        
        if (finger.setLED(true, UF_LED_BLUE, speed) == UF_OK) {
            Serial.println(F("OK"));
            delay(1000);
            finger.setLED(false, UF_LED_BLUE);
            delay(200);
        } else {
            Serial.println(F("FAILED"));
        }
    }
    
    // Final test: Color cycle
    Serial.println(F("\nLED color cycle test:"));
    for (uint8_t i = 0; i < numColors; i++) {
        finger.setLED(true, colors[i]);
        delay(300);
        finger.setLED(false, colors[i]);
    }
    
    Serial.println(F("\nLED test completed."));
}

void testSecurityLevels() {
    Serial.println(F("\n--- SECURITY LEVEL TEST ---"));
    Serial.println(F("Testing different security levels..."));
    
    // Note: Higher security levels = more accurate but slower
    for (uint8_t level = 1; level <= 5; level++) {
        Serial.print(F("Setting security level to "));
        Serial.print(level);
        Serial.print(F("... "));
        
        UF_ErrorCode result = finger.setSecurity(level);
        
        if (result == UF_OK) {
            Serial.println(F("OK"));
            
            // Test identification with this security level
            Serial.print(F("  Testing identification... "));
            unsigned long startTime = millis();
            int16_t id = finger.identify();
            unsigned long endTime = millis();
            
            if (id > 0) {
                Serial.print(F("Found ID #"));
                Serial.print(id);
            } else if (id == 0) {
                Serial.print(F("No match"));
            } else {
                Serial.print(F("Error: "));
                Serial.print(finger.errorToString((UF_ErrorCode)(-id)));
            }
            
            Serial.print(F(" ("));
            Serial.print(endTime - startTime);
            Serial.println(F(" ms)"));
        } else {
            Serial.print(F("FAILED - "));
            Serial.println(finger.errorToString(result));
        }
        
        delay(1000); // Pause between tests
    }
    
    // Return to default level (3)
    finger.setSecurity(3);
    Serial.println(F("\nReturned to default security level (3)."));
}

void testAllFeatures() {
    Serial.println(F("\n=== COMPREHENSIVE FEATURE TEST ==="));
    
    // Run all tests
    detectSensor();
    sensorDetailedInfo();
    communicationTest();
    testLEDs();
    testSecurityLevels();
    performanceTest();
    
    Serial.println(F("\n=== FEATURE TEST COMPLETE ==="));
    Serial.println(F("All available features have been tested."));
}

void resetAndDetect() {
    Serial.println(F("\n--- RESET AND RE-DETECT ---"));
    Serial.println(F("Resetting sensor connection and re-detecting..."));
    
    // Simulate a reset by re-initializing
    if (finger.begin()) {
        Serial.println(F("Re-initialization successful"));
        sensorInfo();
    } else {
        Serial.println(F("Re-initialization failed"));
        Serial.println(F("Trying with auto-baud detection..."));
        detectSensor();
    }
}

void stressTest() {
    Serial.println(F("\n--- STRESS TEST ---"));
    Serial.println(F("WARNING: This test may take several minutes!"));
    Serial.println(F("It performs multiple operations to test reliability."));
    Serial.print(F("Continue? (y/n): "));
    
    while (!Serial.available()) { delay(10); }
    char confirm = Serial.read();
    
    // Clear serial buffer
    while (Serial.available()) { Serial.read(); }
    
    if (confirm != 'y' && confirm != 'Y') {
        Serial.println(F("Test cancelled"));
        return;
    }
    
    const uint16_t OPERATION_COUNT = 50;
    uint16_t successes = 0;
    uint16_t failures = 0;
    
    Serial.println(F("\nStarting stress test with ") + String(OPERATION_COUNT) + F(" operations:"));
    
    for (uint16_t i = 1; i <= OPERATION_COUNT; i++) {
        Serial.print(F("Operation "));
        Serial.print(i);
        Serial.print(F(": "));
        
        // Alternate between different operations
        UF_ErrorCode result;
        switch (i % 5) {
            case 0:
                result = finger.setLED((i % 2) == 0, UF_LED_BLUE);
                break;
            case 1:
                result = finger.isFingerPresent() ? UF_OK : UF_ERROR_NO_FINGER;
                break;
            case 2:
                result = (finger.scanDatabase() >= 0) ? UF_OK : UF_ERROR_COMM;
                break;
            case 3:
                result = finger.setSecurity(2 + (i % 3));
                break;
            case 4:
                uint16_t emptySlot = finger.findEmptySlot();
                result = (emptySlot > 0) ? UF_OK : UF_ERROR_SLOT_FULL;
                break;
        }
        
        if (result == UF_OK) {
            Serial.println(F("OK"));
            successes++;
        } else {
            Serial.print(F("FAILED - "));
            Serial.println(finger.errorToString(result));
            failures++;
        }
        
        // Progress indicator
        if (i % 10 == 0) {
            Serial.print(F("Progress: "));
            Serial.print((i * 100) / OPERATION_COUNT);
            Serial.println(F("%"));
        }
        
        delay(100); // Brief pause between operations
    }
    
    // Turn off LED
    finger.setLED(false, UF_LED_BLUE);
    
    // Results
    Serial.println(F("\n--- STRESS TEST RESULTS ---"));
    Serial.print(F("Total operations: "));
    Serial.println(OPERATION_COUNT);
    Serial.print(F("Successes: "));
    Serial.println(successes);
    Serial.print(F("Failures: "));
    Serial.println(failures);
    Serial.print(F("Success rate: "));
    Serial.print((successes * 100.0) / OPERATION_COUNT, 1);
    Serial.println(F("%"));
    
    // Reliability rating
    Serial.print(F("\nReliability rating: "));
    float successRate = (successes * 100.0) / OPERATION_COUNT;
    
    if (successRate >= 95) {
        Serial.println(F("EXCELLENT"));
    } else if (successRate >= 90) {
        Serial.println(F("VERY GOOD"));
    } else if (successRate >= 80) {
        Serial.println(F("GOOD"));
    } else if (successRate >= 70) {
        Serial.println(F("FAIR"));
    } else {
        Serial.println(F("POOR - Check sensor and connections"));
    }
    
    Serial.println(F("\nStress test completed."));
}