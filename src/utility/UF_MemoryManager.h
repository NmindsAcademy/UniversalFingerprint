/**
 * @file UF_MemoryManager.h
 * @brief Memory management for fingerprint templates
 */

#ifndef UF_MEMORYMANAGER_H
#define UF_MEMORYMANAGER_H

#include <Adafruit_Fingerprint.h>

class UF_MemoryManager {
public:
    UF_MemoryManager(Adafruit_Fingerprint* finger, uint16_t capacity);
    ~UF_MemoryManager();
    
    bool init();
    uint16_t getCapacity() const { return _capacity; }
    uint16_t getEnrolledCount() const { return _enrolledCount; }
    
    bool isSlotOccupied(uint16_t id);
    uint16_t findEmptySlot(uint16_t start = 1);
    uint16_t findEmptySlots(uint16_t count, uint16_t* results, uint16_t start = 1);
    
    bool updateSlot(uint16_t id, bool occupied);
    bool scanAllSlots();
    
    bool defragment();
    bool getStatistics(uint16_t& total, uint16_t& occupied, uint16_t& free);
    
private:
    Adafruit_Fingerprint* _finger;
    uint16_t _capacity;
    uint16_t _enrolledCount;
    bool* _slotMap;
    
    bool _validateID(uint16_t id) const;
};

#endif // UF_MEMORYMANAGER_H