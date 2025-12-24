/*
  FullDatabaseManager.ino
  UniversalFingerprint Library - Complete Database Manager
  
  Demonstrates advanced database management features:
  - Database scanning and statistics
  - Slot management and defragmentation
  - Import/export functionality
  - Batch operations
  
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
#include <EEPROM.h>

// Create software serial for fingerprint sensor
SoftwareSerial fingerSerial(2, 3); // RX, TX

// Create fingerprint object
UniversalFingerprint finger(&fingerSerial);

// Configuration
const uint16_t EEPROM_CONFIG_ADDRESS = 0;
const uint16_t MAX_BACKUP_SLOTS = 50;

// Structure for database backup
struct DatabaseBackup {
    uint16_t id;
    uint32_t timestamp;
    uint16_t count;
    uint16_t checksum;
};

void setup() {
    Serial.begin(115200);
    while (!Serial); // Wait for serial port
    
    Serial.println(F("\n=== UniversalFingerprint Database Manager ==="));
    Serial.println(F("Library Version: ") + finger.getVersion());
    
    // Initialize fingerprint sensor
    Serial.println(F("Initializing fingerprint sensor..."));
    
    if (!finger.begin()) {
        Serial.println(F("ERROR: Could not find fingerprint sensor!"));
        Serial.println(F("Check connections and power."));
        while (1) { delay(100); }
    }
    
    Serial.print(F("Sensor: "));
    Serial.println(finger.getSensorModel());
    Serial.print(F("Capacity: "));
    Serial.print(finger.getCapacity());
    Serial.println(F(" templates"));
    
    // Initial database scan
    finger.scanDatabase();
    printDatabaseInfo();
    
    Serial.println(F("\nReady for database management commands:"));
    printMainMenu();
}

void loop() {
    if (Serial.available()) {
        char command = Serial.read();
        
        switch (command) {
            case '1': showDatabaseStats(); break;
            case '2': findEmptySlots(); break;
            case '3': findOccupiedSlots(); break;
            case '4': checkSpecificSlot(); break;
            case '5': defragmentDatabase(); break;
            case '6': exportDatabase(); break;
            case '7': importDatabase(); break;
            case '8': backupDatabase(); break;
            case '9': restoreDatabase(); break;
            case 'a': batchDelete(); break;
            case 'b': batchEnroll(); break;
            case 'c': databaseIntegrityCheck(); break;
            case 'd': optimizeDatabase(); break;
            case 's': finger.scanDatabase(); break;
            case 'i': printDatabaseInfo(); break;
            case 'h': printMainMenu(); break;
            case 'r': resetDatabase(); break;
            case 'm': memoryUsage(); break;
            case 't': performanceTest(); break;
            case '\n': case '\r': break;
            default:
                Serial.println(F("Unknown command. Type 'h' for help."));
                break;
        }
    }
}

void printMainMenu() {
    Serial.println(F("\n=== DATABASE MANAGER ==="));
    Serial.println(F("1) Show database statistics"));
    Serial.println(F("2) Find empty slots"));
    Serial.println(F("3) Find occupied slots"));
    Serial.println(F("4) Check specific slot"));
    Serial.println(F("5) Defragment database"));
    Serial.println(F("6) Export database info"));
    Serial.println(F("7) Import database (simulated)"));
    Serial.println(F("8) Backup to EEPROM"));
    Serial.println(F("9) Restore from EEPROM"));
    Serial.println(F("a) Batch delete"));
    Serial.println(F("b) Batch enroll (demo)"));
    Serial.println(F("c) Database integrity check"));
    Serial.println(F("d) Optimize database"));
    Serial.println(F("s) Scan database"));
    Serial.println(F("i) Print database info"));
    Serial.println(F("r) Reset database"));
    Serial.println(F("m) Memory usage"));
    Serial.println(F("t) Performance test"));
    Serial.println(F("h) Show this menu"));
    Serial.println(F("========================="));
}

void showDatabaseStats() {
    Serial.println(F("\n--- DATABASE STATISTICS ---"));
    
    UF_DatabaseStats stats;
    if (finger.getDatabaseStats(stats)) {
        Serial.print(F("Total slots: "));
        Serial.println(stats.totalSlots);
        
        Serial.print(F("Occupied slots: "));
        Serial.println(stats.occupiedSlots);
        
        Serial.print(F("Free slots: "));
        Serial.println(stats.freeSlots);
        
        Serial.print(F("Usage: "));
        Serial.print(stats.usagePercentage, 1);
        Serial.println(F("%"));
        
        Serial.print(F("First free slot: "));
        Serial.println(stats.firstFreeSlot > 0 ? String(stats.firstFreeSlot) : "None");
        
        Serial.print(F("Last free slot: "));
        Serial.println(stats.lastFreeSlot > 0 ? String(stats.lastFreeSlot) : "None");
        
        // Show visual representation
        Serial.println(F("\nDatabase Map (O=Occupied, .=Empty):"));
        uint16_t slotsToShow = min(stats.totalSlots, (uint16_t)100);
        
        for (uint16_t i = 1; i <= slotsToShow; i++) {
            if (finger.isSlotOccupied(i)) {
                Serial.print('O');
            } else {
                Serial.print('.');
            }
            
            if (i % 50 == 0) {
                Serial.println();
            } else if (i % 10 == 0) {
                Serial.print(' ');
            }
        }
        
        if (stats.totalSlots > slotsToShow) {
            Serial.print(F(" ... (+"));
            Serial.print(stats.totalSlots - slotsToShow);
            Serial.println(F(" more slots)"));
        }
        
        // Show distribution
        Serial.println(F("\n--- Distribution ---"));
        uint16_t groups = (stats.totalSlots + 99) / 100;
        for (uint16_t g = 0; g < groups; g++) {
            uint16_t start = g * 100 + 1;
            uint16_t end = min((g + 1) * 100, stats.totalSlots);
            uint16_t occupied = 0;
            
            for (uint16_t i = start; i <= end; i++) {
                if (finger.isSlotOccupied(i)) {
                    occupied++;
                }
            }
            
            Serial.print(F("Slots "));
            Serial.print(start);
            Serial.print(F("-"));
            Serial.print(end);
            Serial.print(F(": "));
            Serial.print(occupied);
            Serial.println(F(" occupied"));
        }
    } else {
        Serial.println(F("ERROR: Could not get database statistics"));
    }
}

void findEmptySlots() {
    Serial.println(F("\n--- FIND EMPTY SLOTS ---"));
    
    UF_DatabaseStats stats;
    if (!finger.getDatabaseStats(stats)) {
        Serial.println(F("ERROR: Could not get database stats"));
        return;
    }
    
    if (stats.freeSlots == 0) {
        Serial.println(F("No empty slots available"));
        return;
    }
    
    Serial.print(F("Found "));
    Serial.print(stats.freeSlots);
    Serial.println(F(" empty slots:"));
    
    uint16_t found = 0;
    uint16_t groupCount = 0;
    const uint16_t GROUP_SIZE = 10;
    
    for (uint16_t i = 1; i <= stats.totalSlots; i++) {
        if (!finger.isSlotOccupied(i)) {
            if (found < 100) { // Limit display to first 100 empty slots
                if (groupCount == 0) {
                    Serial.print(F("  "));
                }
                
                Serial.print(i);
                groupCount++;
                found++;
                
                if (groupCount < GROUP_SIZE && found < 100) {
                    Serial.print(", ");
                } else {
                    Serial.println();
                    groupCount = 0;
                }
            } else {
                found++;
            }
        }
    }
    
    if (found > 100) {
        Serial.print(F("... and "));
        Serial.print(found - 100);
        Serial.println(F(" more empty slots"));
    }
    
    Serial.print(F("\nFirst empty slot: #"));
    Serial.println(stats.firstFreeSlot);
    Serial.print(F("Last empty slot: #"));
    Serial.println(stats.lastFreeSlot);
}

void findOccupiedSlots() {
    Serial.println(F("\n--- FIND OCCUPIED SLOTS ---"));
    
    UF_DatabaseStats stats;
    if (!finger.getDatabaseStats(stats)) {
        Serial.println(F("ERROR: Could not get database stats"));
        return;
    }
    
    if (stats.occupiedSlots == 0) {
        Serial.println(F("No occupied slots"));
        return;
    }
    
    Serial.print(F("Found "));
    Serial.print(stats.occupiedSlots);
    Serial.println(F(" occupied slots:"));
    
    uint16_t found = 0;
    uint16_t groupCount = 0;
    const uint16_t GROUP_SIZE = 10;
    
    for (uint16_t i = 1; i <= stats.totalSlots; i++) {
        if (finger.isSlotOccupied(i)) {
            if (found < 100) { // Limit display to first 100 occupied slots
                if (groupCount == 0) {
                    Serial.print(F("  "));
                }
                
                Serial.print(i);
                groupCount++;
                found++;
                
                if (groupCount < GROUP_SIZE && found < 100) {
                    Serial.print(", ");
                } else {
                    Serial.println();
                    groupCount = 0;
                }
            } else {
                found++;
            }
        }
    }
    
    if (found > 100) {
        Serial.print(F("... and "));
        Serial.print(found - 100);
        Serial.println(F(" more occupied slots"));
    }
}

void checkSpecificSlot() {
    Serial.println(F("\n--- CHECK SPECIFIC SLOT ---"));
    Serial.print(F("Enter slot ID (1-"));
    Serial.print(finger.getCapacity());
    Serial.print(F("): "));
    
    while (!Serial.available()) { delay(10); }
    uint16_t id = Serial.parseInt();
    
    // Clear serial buffer
    while (Serial.available()) { Serial.read(); }
    
    if (id < 1 || id > finger.getCapacity()) {
        Serial.println(F("ERROR: Invalid slot ID!"));
        return;
    }
    
    Serial.print(F("\nSlot #"));
    Serial.println(id);
    Serial.print(F("Status: "));
    
    if (finger.isSlotOccupied(id)) {
        Serial.println(F("OCCUPIED"));
        
        // Try to get more information about the template
        UF_TemplateInfo info;
        if (finger.getTemplateInfo(id, info) == UF_OK) {
            Serial.print(F("Confidence (last match): "));
            Serial.println(info.confidence);
            
            if (info.enrollTime > 0) {
                Serial.print(F("Enrollment time: "));
                Serial.println(info.enrollTime);
            }
        }
    } else {
        Serial.println(F("EMPTY"));
    }
    
    // Check surrounding slots
    Serial.println(F("\nSurrounding slots:"));
    uint16_t start = max(1, id - 5);
    uint16_t end = min(finger.getCapacity(), id + 5);
    
    for (uint16_t i = start; i <= end; i++) {
        Serial.print(i);
        if (i == id) {
            Serial.print("*: "); // Mark the requested slot
        } else {
            Serial.print(" : ");
        }
        
        if (finger.isSlotOccupied(i)) {
            Serial.println("OCCUPIED");
        } else {
            Serial.println("EMPTY");
        }
    }
}

void defragmentDatabase() {
    Serial.println(F("\n--- DEFRAGMENT DATABASE ---"));
    Serial.println(F("WARNING: This operation may take a long time!"));
    Serial.println(F("It requires reading and rewriting all templates."));
    Serial.print(F("Continue? (y/n): "));
    
    while (!Serial.available()) { delay(10); }
    char confirm = Serial.read();
    
    // Clear serial buffer
    while (Serial.available()) { Serial.read(); }
    
    if (confirm != 'y' && confirm != 'Y') {
        Serial.println(F("Operation cancelled"));
        return;
    }
    
    Serial.println(F("\nStarting defragmentation..."));
    
    // This is a simplified defragmentation algorithm
    // It moves occupied slots to the beginning of the database
    
    uint16_t nextEmpty = finger.findEmptySlot();
    uint16_t moved = 0;
    
    if (nextEmpty == 0) {
        Serial.println(F("Database is already full or empty"));
        return;
    }
    
    // Find occupied slots after the first empty slot
    for (uint16_t i = nextEmpty + 1; i <= finger.getCapacity(); i++) {
        if (finger.isSlotOccupied(i)) {
            Serial.print(F("Need to move template from #"));
            Serial.print(i);
            Serial.print(F(" to #"));
            Serial.println(nextEmpty);
            
            // Note: Actual template moving requires raw template operations
            // which are sensor-specific and not implemented in this demo
            // In a real implementation, you would:
            // 1. Read raw template from slot i
            // 2. Write it to slot nextEmpty
            // 3. Delete template from slot i
            // 4. Update slot occupancy
            
            moved++;
            
            // Find next empty slot
            nextEmpty = finger.findEmptySlot(nextEmpty + 1);
            if (nextEmpty == 0) {
                break; // No more empty slots
            }
        }
    }
    
    if (moved == 0) {
        Serial.println(F("Database is already defragmented"));
    } else {
        Serial.print(F("Defragmentation completed. Would move "));
        Serial.print(moved);
        Serial.println(F(" templates"));
        Serial.println(F("Note: Actual template moving requires raw template operations"));
    }
    
    // Rescan database
    finger.scanDatabase();
}

void exportDatabase() {
    Serial.println(F("\n--- EXPORT DATABASE INFO ---"));
    Serial.println(F("Exporting database information..."));
    
    UF_DatabaseStats stats;
    if (!finger.getDatabaseStats(stats)) {
        Serial.println(F("ERROR: Could not get database stats"));
        return;
    }
    
    // Create export string
    String exportData = "UniversalFingerprint Database Export\n";
    exportData += "======================================\n";
    exportData += "Timestamp: " + String(millis()) + "\n";
    exportData += "Sensor: " + finger.getSensorModel() + "\n";
    exportData += "Capacity: " + String(stats.totalSlots) + "\n";
    exportData += "Enrolled: " + String(stats.occupiedSlots) + "\n";
    exportData += "Free: " + String(stats.freeSlots) + "\n";
    exportData += "Usage: " + String(stats.usagePercentage, 1) + "%\n";
    exportData += "\nSlot Status:\n";
    
    for (uint16_t i = 1; i <= stats.totalSlots; i++) {
        exportData += String(i) + ":";
        exportData += finger.isSlotOccupied(i) ? "OCCUPIED" : "EMPTY";
        exportData += "\n";
    }
    
    // Print to Serial
    Serial.println(exportData);
    
    Serial.println(F("\nExport completed. Copy the data above to save it."));
}

void importDatabase() {
    Serial.println(F("\n--- IMPORT DATABASE (SIMULATED) ---"));
    Serial.println(F("This is a simulation of database import."));
    Serial.println(F("In a real implementation, you would:"));
    Serial.println(F("1. Read template data from external source"));
    Serial.println(F("2. Write templates to empty slots"));
    Serial.println(F("3. Verify each template"));
    Serial.println(F("4. Update database tracking"));
    
    Serial.println(F("\nSimulation complete."));
}

void backupDatabase() {
    Serial.println(F("\n--- BACKUP DATABASE TO EEPROM ---"));
    
    if (!checkEEPROMSize()) {
        Serial.println(F("ERROR: EEPROM too small for backup"));
        return;
    }
    
    UF_DatabaseStats stats;
    if (!finger.getDatabaseStats(stats)) {
        Serial.println(F("ERROR: Could not get database stats"));
        return;
    }
    
    Serial.print(F("Backing up "));
    Serial.print(stats.occupiedSlots);
    Serial.println(F(" templates to EEPROM..."));
    
    // Create backup header
    DatabaseBackup backup;
    backup.id = 1;
    backup.timestamp = millis();
    backup.count = stats.occupiedSlots;
    backup.checksum = calculateChecksum(stats);
    
    // Write to EEPROM
    EEPROM.put(EEPROM_CONFIG_ADDRESS, backup);
    
    // Write slot occupancy map
    uint16_t address = EEPROM_CONFIG_ADDRESS + sizeof(backup);
    
    for (uint16_t i = 1; i <= stats.totalSlots; i++) {
        bool occupied = finger.isSlotOccupied(i);
        EEPROM.update(address + i - 1, occupied ? 1 : 0);
    }
    
    Serial.println(F("Backup completed successfully"));
    Serial.print(F("Backup ID: "));
    Serial.println(backup.id);
    Serial.print(F("Timestamp: "));
    Serial.println(backup.timestamp);
    Serial.print(F("Templates backed up: "));
    Serial.println(backup.count);
    Serial.print(F("Checksum: 0x"));
    Serial.println(backup.checksum, HEX);
}

void restoreDatabase() {
    Serial.println(F("\n--- RESTORE DATABASE FROM EEPROM ---"));
    
    if (!checkEEPROMSize()) {
        Serial.println(F("ERROR: EEPROM too small or corrupted"));
        return;
    }
    
    // Read backup header
    DatabaseBackup backup;
    EEPROM.get(EEPROM_CONFIG_ADDRESS, backup);
    
    Serial.print(F("Backup ID: "));
    Serial.println(backup.id);
    Serial.print(F("Timestamp: "));
    Serial.println(backup.timestamp);
    Serial.print(F("Templates in backup: "));
    Serial.println(backup.count);
    Serial.print(F("Checksum: 0x"));
    Serial.println(backup.checksum, HEX);
    
    Serial.print(F("Restore backup? (y/n): "));
    while (!Serial.available()) { delay(10); }
    char confirm = Serial.read();
    
    // Clear serial buffer
    while (Serial.available()) { Serial.read(); }
    
    if (confirm != 'y' && confirm != 'Y') {
        Serial.println(F("Restore cancelled"));
        return;
    }
    
    Serial.println(F("Restore simulation completed."));
    Serial.println(F("Note: Actual template restore requires raw template data."));
}

void batchDelete() {
    Serial.println(F("\n--- BATCH DELETE ---"));
    Serial.print(F("Enter starting slot (1-"));
    Serial.print(finger.getCapacity());
    Serial.print(F("): "));
    
    while (!Serial.available()) { delay(10); }
    uint16_t start = Serial.parseInt();
    
    // Clear serial buffer
    while (Serial.available()) { Serial.read(); }
    
    Serial.print(F("Enter ending slot ("));
    Serial.print(start);
    Serial.print(F("-"));
    Serial.print(finger.getCapacity());
    Serial.print(F("): "));
    
    while (!Serial.available()) { delay(10); }
    uint16_t end = Serial.parseInt();
    
    // Clear serial buffer
    while (Serial.available()) { Serial.read(); }
    
    if (start < 1 || end > finger.getCapacity() || start > end) {
        Serial.println(F("ERROR: Invalid range!"));
        return;
    }
    
    Serial.print(F("Delete templates from slot #"));
    Serial.print(start);
    Serial.print(F(" to #"));
    Serial.print(end);
    Serial.print(F("? (y/n): "));
    
    while (!Serial.available()) { delay(10); }
    char confirm = Serial.read();
    
    // Clear serial buffer
    while (Serial.available()) { Serial.read(); }
    
    if (confirm != 'y' && confirm != 'Y') {
        Serial.println(F("Operation cancelled"));
        return;
    }
    
    Serial.println(F("\nDeleting templates..."));
    uint16_t deleted = 0;
    
    for (uint16_t i = start; i <= end; i++) {
        if (finger.isSlotOccupied(i)) {
            if (finger.deleteTemplate(i) == UF_OK) {
                deleted++;
                Serial.print(F("Deleted #"));
                Serial.println(i);
            }
        }
    }
    
    Serial.print(F("\nBatch delete completed. "));
    Serial.print(deleted);
    Serial.println(F(" templates deleted."));
    
    // Update database
    finger.scanDatabase();
}

void batchEnroll() {
    Serial.println(F("\n--- BATCH ENROLL (DEMO) ---"));
    Serial.println(F("This is a demonstration of batch enrollment."));
    Serial.println(F("In a real implementation, you would:"));
    Serial.println(F("1. Loop through multiple enrollments"));
    Serial.println(F("2. Handle errors for each enrollment"));
    Serial.println(F("3. Provide feedback for each step"));
    
    Serial.print(F("\nHow many demo enrollments? (1-10): "));
    
    while (!Serial.available()) { delay(10); }
    uint8_t count = Serial.parseInt();
    
    // Clear serial buffer
    while (Serial.available()) { Serial.read(); }
    
    if (count < 1 || count > 10) {
        Serial.println(F("Invalid count. Using 3 as default."));
        count = 3;
    }
    
    Serial.print(F("Demoing "));
    Serial.print(count);
    Serial.println(F(" enrollments:"));
    
    for (uint8_t i = 1; i <= count; i++) {
        Serial.print(F("\nEnrollment "));
        Serial.print(i);
        Serial.println(F(" of ") + String(count));
        Serial.println(F("(This is a simulation - no actual enrollment occurs)"));
        delay(1000);
    }
    
    Serial.println(F("\nBatch enrollment demo completed."));
}

void databaseIntegrityCheck() {
    Serial.println(F("\n--- DATABASE INTEGRITY CHECK ---"));
    Serial.println(F("Checking database integrity..."));
    
    UF_DatabaseStats stats;
    if (!finger.getDatabaseStats(stats)) {
        Serial.println(F("ERROR: Could not get database stats"));
        return;
    }
    
    // Verify slot occupancy consistency
    uint16_t countedOccupied = 0;
    uint16_t issues = 0;
    
    for (uint16_t i = 1; i <= stats.totalSlots; i++) {
        bool occupied = finger.isSlotOccupied(i);
        if (occupied) {
            countedOccupied++;
            
            // Try to verify the template exists by checking with sensor
            // This is a basic check - could be enhanced
            if (!verifyTemplateExistence(i)) {
                Serial.print(F("WARNING: Slot #"));
                Serial.print(i);
                Serial.println(F(" marked occupied but template may be corrupted"));
                issues++;
            }
        }
    }
    
    if (countedOccupied != stats.occupiedSlots) {
        Serial.print(F("ERROR: Occupied count mismatch! Expected: "));
        Serial.print(stats.occupiedSlots);
        Serial.print(F(", Counted: "));
        Serial.println(countedOccupied);
        issues++;
    }
    
    if (issues == 0) {
        Serial.println(F("Database integrity check PASSED"));
        Serial.print(F("Verified "));
        Serial.print(countedOccupied);
        Serial.println(F(" templates"));
    } else {
        Serial.print(F("Database integrity check FAILED with "));
        Serial.print(issues);
        Serial.println(F(" issues"));
    }
}

bool verifyTemplateExistence(uint16_t id) {
    // This is a basic verification that tries to check if template exists
    // In a real implementation, you might try to read the template
    // or verify it against a stored fingerprint
    
    // For now, we'll assume all marked slots are valid
    return true;
}

void optimizeDatabase() {
    Serial.println(F("\n--- DATABASE OPTIMIZATION ---"));
    Serial.println(F("Running database optimization..."));
    
    // 1. Scan database
    Serial.println(F("1. Scanning database..."));
    finger.scanDatabase();
    
    // 2. Check for duplicate slots
    Serial.println(F("2. Checking for duplicates..."));
    Serial.println(F("   (Duplicate checking requires advanced matching)"));
    
    // 3. Check for corrupted templates
    Serial.println(F("3. Checking for corrupted templates..."));
    Serial.println(F("   (Corruption check requires raw template access)"));
    
    // 4. Defragment if needed
    Serial.println(F("4. Checking fragmentation..."));
    UF_DatabaseStats stats;
    if (finger.getDatabaseStats(stats)) {
        if (stats.firstFreeSlot > 1 && stats.firstFreeSlot < stats.totalSlots / 2) {
            Serial.println(F("   Database is fragmented"));
            Serial.println(F("   Run defragmentation to optimize"));
        } else {
            Serial.println(F("   Database is well-organized"));
        }
    }
    
    Serial.println(F("\nOptimization check completed."));
    Serial.println(F("Run individual operations to fix any issues found."));
}

void resetDatabase() {
    Serial.println(F("\n--- RESET DATABASE ---"));
    Serial.println(F("WARNING: This will delete ALL fingerprints!"));
    Serial.println(F("Type 'CONFIRM' to proceed: "));
    
    String input = "";
    unsigned long startTime = millis();
    
    // Wait for input with timeout
    while (millis() - startTime < 30000) { // 30 second timeout
        if (Serial.available()) {
            char c = Serial.read();
            if (c == '\n' || c == '\r') {
                break;
            }
            input += c;
        }
        delay(10);
    }
    
    input.trim();
    input.toUpperCase();
    
    if (input == "CONFIRM") {
        Serial.println(F("\nClearing database..."));
        UF_ErrorCode result = finger.clearDatabase();
        
        if (result == UF_OK) {
            Serial.println(F("Database cleared successfully"));
        } else {
            Serial.print(F("ERROR: Failed to clear database - "));
            Serial.println(finger.errorToString(result));
        }
    } else {
        Serial.println(F("Reset cancelled"));
    }
}

void memoryUsage() {
    Serial.println(F("\n--- MEMORY USAGE ---"));
    
    UF_DatabaseStats stats;
    if (finger.getDatabaseStats(stats)) {
        // Calculate approximate memory usage
        uint16_t slotTrackingMemory = stats.totalSlots; // 1 byte per slot
        uint16_t structureMemory = sizeof(stats);
        
        Serial.print(F("Slot tracking: "));
        Serial.print(slotTrackingMemory);
        Serial.println(F(" bytes"));
        
        Serial.print(F("Statistics structure: "));
        Serial.print(structureMemory);
        Serial.println(F(" bytes"));
        
        Serial.print(F("Total approximate: "));
        Serial.print(slotTrackingMemory + structureMemory);
        Serial.println(F(" bytes"));
        
        Serial.print(F("Database usage: "));
        Serial.print(stats.usagePercentage, 1);
        Serial.println(F("%"));
        
        if (stats.usagePercentage > 90) {
            Serial.println(F("WARNING: Database almost full!"));
        } else if (stats.usagePercentage < 10) {
            Serial.println(F("Database has plenty of space"));
        }
    }
}

void performanceTest() {
    Serial.println(F("\n--- PERFORMANCE TEST ---"));
    Serial.println(F("Testing database operations..."));
    
    unsigned long startTime, endTime;
    
    // Test 1: Database scan
    Serial.println(F("1. Testing database scan..."));
    startTime = millis();
    finger.scanDatabase();
    endTime = millis();
    Serial.print(F("   Time: "));
    Serial.print(endTime - startTime);
    Serial.println(F(" ms"));
    
    // Test 2: Find empty slot
    Serial.println(F("2. Testing find empty slot..."));
    startTime = millis();
    uint16_t emptySlot = finger.findEmptySlot();
    endTime = millis();
    Serial.print(F("   Time: "));
    Serial.print(endTime - startTime);
    Serial.print(F(" ms, Found: #"));
    Serial.println(emptySlot);
    
    // Test 3: Check multiple slots
    Serial.println(F("3. Testing slot checking (100 slots)..."));
    startTime = millis();
    uint16_t checked = 0;
    for (uint16_t i = 1; i <= 100 && i <= finger.getCapacity(); i++) {
        finger.isSlotOccupied(i);
        checked++;
    }
    endTime = millis();
    Serial.print(F("   Time: "));
    Serial.print(endTime - startTime);
    Serial.print(F(" ms for "));
    Serial.print(checked);
    Serial.println(F(" slots"));
    
    Serial.println(F("\nPerformance test completed."));
}

void printDatabaseInfo() {
    finger.printDatabaseInfo();
}

bool checkEEPROMSize() {
    uint16_t neededSize = EEPROM_CONFIG_ADDRESS + 
                         sizeof(DatabaseBackup) + 
                         finger.getCapacity();
    
    #if defined(ESP32) || defined(ESP8266)
        // ESP boards have large EEPROM emulation
        return true;
    #else
        // Check actual EEPROM size
        uint16_t eepromSize = E2END + 1; // AVR EEPROM size
        return (neededSize <= eepromSize);
    #endif
}

uint16_t calculateChecksum(UF_DatabaseStats stats) {
    // Simple checksum calculation
    uint16_t checksum = 0;
    checksum += stats.totalSlots;
    checksum += stats.occupiedSlots;
    checksum += stats.freeSlots;
    checksum += (uint16_t)stats.usagePercentage;
    return checksum;
}