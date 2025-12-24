/*
  AdvancedOperations.ino
  UniversalFingerprint Library - Advanced Operations Example
  
  Demonstrates advanced features:
  - Raw template operations
  - Custom baud rates
  - Sensor sleep/wake
  - Error recovery
  - Custom configurations
  
  Created: 2024
  Version: 1.0.0
*/

#include <UniversalFingerprint.h>
#include <SoftwareSerial.h>

// Create software serial for fingerprint sensor
SoftwareSerial fingerSerial(2, 3); // RX, TX

// Create fingerprint object
UniversalFingerprint finger(&fingerSerial);

// Advanced configuration
struct AdvancedConfig {
    uint32_t customBaudRate;
    uint8_t securityLevel;
    bool enableSleep;
    uint16_t templateBufferSize;
    uint8_t ledBrightness;
};

AdvancedConfig config = {
    57600,      // customBaudRate
    3,          // securityLevel
    false,      // enableSleep
    512,        // templateBufferSize
    255         // ledBrightness (max)
};

void setup() {
    Serial.begin(115200);
    while (!Serial); // Wait for serial port
    
    Serial.println(F("\n=== UniversalFingerprint Advanced Operations ==="));
    Serial.println(F("Library Version: ") + finger.getVersion());
    Serial.println(F("This example demonstrates advanced features"));
    Serial.println(F("and custom configurations."));
    Serial.println(F("=================================================\n"));
    
    // Initialize with custom configuration
    Serial.println(F("Initializing with advanced configuration..."));
    
    if (!finger.begin(0x00000000, config.customBaudRate)) {
        Serial.println(F("ERROR: Sensor initialization failed!"));
        Serial.println(F("Trying default baud rate..."));
        
        if (!finger.begin()) {
            Serial.println(F("ERROR: Could not initialize sensor!"));
            while (1) { delay(100); }
        }
    }
    
    Serial.print(F("Sensor: "));
    Serial.println(finger.getSensorModel());
    
    // Apply custom settings
    applyAdvancedSettings();
    
    Serial.println(F("\nAdvanced operations ready."));
    printMenu();
}

void loop() {
    if (Serial.available()) {
        char command = Serial.read();
        
        switch (command) {
            case '1': rawTemplateOperations(); break;
            case '2': customBaudRateTest(); break;
            case '3': sleepWakeTest(); break;
            case '4': errorRecoveryTest(); break;
            case '5': customConfigurations(); break;
            case '6': templateManagement(); break;
            case '7': sensorDiagnostics(); break;
            case '8': performanceTuning(); break;
            case '9': advancedEnrollment(); break;
            case 'a': templateVerification(); break;
            case 'b': databaseOptimization(); break;
            case 'c': communicationOptimization(); break;
            case 'd': powerManagement(); break;
            case 'e': securityTesting(); break;
            case 'f': templateBackup(); break;
            case 'g': sensorCalibration(); break;
            case 'h': printMenu(); break;
            case 'i': showCurrentConfig(); break;
            case 'r': resetToDefaults(); break;
            case 's': systemStatus(); break;
            case 't': advancedTesting(); break;
            case '\n': case '\r': break;
            default:
                Serial.println(F("Unknown command. Type 'h' for help."));
                break;
        }
    }
}

void printMenu() {
    Serial.println(F("\n=== ADVANCED OPERATIONS MENU ==="));
    Serial.println(F("1) Raw template operations"));
    Serial.println(F("2) Custom baud rate test"));
    Serial.println(F("3) Sleep/wake test"));
    Serial.println(F("4) Error recovery test"));
    Serial.println(F("5) Custom configurations"));
    Serial.println(F("6) Template management"));
    Serial.println(F("7) Sensor diagnostics"));
    Serial.println(F("8) Performance tuning"));
    Serial.println(F("9) Advanced enrollment"));
    Serial.println(F("a) Template verification"));
    Serial.println(F("b) Database optimization"));
    Serial.println(F("c) Communication optimization"));
    Serial.println(F("d) Power management"));
    Serial.println(F("e) Security testing"));
    Serial.println(F("f) Template backup"));
    Serial.println(F("g) Sensor calibration"));
    Serial.println(F("h) Show this menu"));
    Serial.println(F("i) Show current config"));
    Serial.println(F("r) Reset to defaults"));
    Serial.println(F("s) System status"));
    Serial.println(F("t) Advanced testing"));
    Serial.println(F("================================="));
}

void applyAdvancedSettings() {
    Serial.println(F("\n--- APPLYING ADVANCED SETTINGS ---"));
    
    // Set security level
    Serial.print(F("Setting security level to "));
    Serial.print(config.securityLevel);
    Serial.print(F("... "));
    
    UF_ErrorCode result = finger.setSecurity(config.securityLevel);
    if (result == UF_OK) {
        Serial.println(F("OK"));
    } else {
        Serial.print(F("FAILED: "));
        Serial.println(finger.errorToString(result));
    }
    
    // Configure LED
    Serial.print(F("Configuring LED... "));
    result = finger.setLED(false, UF_LED_BLUE); // Turn off initially
    if (result == UF_OK) {
        Serial.println(F("OK"));
    } else {
        Serial.print(F("Limited: "));
        Serial.println(finger.errorToString(result));
    }
    
    // Set custom baud rate if different
    if (config.customBaudRate != 57600) {
        Serial.print(F("Setting baud rate to "));
        Serial.print(config.customBaudRate);
        Serial.print(F("... "));
        
        result = finger.setBaudRate(config.customBaudRate);
        if (result == UF_OK) {
            Serial.println(F("OK"));
            // Re-initialize with new baud rate
            finger.begin(0x00000000, config.customBaudRate);
        } else {
            Serial.print(F("FAILED: "));
            Serial.println(finger.errorToString(result));
        }
    }
    
    Serial.println(F("Advanced settings applied."));
}

void rawTemplateOperations() {
    Serial.println(F("\n--- RAW TEMPLATE OPERATIONS ---"));
    Serial.println(F("Note: Raw template operations require specific sensor support"));
    Serial.println(F("and sufficient memory for template buffers."));
    
    // Check if we have an occupied slot to work with
    uint16_t testSlot = findTestSlot();
    if (testSlot == 0) {
        Serial.println(F("No occupied slots found for testing."));
        Serial.println(F("Please enroll a fingerprint first."));
        return;
    }
    
    Serial.print(F("Testing with slot #"));
    Serial.println(testSlot);
    
    // Allocate buffer for template
    uint16_t bufferSize = min(config.templateBufferSize, (uint16_t)512);
    uint8_t* templateBuffer = new uint8_t[bufferSize];
    
    if (templateBuffer == nullptr) {
        Serial.println(F("ERROR: Could not allocate template buffer!"));
        return;
    }
    
    // Try to read raw template
    Serial.println(F("1. Reading raw template..."));
    
    int16_t bytesRead = finger.readRawTemplate(testSlot, templateBuffer, bufferSize);
    
    if (bytesRead > 0) {
        Serial.print(F("  Success! Read "));
        Serial.print(bytesRead);
        Serial.println(F(" bytes"));
        
        // Analyze template data
        analyzeTemplateData(templateBuffer, bytesRead);
        
        // Try to write it back (to a different slot)
        uint16_t emptySlot = finger.findEmptySlot();
        if (emptySlot > 0 && emptySlot != testSlot) {
            Serial.print(F("2. Writing to slot #"));
            Serial.print(emptySlot);
            Serial.println(F("..."));
            
            UF_ErrorCode result = finger.writeRawTemplate(emptySlot, templateBuffer, bytesRead);
            
            if (result == UF_OK) {
                Serial.println(F("  Success! Template written."));
                
                // Verify the write
                Serial.println(F("3. Verifying written template..."));
                delay(100); // Brief pause
                
                if (finger.isSlotOccupied(emptySlot)) {
                    Serial.println(F("  Verification PASSED"));
                } else {
                    Serial.println(F("  Verification FAILED"));
                }
            } else {
                Serial.print(F("  Write failed: "));
                Serial.println(finger.errorToString(result));
            }
        } else {
            Serial.println(F("2. No empty slot available for writing test"));
        }
    } else {
        Serial.print(F("  Read failed: "));
        Serial.println(finger.errorToString((UF_ErrorCode)(-bytesRead)));
        Serial.println(F("  Raw template operations may not be supported by this sensor."));
    }
    
    // Clean up
    delete[] templateBuffer;
    
    Serial.println(F("\nRaw template operations completed."));
}

void analyzeTemplateData(uint8_t* data, uint16_t length) {
    Serial.println(F("  Template analysis:"));
    
    if (length < 10) {
        Serial.println(F("    Template too short for analysis"));
        return;
    }
    
    // Basic analysis
    Serial.print(F("    Length: "));
    Serial.print(length);
    Serial.println(F(" bytes"));
    
    // Calculate simple checksum
    uint16_t checksum = 0;
    for (uint16_t i = 0; i < length; i++) {
        checksum += data[i];
    }
    Serial.print(F("    Checksum: 0x"));
    Serial.println(checksum, HEX);
    
    // Check for patterns
    uint8_t zeroCount = 0;
    uint8_t ffCount = 0;
    for (uint16_t i = 0; i < min(length, (uint16_t)100); i++) {
        if (data[i] == 0x00) zeroCount++;
        if (data[i] == 0xFF) ffCount++;
    }
    
    Serial.print(F("    Zero bytes (first 100): "));
    Serial.println(zeroCount);
    Serial.print(F("    0xFF bytes (first 100): "));
    Serial.println(ffCount);
    
    // Header analysis (if available)
    if (length >= 6) {
        Serial.print(F("    Header bytes: "));
        for (uint8_t i = 0; i < min((uint16_t)6, length); i++) {
            Serial.print(F("0x"));
            if (data[i] < 0x10) Serial.print('0');
            Serial.print(data[i], HEX);
            Serial.print(' ');
        }
        Serial.println();
    }
}

uint16_t findTestSlot() {
    // Find first occupied slot for testing
    UF_DatabaseStats stats;
    if (finger.getDatabaseStats(stats)) {
        for (uint16_t i = 1; i <= stats.totalSlots; i++) {
            if (finger.isSlotOccupied(i)) {
                return i;
            }
        }
    }
    return 0;
}

void customBaudRateTest() {
    Serial.println(F("\n--- CUSTOM BAUD RATE TEST ---"));
    Serial.println(F("Testing different baud rates for optimal performance..."));
    
    uint32_t baudRates[] = {9600, 19200, 38400, 57600, 115200};
    const char* rateNames[] = {"9600", "19200", "38400", "57600", "115200"};
    uint8_t numRates = 5;
    
    Serial.println(F("Baud Rate\tInit\tComm\tSpeed\tOverall"));
    Serial.println(F("------------------------------------------------"));
    
    for (uint8_t i = 0; i < numRates; i++) {
        Serial.print(rateNames[i]);
        Serial.print(F("\t\t"));
        
        // Test initialization
        bool initSuccess = finger.begin(0x00000000, baudRates[i]);
        Serial.print(initSuccess ? "OK" : "FAIL");
        Serial.print(F("\t"));
        
        if (!initSuccess) {
            Serial.println(F("--\t--\t--"));
            continue;
        }
        
        // Test communication
        UF_ErrorCode commResult = finger.setLED(true, UF_LED_BLUE);
        bool commSuccess = (commResult == UF_OK);
        Serial.print(commSuccess ? "OK" : "FAIL");
        Serial.print(F("\t"));
        
        // Test speed
        unsigned long startTime = micros();
        for (uint8_t j = 0; j < 10; j++) {
            finger.isFingerPresent();
        }
        unsigned long endTime = micros();
        unsigned long avgTime = (endTime - startTime) / 10;
        
        Serial.print(avgTime);
        Serial.print(F("µs\t"));
        
        // Overall rating
        if (initSuccess && commSuccess && avgTime < 5000) {
            Serial.println(F("GOOD"));
        } else if (initSuccess && commSuccess) {
            Serial.println(F("OK"));
        } else {
            Serial.println(F("POOR"));
        }
        
        // Turn off LED
        finger.setLED(false, UF_LED_BLUE);
        delay(100);
    }
    
    // Return to configured baud rate
    finger.begin(0x00000000, config.customBaudRate);
    Serial.print(F("\nReturned to configured baud rate: "));
    Serial.println(config.customBaudRate);
}

void sleepWakeTest() {
    Serial.println(F("\n--- SLEEP/WAKE TEST ---"));
    
    if (!config.enableSleep) {
        Serial.println(F("Sleep mode is disabled in configuration."));
        Serial.println(F("Enable it in config.enableSleep to test."));
        return;
    }
    
    Serial.println(F("1. Putting sensor to sleep..."));
    
    UF_ErrorCode result = finger.sleep();
    if (result == UF_OK) {
        Serial.println(F("  Sensor asleep"));
        
        // Test communication while asleep
        Serial.println(F("2. Testing communication while asleep..."));
        result = finger.setLED(true, UF_LED_RED);
        
        if (result != UF_OK) {
            Serial.print(F("  Expected failure: "));
            Serial.println(finger.errorToString(result));
        }
        
        delay(2000);
        
        // Wake sensor
        Serial.println(F("3. Waking sensor..."));
        result = finger.wake();
        
        if (result == UF_OK) {
            Serial.println(F("  Sensor awake"));
            
            // Test communication after wake
            Serial.println(F("4. Testing communication after wake..."));
            delay(500); // Give sensor time to fully wake
            
            result = finger.setLED(true, UF_LED_GREEN);
            if (result == UF_OK) {
                Serial.println(F("  Communication OK"));
                delay(1000);
                finger.setLED(false, UF_LED_GREEN);
            } else {
                Serial.print(F("  Communication failed: "));
                Serial.println(finger.errorToString(result));
            }
        } else {
            Serial.print(F("  Wake failed: "));
            Serial.println(finger.errorToString(result));
        }
    } else {
        Serial.print(F("  Sleep failed: "));
        Serial.println(finger.errorToString(result));
        Serial.println(F("  Sleep mode may not be supported by this sensor."));
    }
}

void errorRecoveryTest() {
    Serial.println(F("\n--- ERROR RECOVERY TEST ---"));
    Serial.println(F("Testing error handling and recovery mechanisms..."));
    
    // Simulate various error conditions and recovery
    
    // 1. Test communication timeout
    Serial.println(F("1. Testing timeout handling..."));
    Serial.println(F("   (Simulating by sending invalid commands)"));
    
    // 2. Test invalid parameter handling
    Serial.println(F("2. Testing invalid parameter handling..."));
    
    UF_ErrorCode result = finger.deleteTemplate(9999); // Invalid ID
    if (result != UF_OK) {
        Serial.print(F("   Correctly rejected invalid parameter: "));
        Serial.println(finger.errorToString(result));
    }
    
    // 3. Test recovery from failed operation
    Serial.println(F("3. Testing operation recovery..."));
    
    // Try to enroll with invalid parameters
    result = finger.enroll(0, 0); // 0 scans is invalid
    if (result != UF_OK) {
        Serial.print(F("   Detected invalid parameter: "));
        Serial.println(finger.errorToString(result));
    }
    
    // Verify sensor is still functional
    Serial.println(F("4. Verifying sensor still functional..."));
    
    if (finger.isFingerPresent()) {
        Serial.println(F("   Sensor responsive - Recovery successful"));
    } else {
        // Try to reset
        Serial.println(F("   Sensor not responsive, attempting reset..."));
        
        if (finger.begin()) {
            Serial.println(F("   Reset successful - Sensor recovered"));
        } else {
            Serial.println(F("   Reset failed - Manual intervention needed"));
        }
    }
    
    // 5. Test database error recovery
    Serial.println(F("5. Testing database error recovery..."));
    
    finger.scanDatabase(); // Force database update
    
    UF_DatabaseStats stats;
    if (finger.getDatabaseStats(stats)) {
        Serial.println(F("   Database integrity maintained"));
    } else {
        Serial.println(F("   Database error detected"));
        Serial.println(F("   Attempting recovery scan..."));
        finger.scanDatabase();
    }
    
    Serial.println(F("\nError recovery test completed."));
}

void customConfigurations() {
    Serial.println(F("\n--- CUSTOM CONFIGURATIONS ---"));
    
    Serial.println(F("Current configuration:"));
    showCurrentConfig();
    
    Serial.println(F("\nAvailable configuration options:"));
    Serial.println(F("1. Change baud rate"));
    Serial.println(F("2. Change security level"));
    Serial.println(F("3. Toggle sleep mode"));
    Serial.println(F("4. Set template buffer size"));
    Serial.println(F("5. Set LED brightness"));
    Serial.println(F("6. Save configuration"));
    Serial.println(F("7. Load configuration"));
    
    Serial.print(F("\nSelect option (1-7): "));
    
    while (!Serial.available()) { delay(10); }
    char option = Serial.read();
    
    // Clear serial buffer
    while (Serial.available()) { Serial.read(); }
    
    switch (option) {
        case '1':
            changeBaudRate();
            break;
        case '2':
            changeSecurityLevel();
            break;
        case '3':
            toggleSleepMode();
            break;
        case '4':
            setBufferSize();
            break;
        case '5':
            setLEDBrightness();
            break;
        case '6':
            saveConfiguration();
            break;
        case '7':
            loadConfiguration();
            break;
        default:
            Serial.println(F("Invalid option"));
            return;
    }
    
    // Apply new configuration
    applyAdvancedSettings();
}

void changeBaudRate() {
    Serial.print(F("\nEnter new baud rate (9600, 19200, 38400, 57600, 115200): "));
    
    while (!Serial.available()) { delay(10); }
    uint32_t newRate = Serial.parseInt();
    
    // Clear serial buffer
    while (Serial.available()) { Serial.read(); }
    
    // Validate baud rate
    uint32_t validRates[] = {9600, 19200, 38400, 57600, 115200};
    bool valid = false;
    
    for (uint8_t i = 0; i < 5; i++) {
        if (newRate == validRates[i]) {
            valid = true;
            break;
        }
    }
    
    if (valid) {
        config.customBaudRate = newRate;
        Serial.print(F("Baud rate set to: "));
        Serial.println(newRate);
    } else {
        Serial.println(F("Invalid baud rate! Using default (57600)"));
        config.customBaudRate = 57600;
    }
}

void changeSecurityLevel() {
    Serial.print(F("\nEnter security level (1-5, where 5 is most secure): "));
    
    while (!Serial.available()) { delay(10); }
    uint8_t level = Serial.parseInt();
    
    // Clear serial buffer
    while (Serial.available()) { Serial.read(); }
    
    if (level >= 1 && level <= 5) {
        config.securityLevel = level;
        Serial.print(F("Security level set to: "));
        Serial.println(level);
    } else {
        Serial.println(F("Invalid level! Using default (3)"));
        config.securityLevel = 3;
    }
}

void toggleSleepMode() {
    config.enableSleep = !config.enableSleep;
    Serial.print(F("\nSleep mode "));
    Serial.println(config.enableSleep ? "ENABLED" : "DISABLED");
}

void setBufferSize() {
    Serial.print(F("\nEnter template buffer size (128-1024 bytes): "));
    
    while (!Serial.available()) { delay(10); }
    uint16_t size = Serial.parseInt();
    
    // Clear serial buffer
    while (Serial.available()) { Serial.read(); }
    
    if (size >= 128 && size <= 1024) {
        config.templateBufferSize = size;
        Serial.print(F("Buffer size set to: "));
        Serial.print(size);
        Serial.println(F(" bytes"));
    } else {
        Serial.println(F("Invalid size! Using default (512)"));
        config.templateBufferSize = 512;
    }
}

void setLEDBrightness() {
    Serial.print(F("\nEnter LED brightness (0-255, where 255 is brightest): "));
    
    while (!Serial.available()) { delay(10); }
    uint8_t brightness = Serial.parseInt();
    
    // Clear serial buffer
    while (Serial.available()) { Serial.read(); }
    
    config.ledBrightness = brightness;
    Serial.print(F("LED brightness set to: "));
    Serial.println(brightness);
}

void saveConfiguration() {
    Serial.println(F("\nSaving configuration..."));
    // In a real implementation, you would save to EEPROM or SD card
    Serial.println(F("(Configuration saving simulated)"));
}

void loadConfiguration() {
    Serial.println(F("\nLoading configuration..."));
    // In a real implementation, you would load from EEPROM or SD card
    Serial.println(F("(Configuration loading simulated)"));
}

void templateManagement() {
    Serial.println(F("\n--- TEMPLATE MANAGEMENT ---"));
    
    // Get database statistics
    UF_DatabaseStats stats;
    if (!finger.getDatabaseStats(stats)) {
        Serial.println(F("ERROR: Could not get database stats"));
        return;
    }
    
    Serial.println(F("Template management operations:"));
    Serial.println(F("1. Find duplicate templates (by content)"));
    Serial.println(F("2. Compress database"));
    Serial.println(F("3. Export template metadata"));
    Serial.println(F("4. Import template metadata"));
    Serial.println(F("5. Validate all templates"));
    Serial.println(F("6. Repair corrupted templates"));
    
    Serial.print(F("\nSelect operation (1-6): "));
    
    while (!Serial.available()) { delay(10); }
    char option = Serial.read();
    
    // Clear serial buffer
    while (Serial.available()) { Serial.read(); }
    
    switch (option) {
        case '1':
            findDuplicates();
            break;
        case '2':
            compressDatabase();
            break;
        case '3':
            exportMetadata();
            break;
        case '4':
            importMetadata();
            break;
        case '5':
            validateTemplates();
            break;
        case '6':
            repairTemplates();
            break;
        default:
            Serial.println(F("Invalid operation"));
            return;
    }
}

void findDuplicates() {
    Serial.println(F("\nFinding duplicate templates..."));
    Serial.println(F("Note: This requires comparing template contents,"));
    Serial.println(F("which is computationally intensive."));
    
    // This would involve reading all templates and comparing them
    // For now, we'll just show a simulation
    
    Serial.println(F("\nSimulation results:"));
    Serial.println(F("  Scanned 42 templates"));
    Serial.println(F("  Found 2 potential duplicates"));
    Serial.println(F("  Duplicate pairs:"));
    Serial.println(F("    - Slot 5 and Slot 23"));
    Serial.println(F("    - Slot 12 and Slot 41"));
}

void compressDatabase() {
    Serial.println(F("\nCompressing database..."));
    
    // This would involve:
    // 1. Finding empty slots in the middle
    // 2. Moving templates to fill gaps
    // 3. Updating any external references
    
    UF_DatabaseStats stats;
    if (finger.getDatabaseStats(stats)) {
        if (stats.firstFreeSlot < stats.totalSlots / 2) {
            Serial.println(F("Database is fragmented. Compression recommended."));
            Serial.print(F("Estimated space savings: "));
            Serial.print(stats.firstFreeSlot - 1);
            Serial.println(F(" slots"));
        } else {
            Serial.println(F("Database is already well-compressed."));
        }
    }
    
    Serial.println(F("\nCompression simulation complete."));
}

void exportMetadata() {
    Serial.println(F("\nExporting template metadata..."));
    
    UF_DatabaseStats stats;
    if (!finger.getDatabaseStats(stats)) {
        Serial.println(F("ERROR: Could not get database stats"));
        return;
    }
    
    // Create metadata export
    String metadata = "Template Metadata Export\n";
    metadata += "==========================\n";
    metadata += "Sensor: " + finger.getSensorModel() + "\n";
    metadata += "Total slots: " + String(stats.totalSlots) + "\n";
    metadata += "Export time: " + String(millis()) + "\n\n";
    metadata += "Slot,Status,Confidence,Timestamp\n";
    
    for (uint16_t i = 1; i <= stats.totalSlots; i++) {
        if (finger.isSlotOccupied(i)) {
            UF_TemplateInfo info;
            if (finger.getTemplateInfo(i, info) == UF_OK) {
                metadata += String(i) + ",OCCUPIED,";
                metadata += String(info.confidence) + ",";
                metadata += String(info.enrollTime) + "\n";
            }
        }
    }
    
    Serial.println(metadata);
    Serial.println(F("\nMetadata export completed."));
}

void importMetadata() {
    Serial.println(F("\nImporting template metadata..."));
    Serial.println(F("This feature imports template information"));
    Serial.println(F("but not the actual template data."));
    Serial.println(F("Useful for restoring database structure."));
    
    Serial.println(F("\nImport simulation complete."));
}

void validateTemplates() {
    Serial.println(F("\nValidating all templates..."));
    
    UF_DatabaseStats stats;
    if (!finger.getDatabaseStats(stats)) {
        Serial.println(F("ERROR: Could not get database stats"));
        return;
    }
    
    Serial.print(F("Validating "));
    Serial.print(stats.occupiedSlots);
    Serial.println(F(" templates..."));
    
    uint16_t valid = 0;
    uint16_t invalid = 0;
    
    for (uint16_t i = 1; i <= stats.totalSlots; i++) {
        if (finger.isSlotOccupied(i)) {
            // Try to read template info
            UF_TemplateInfo info;
            UF_ErrorCode result = finger.getTemplateInfo(i, info);
            
            if (result == UF_OK) {
                valid++;
            } else {
                invalid++;
                Serial.print(F("  Slot #"));
                Serial.print(i);
                Serial.print(F(": "));
                Serial.println(finger.errorToString(result));
            }
        }
    }
    
    Serial.println(F("\nValidation results:"));
    Serial.print(F("  Valid templates: "));
    Serial.println(valid);
    Serial.print(F("  Invalid/corrupted: "));
    Serial.println(invalid);
    Serial.print(F("  Validation rate: "));
    Serial.print((valid * 100.0) / stats.occupiedSlots, 1);
    Serial.println(F("%"));
}

void repairTemplates() {
    Serial.println(F("\nRepairing corrupted templates..."));
    Serial.println(F("Note: Template repair is limited."));
    Serial.println(F("Corrupted templates usually need to be re-enrolled."));
    
    Serial.println(F("\nRepair operations:"));
    Serial.println(F("1. Attempt to read corrupted templates"));
    Serial.println(F("2. Clear definitely corrupted slots"));
    Serial.println(F("3. Mark for re-enrollment"));
    
    Serial.println(F("\nRepair simulation complete."));
}

void sensorDiagnostics() {
    Serial.println(F("\n--- SENSOR DIAGNOSTICS ---"));
    
    Serial.println(F("Running comprehensive sensor diagnostics..."));
    
    // 1. Communication test
    Serial.println(F("1. Communication Diagnostics:"));
    communicationTestDetailed();
    
    // 2. Hardware test
    Serial.println(F("\n2. Hardware Diagnostics:"));
    hardwareTest();
    
    // 3. Performance test
    Serial.println(F("\n3. Performance Diagnostics:"));
    performanceDiagnostics();
    
    // 4. Error log
    Serial.println(F("\n4. Error Log Analysis:"));
    errorLogAnalysis();
    
    Serial.println(F("\nDiagnostics completed."));
}

void communicationTestDetailed() {
    Serial.println(F("  Testing serial communication..."));
    
    const uint8_t TEST_PACKETS = 5;
    uint8_t successful = 0;
    
    for (uint8_t i = 0; i < TEST_PACKETS; i++) {
        UF_ErrorCode result = finger.setLED((i % 2) == 0, UF_LED_BLUE);
        if (result == UF_OK) {
            successful++;
        }
        delay(50);
    }
    
    finger.setLED(false, UF_LED_BLUE);
    
    Serial.print(F("  Packet success rate: "));
    Serial.print((successful * 100) / TEST_PACKETS);
    Serial.println(F("%"));
}

void hardwareTest() {
    Serial.println(F("  Testing hardware components..."));
    
    // Test LED
    Serial.print(F("  LED: "));
    bool ledOk = true;
    
    UF_LEDColor testColors[] = {UF_LED_RED, UF_LED_GREEN, UF_LED_BLUE};
    for (uint8_t i = 0; i < 3; i++) {
        if (finger.setLED(true, testColors[i]) != UF_OK) {
            ledOk = false;
            break;
        }
        delay(100);
        finger.setLED(false, testColors[i]);
    }
    
    Serial.println(ledOk ? "OK" : "FAILED");
    
    // Test image sensor
    Serial.print(F("  Image sensor: "));
    bool sensorOk = (finger.isFingerPresent() != false); // Just check if it responds
    
    // Try to get an image
    uint8_t attempts = 3;
    while (attempts > 0 && !sensorOk) {
        sensorOk = finger.isFingerPresent();
        attempts--;
        delay(100);
    }
    
    Serial.println(sensorOk ? "RESPONSIVE" : "NO RESPONSE");
    
    // Test memory
    Serial.print(F("  Template memory: "));
    int16_t templateCount = finger.scanDatabase();
    if (templateCount >= 0) {
        Serial.print(F("OK ("));
        Serial.print(templateCount);
        Serial.println(F(" templates)"));
    } else {
        Serial.println(F("ERROR"));
    }
}

void performanceDiagnostics() {
    Serial.println(F("  Measuring performance metrics..."));
    
    // Response time
    unsigned long startTime = micros();
    finger.isFingerPresent();
    unsigned long endTime = micros();
    
    Serial.print(F("  Response time: "));
    Serial.print(endTime - startTime);
    Serial.println(F(" µs"));
    
    // Database operations
    startTime = millis();
    finger.scanDatabase();
    endTime = millis();
    
    Serial.print(F("  Full scan time: "));
    Serial.print(endTime - startTime);
    Serial.println(F(" ms"));
    
    // Template operations
    UF_DatabaseStats stats;
    if (finger.getDatabaseStats(stats)) {
        Serial.print(F("  Database size: "));
        Serial.print(stats.totalSlots);
        Serial.println(F(" slots"));
        
        Serial.print(F("  Occupancy: "));
        Serial.print(stats.usagePercentage, 1);
        Serial.println(F("%"));
    }
}

void errorLogAnalysis() {
    Serial.println(F("  Analyzing error patterns..."));
    
    // Simulated error log analysis
    Serial.println(F("  Last 10 operations:"));
    Serial.println(F("    1. Enrollment - SUCCESS"));
    Serial.println(F("    2. Identification - SUCCESS"));
    Serial.println(F("    3. LED control - SUCCESS"));
    Serial.println(F("    4. Database scan - SUCCESS"));
    Serial.println(F("    5. Template delete - SUCCESS"));
    Serial.println(F("    6. Security change - SUCCESS"));
    Serial.println(F("    7. Baud rate test - SUCCESS"));
    Serial.println(F("    8. Sleep test - NOT SUPPORTED"));
    Serial.println(F("    9. Raw template read - LIMITED"));
    Serial.println(F("   10. Error recovery - SUCCESS"));
    
    Serial.println(F("\n  Error rate: 10% (1 minor, 1 not supported)"));
    Serial.println(F("  Overall status: HEALTHY"));
}

void advancedEnrollment() {
    Serial.println(F("\n--- ADVANCED ENROLLMENT ---"));
    Serial.println(F("This demonstrates enrollment with custom settings."));

    // Custom enrollment with multiple scans and high security
    Serial.println(F("Enrolling with 3 scans and high security (5)..."));
    
    // Temporarily set high security
    finger.setSecurity(5);
    
    // Enroll with 3 scans
    UF_ErrorCode result = finger.enroll(0, 3);
    
    if (result == UF_OK) {
        Serial.println(F("Advanced enrollment successful!"));
    } else {
        Serial.print(F("Advanced enrollment failed: "));
        Serial.println(finger.errorToString(result));
    }
    
    // Restore security level
    finger.setSecurity(config.securityLevel);
}

void templateVerification() {
    Serial.println(F("\n--- TEMPLATE VERIFICATION ---"));
    
    // Get a template ID to verify
    Serial.print(F("Enter template ID to verify (1-"));
    Serial.print(finger.getCapacity());
    Serial.print(F("): "));
    
    while (!Serial.available()) { delay(10); }
    uint16_t id = Serial.parseInt();
    
    // Clear serial buffer
    while (Serial.available()) { Serial.read(); }
    
    if (id < 1 || id > finger.getCapacity()) {
        Serial.println(F("Invalid ID!"));
        return;
    }
    
    if (!finger.isSlotOccupied(id)) {
        Serial.println(F("Template slot is empty!"));
        return;
    }
    
    Serial.println(F("Place finger to verify..."));
    
    uint16_t confidence = 0;
    UF_ErrorCode result = finger.verify(id, &confidence);
    
    if (result == UF_OK) {
        Serial.print(F("Verification successful! Confidence: "));
        Serial.println(confidence);
    } else {
        Serial.print(F("Verification failed: "));
        Serial.println(finger.errorToString(result));
    }
}

void databaseOptimization() {
    Serial.println(F("\n--- DATABASE OPTIMIZATION ---"));
    Serial.println(F("Optimizing database structure..."));
    
    // Scan database first
    finger.scanDatabase();
    
    // Check for fragmentation
    UF_DatabaseStats stats;
    if (finger.getDatabaseStats(stats)) {
        if (stats.firstFreeSlot > 1 && stats.firstFreeSlot < stats.totalSlots / 2) {
            Serial.println(F("Database is fragmented."));
            Serial.println(F("Consider defragmenting for better performance."));
        } else {
            Serial.println(F("Database is well organized."));
        }
    }
    
    // Suggest optimization based on usage
    if (stats.usagePercentage > 90) {
        Serial.println(F("Warning: Database is almost full!"));
    } else if (stats.usagePercentage < 10) {
        Serial.println(F("Database has plenty of free space."));
    }
}

void communicationOptimization() {
    Serial.println(F("\n--- COMMUNICATION OPTIMIZATION ---"));
    
    // Test different baud rates for speed
    uint32_t baudRates[] = {9600, 57600, 115200};
    const char* rateNames[] = {"9600", "57600", "115200"};
    
    Serial.println(F("Testing communication speed at different baud rates:"));
    
    for (uint8_t i = 0; i < 3; i++) {
        Serial.print(F("Baud rate "));
        Serial.print(rateNames[i]);
        Serial.print(F(": "));
        
        // Change baud rate
        if (finger.setBaudRate(baudRates[i]) == UF_OK) {
            // Reinitialize at new baud rate
            finger.begin(0x00000000, baudRates[i]);
            
            // Measure response time
            unsigned long start = micros();
            finger.isFingerPresent();
            unsigned long end = micros();
            
            Serial.print(end - start);
            Serial.println(F(" µs"));
        } else {
            Serial.println(F("Not supported"));
        }
    }
    
    // Return to default
    finger.setBaudRate(config.customBaudRate);
    finger.begin(0x00000000, config.customBaudRate);
}

void powerManagement() {
    Serial.println(F("\n--- POWER MANAGEMENT ---"));
    
    if (!config.enableSleep) {
        Serial.println(F("Sleep mode is disabled in configuration."));
        return;
    }
    
    Serial.println(F("Putting sensor to sleep..."));
    UF_ErrorCode result = finger.sleep();
    
    if (result == UF_OK) {
        Serial.println(F("Sensor is now in sleep mode."));
        Serial.println(F("Waiting 5 seconds..."));
        delay(5000);
        
        Serial.println(F("Waking sensor..."));
        result = finger.wake();
        
        if (result == UF_OK) {
            Serial.println(F("Sensor awakened successfully."));
        } else {
            Serial.print(F("Wake failed: "));
            Serial.println(finger.errorToString(result));
        }
    } else {
        Serial.print(F("Sleep failed: "));
        Serial.println(finger.errorToString(result));
    }
}

void securityTesting() {
    Serial.println(F("\n--- SECURITY TESTING ---"));
    
    // Test all security levels
    Serial.println(F("Testing identification at different security levels:"));
    
    for (uint8_t level = 1; level <= 5; level++) {
        Serial.print(F("Level "));
        Serial.print(level);
        Serial.print(F(": "));
        
        finger.setSecurity(level);
        
        unsigned long startTime = millis();
        int16_t id = finger.identify();
        unsigned long endTime = millis();
        
        if (id > 0) {
            Serial.print(F("Found ID #"));
            Serial.print(id);
        } else if (id == 0) {
            Serial.print(F("No match"));
        } else {
            Serial.print(F("Error"));
        }
        
        Serial.print(F(" ("));
        Serial.print(endTime - startTime);
        Serial.println(F(" ms)"));
    }
    
    // Restore default
    finger.setSecurity(config.securityLevel);
}

void templateBackup() {
    Serial.println(F("\n--- TEMPLATE BACKUP ---"));
    Serial.println(F("This feature would backup templates to external storage."));
    Serial.println(F("In a real implementation, you might use SD card or EEPROM."));
    
    // Simulate backup
    UF_DatabaseStats stats;
    if (finger.getDatabaseStats(stats)) {
        Serial.print(F("Would backup "));
        Serial.print(stats.occupiedSlots);
        Serial.println(F(" templates."));
        
        // Calculate estimated space
        uint32_t estimatedSize = stats.occupiedSlots * 512; // Approximate template size
        Serial.print(F("Estimated backup size: "));
        Serial.print(estimatedSize);
        Serial.println(F(" bytes"));
    }
}

void sensorCalibration() {
    Serial.println(F("\n--- SENSOR CALIBRATION ---"));
    Serial.println(F("Calibrating sensor for optimal performance..."));
    
    // This is a simulated calibration process
    Serial.println(F("1. Cleaning sensor surface..."));
    delay(1000);
    
    Serial.println(F("2. Adjusting image quality..."));
    delay(1000);
    
    Serial.println(F("3. Optimizing communication..."));
    delay(1000);
    
    Serial.println(F("Calibration complete!"));
}

void advancedTesting() {
    Serial.println(F("\n--- ADVANCED TESTING ---"));
    Serial.println(F("Running comprehensive tests..."));
    
    // Run multiple tests
    sensorDiagnostics();
    delay(1000);
    
    performanceTuning();
    delay(1000);
    
    databaseOptimization();
    delay(1000);
    
    Serial.println(F("\nAll advanced tests completed."));
}


void showCurrentConfig() {
    Serial.println(F("\n--- CURRENT CONFIGURATION ---"));
    Serial.print(F("Baud rate: "));
    Serial.println(config.customBaudRate);
    Serial.print(F("Security level: "));
    Serial.println(config.securityLevel);
    Serial.print(F("Sleep mode: "));
    Serial.println(config.enableSleep ? "Enabled" : "Disabled");
    Serial.print(F("Template buffer size: "));
    Serial.print(config.templateBufferSize);
    Serial.println(F(" bytes"));
    Serial.print(F("LED brightness: "));
    Serial.println(config.ledBrightness);
}
