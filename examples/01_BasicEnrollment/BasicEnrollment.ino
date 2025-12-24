/*
  BasicEnrollment.ino
  UniversalFingerprint Library - Basic Example
  
  Demonstrates basic fingerprint enrollment and identification.
  
  Connections:
  - Fingerprint Sensor TX -> Arduino D2
  - Fingerprint Sensor RX -> Arduino D3
  - Fingerprint Sensor VCC -> 3.3V or 5V (check sensor specs)
  - Fingerprint Sensor GND -> GND
  
  Created: 2024
  Version: 1.0.0
*/

#include <UniversalFingerprint.h>
#include <SoftwareSerial.h>

// Create software serial for fingerprint sensor
SoftwareSerial fingerSerial(2, 3); // RX, TX

// Create fingerprint object
UniversalFingerprint finger(&fingerSerial);

void setup() {
    Serial.begin(115200);
    while (!Serial); // Wait for serial port
    
    Serial.println(F("\n=== UniversalFingerprint Basic Example ==="));
    Serial.println(F("Library Version: ") + finger.getVersion());
    
    // Initialize fingerprint sensor
    Serial.println(F("Initializing fingerprint sensor..."));
    
    if (!finger.begin()) {
        Serial.println(F("ERROR: Could not find fingerprint sensor!"));
        Serial.println(F("Please check wiring and power."));
        while (1) { delay(100); }
    }
    
    Serial.print(F("Sensor detected: "));
    Serial.println(finger.getSensorModel());
    
    Serial.print(F("Capacity: "));
    Serial.print(finger.getCapacity());
    Serial.println(F(" templates"));
    
    // Show current database status
    finger.printDatabaseInfo();
    
    Serial.println(F("\nReady for commands:"));
    printMenu();
}

void loop() {
    if (Serial.available()) {
        char command = Serial.read();
        
        switch (command) {
            case '1': enrollFinger(); break;
            case '2': identifyFinger(); break;
            case '3': verifyFinger(); break;
            case '4': deleteFinger(); break;
            case '5': finger.printDatabaseInfo(); break;
            case '6': scanDatabase(); break;
            case '7': clearDatabase(); break;
            case 'i': sensorInfo(); break;
            case 'h': printMenu(); break;
            case 'l': ledTest(); break;
            case '\n': case '\r': break; // Ignore newlines
            default:
                Serial.println(F("Unknown command. Type 'h' for help."));
                break;
        }
    }
}

void printMenu() {
    Serial.println(F("\n=== MENU ==="));
    Serial.println(F("1) Enroll fingerprint"));
    Serial.println(F("2) Identify fingerprint"));
    Serial.println(F("3) Verify fingerprint"));
    Serial.println(F("4) Delete fingerprint"));
    Serial.println(F("5) Show database info"));
    Serial.println(F("6) Scan database"));
    Serial.println(F("7) Clear all fingerprints"));
    Serial.println(F("i) Sensor information"));
    Serial.println(F("l) LED test"));
    Serial.println(F("h) Show this menu"));
    Serial.println(F("============="));
}

void enrollFinger() {
    Serial.println(F("\n--- ENROLLMENT ---"));
    
    // Ask for ID or auto-select
    Serial.println(F("Enter template ID (1-") + String(finger.getCapacity()) + 
                   F(") or 0 for auto-select: "));
    
    while (!Serial.available()) { delay(10); }
    uint16_t id = Serial.parseInt();
    
    // Clear serial buffer
    while (Serial.available()) { Serial.read(); }
    
    if (id == 0) {
        Serial.println(F("Auto-selecting ID..."));
    } else if (id < 1 || id > finger.getCapacity()) {
        Serial.println(F("ERROR: Invalid ID!"));
        return;
    }
    
    // Start enrollment
    Serial.println(F("Starting enrollment process..."));
    
    UF_ErrorCode result = finger.enroll(id);
    
    if (result == UF_OK) {
        Serial.print(F("SUCCESS: Fingerprint enrolled with ID #"));
        Serial.println(id);
    } else {
        Serial.print(F("ERROR: Enrollment failed - "));
        Serial.println(finger.errorToString(result));
    }
}

void identifyFinger() {
    Serial.println(F("\n--- IDENTIFICATION ---"));
    Serial.println(F("Place finger on sensor..."));
    
    uint16_t confidence = 0;
    int16_t id = finger.identify(&confidence);
    
    if (id > 0) {
        Serial.print(F("Fingerprint MATCHED!"));
        Serial.print(F(" ID: #"));
        Serial.print(id);
        Serial.print(F(" Confidence: "));
        Serial.println(confidence);
    } else if (id == 0) {
        Serial.println(F("Fingerprint NOT FOUND in database"));
    } else {
        Serial.print(F("ERROR: Identification failed - "));
        Serial.println(finger.errorToString((UF_ErrorCode)(-id)));
    }
}

void verifyFinger() {
    Serial.println(F("\n--- VERIFICATION ---"));
    Serial.print(F("Enter template ID to verify (1-") + String(finger.getCapacity()) + F("): "));
    
    while (!Serial.available()) { delay(10); }
    uint16_t id = Serial.parseInt();
    
    // Clear serial buffer
    while (Serial.available()) { Serial.read(); }
    
    if (id < 1 || id > finger.getCapacity()) {
        Serial.println(F("ERROR: Invalid ID!"));
        return;
    }
    
    Serial.println(F("Place finger on sensor..."));
    
    uint16_t confidence = 0;
    UF_ErrorCode result = finger.verify(id, &confidence);
    
    if (result == UF_OK) {
        Serial.print(F("VERIFIED! Confidence: "));
        Serial.println(confidence);
    } else {
        Serial.print(F("NOT VERIFIED: "));
        Serial.println(finger.errorToString(result));
    }
}

void deleteFinger() {
    Serial.println(F("\n--- DELETE ---"));
    Serial.print(F("Enter template ID to delete (1-") + String(finger.getCapacity()) + F("): "));
    
    while (!Serial.available()) { delay(10); }
    uint16_t id = Serial.parseInt();
    
    // Clear serial buffer
    while (Serial.available()) { Serial.read(); }
    
    if (id < 1 || id > finger.getCapacity()) {
        Serial.println(F("ERROR: Invalid ID!"));
        return;
    }
    
    // Confirm deletion
    Serial.print(F("Delete fingerprint ID #"));
    Serial.print(id);
    Serial.print(F("? (y/n): "));
    
    while (!Serial.available()) { delay(10); }
    char confirm = Serial.read();
    
    // Clear serial buffer
    while (Serial.available()) { Serial.read(); }
    
    if (confirm == 'y' || confirm == 'Y') {
        UF_ErrorCode result = finger.deleteTemplate(id);
        
        if (result == UF_OK) {
            Serial.println(F("Fingerprint deleted successfully"));
        } else {
            Serial.print(F("ERROR: Deletion failed - "));
            Serial.println(finger.errorToString(result));
        }
    } else {
        Serial.println(F("Deletion cancelled"));
    }
}

void scanDatabase() {
    Serial.println(F("\n--- DATABASE SCAN ---"));
    Serial.println(F("Scanning database..."));
    
    int16_t count = finger.scanDatabase();
    
    if (count >= 0) {
        Serial.print(F("Found "));
        Serial.print(count);
        Serial.println(F(" fingerprints"));
    } else {
        Serial.println(F("ERROR: Database scan failed"));
    }
}

void clearDatabase() {
    Serial.println(F("\n--- CLEAR DATABASE ---"));
    Serial.println(F("WARNING: This will delete ALL fingerprints!"));
    Serial.print(F("Are you sure? (y/n): "));
    
    while (!Serial.available()) { delay(10); }
    char confirm = Serial.read();
    
    // Clear serial buffer
    while (Serial.available()) { Serial.read(); }
    
    if (confirm == 'y' || confirm == 'Y') {
        UF_ErrorCode result = finger.clearDatabase();
        
        if (result == UF_OK) {
            Serial.println(F("All fingerprints deleted"));
        } else {
            Serial.print(F("ERROR: Failed to clear database - "));
            Serial.println(finger.errorToString(result));
        }
    } else {
        Serial.println(F("Operation cancelled"));
    }
}

void sensorInfo() {
    Serial.println(F("\n--- SENSOR INFORMATION ---"));
    
    UF_SensorInfo info;
    if (finger.getSensorInfo(info)) {
        Serial.print(F("Model: "));
        Serial.println(info.modelName);
        
        Serial.print(F("Vendor: "));
        Serial.println(info.vendor);
        
        Serial.print(F("Capacity: "));
        Serial.print(info.capacity);
        Serial.println(F(" templates"));
        
        Serial.print(F("Baud rate: "));
        Serial.println(info.defaultBaud);
        
        Serial.print(F("Has LED: "));
        Serial.println(info.hasLED ? "Yes" : "No");
        
        Serial.print(F("Has touch sensor: "));
        Serial.println(info.hasTouchSensor ? "Yes" : "No");
    } else {
        Serial.println(F("ERROR: Could not get sensor info"));
    }
}

void ledTest() {
    Serial.println(F("\n--- LED TEST ---"));
    
    // Test different LED colors
    UF_LEDColor colors[] = {UF_LED_RED, UF_LED_BLUE, UF_LED_GREEN, UF_LED_PURPLE};
    const char* colorNames[] = {"Red", "Blue", "Green", "Purple"};
    
    for (int i = 0; i < 4; i++) {
        Serial.print(F("Turning on "));
        Serial.println(colorNames[i]);
        
        finger.setLED(true, colors[i]);
        delay(1000);
        
        finger.setLED(false, colors[i]);
        delay(500);
    }
    
    Serial.println(F("LED test complete"));
}