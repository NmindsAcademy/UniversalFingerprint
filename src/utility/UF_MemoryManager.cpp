/**
 * @file UF_MemoryManager.cpp
 * @brief Implementation of memory management
 */

#include "UF_MemoryManager.h"
#include <cstring>

UF_MemoryManager::UF_MemoryManager(Adafruit_Fingerprint* finger, uint16_t capacity)
    : _finger(finger), _capacity(capacity), _enrolledCount(0), _slotMap(nullptr) {
}

UF_MemoryManager::~UF_MemoryManager() {
    if (_slotMap != nullptr) {
        delete[] _slotMap;
    }
}

bool UF_MemoryManager::init() {
    if (_capacity == 0) {
        return false;
    }
    
    _slotMap = new bool[_capacity];
    if (_slotMap == nullptr) {
        return false;
    }
    
    memset(_slotMap, 0, _capacity * sizeof(bool));
    
    // Initial scan
    return scanAllSlots();
}

bool UF_MemoryManager::isSlotOccupied(uint16_t id) {
    if (!_validateID(id) || _slotMap == nullptr) {
        return false;
    }
    
    return _slotMap[id - 1];
}

uint16_t UF_MemoryManager::findEmptySlot(uint16_t start) {
    if (_slotMap == nullptr || start < 1 || start > _capacity) {
        return 0;
    }
    
    for (uint16_t i = start - 1; i < _capacity; i++) {
        if (!_slotMap[i]) {
            return i + 1;
        }
    }
    
    return 0;
}

uint16_t UF_MemoryManager::findEmptySlots(uint16_t count, uint16_t* results, uint16_t start) {
    if (_slotMap == nullptr || count == 0 || results == nullptr) {
        return 0;
    }
    
    uint16_t found = 0;
    for (uint16_t i = start - 1; i < _capacity && found < count; i++) {
        if (!_slotMap[i]) {
            results[found] = i + 1;
            found++;
        }
    }
    
    return found;
}

bool UF_MemoryManager::updateSlot(uint16_t id, bool occupied) {
    if (!_validateID(id) || _slotMap == nullptr) {
        return false;
    }
    
    bool current = _slotMap[id - 1];
    _slotMap[id - 1] = occupied;
    
    // Update count
    if (current && !occupied) {
        // Became empty
        if (_enrolledCount > 0) {
            _enrolledCount--;
        }
    } else if (!current && occupied) {
        // Became occupied
        _enrolledCount++;
    }
    
    return true;
}

bool UF_MemoryManager::scanAllSlots() {
    if (_slotMap == nullptr || _finger == nullptr) {
        return false;
    }
    
    _enrolledCount = 0;
    
    for (uint16_t i = 0; i < _capacity; i++) {
        uint16_t id = i + 1;
        uint8_t result = _finger->getTemplateCount(id);
        
        if (result == FINGERPRINT_OK) {
            _slotMap[i] = true;
            _enrolledCount++;
        } else {
            _slotMap[i] = false;
        }
    }
    
    return true;
}

bool UF_MemoryManager::defragment() {
    // This is a simple defragmentation that moves templates to lower IDs
    // Note: This requires reading and rewriting templates, which may be slow
    
    bool success = true;
    uint16_t nextEmpty = findEmptySlot(1);
    
    for (uint16_t i = nextEmpty + 1; i <= _capacity; i++) {
        if (isSlotOccupied(i)) {
            // Found an occupied slot after an empty one
            // We should move this template to the empty slot
            // This requires raw template operations which are sensor-specific
            // For now, just report that defragmentation is needed
            success = false;
            break;
        }
    }
    
    return success;
}

bool UF_MemoryManager::getStatistics(uint16_t& total, uint16_t& occupied, uint16_t& free) {
    total = _capacity;
    occupied = _enrolledCount;
    free = _capacity - _enrolledCount;
    return true;
}

bool UF_MemoryManager::_validateID(uint16_t id) const {
    return (id >= 1 && id <= _capacity);
}