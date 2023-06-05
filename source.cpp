#include <iostream>
#include <fstream>

struct MemoryBlock {
    int blockId;
    int size;
    MemoryBlock* next;
};

struct Allocation {
    int requestId;
    int blockId;
    int size;
    Allocation* next;
};

MemoryBlock* freeMemoryList = nullptr;
Allocation* allocationList = nullptr;

void insertFreeMemoryBlock(MemoryBlock*& list, MemoryBlock* block) {
    if (list == nullptr || block->size < list->size) {
        block->next = list;
        list = block;
    }
    else {
        MemoryBlock* current = list;
        while (current->next != nullptr && current->next->size < block->size) {
            current = current->next;
        }
        block->next = current->next;
        current->next = block;
    }
}

MemoryBlock* findBestFitBlock(MemoryBlock* list, int size) {
    MemoryBlock* bestFit = nullptr;
    MemoryBlock* current = list;
    while (current != nullptr) {
        if (current->size >= size && (bestFit == nullptr || current->size < bestFit->size)) {
            bestFit = current;
        }
        current = current->next;
    }
    return bestFit;
}

MemoryBlock* removeFreeMemoryBlock(MemoryBlock*& list, MemoryBlock* block) {
    if (list == block) {
        list = block->next;
    }
    else {
        MemoryBlock* current = list;
        while (current->next != block) {
            current = current->next;
        }
        current->next = block->next;
    }
    block->next = nullptr;
    return block;
}

void allocateMemory(int requestId, int size) {
    MemoryBlock* block = findBestFitBlock(freeMemoryList, size);
    if (block != nullptr) {
        removeFreeMemoryBlock(freeMemoryList, block);
        Allocation* allocation = new Allocation;
        allocation->requestId = requestId;
        allocation->blockId = block->blockId;
        allocation->size = size;
        allocation->next = allocationList;
        allocationList = allocation;
        std::cout << size << " bytes have been allocated at block " << block->blockId << " for request " << requestId << ".\n";
        if (block->size > size) {
            MemoryBlock* remainingBlock = new MemoryBlock;
            remainingBlock->blockId = block->blockId + 1;
            remainingBlock->size = block->size - size;
            insertFreeMemoryBlock(freeMemoryList, remainingBlock);
        }
        delete block;
    }
    else {
        std::cout << "Unable to allocate memory for request " << requestId << ".\n";
    }
}

Allocation* findAllocation(int requestId) {
    Allocation* current = allocationList;
    while (current != nullptr) {
        if (current->requestId == requestId) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

void releaseMemory(int requestId) {
    Allocation* allocation = findAllocation(requestId);
    if (allocation != nullptr) {
        MemoryBlock* block = new MemoryBlock;
        block->blockId = allocation->blockId;
        block->size = allocation->size;
        insertFreeMemoryBlock(freeMemoryList, block);
        std::cout << allocation->size << " bytes have been returned back to block " << allocation->blockId << " for request " << requestId << ".\n";
        Allocation* previous = nullptr;
        Allocation* current = allocationList;
        while (current != nullptr && current != allocation) {
            previous = current;
            current = current->next;
        }
        if (previous == nullptr) {
            allocationList = current->next;
        }
        else {
            previous->next = current->next;
        }
        delete current;
    }
    else {
        std::cout << "Memory block not found for request " << requestId << ".\n";
    }
}

void processRequests(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Failed to open file: " << filename << "\n";
        return;
    }

    std::string request;
    int requestId = 1;

    while (file >> request) {
        if (request == "A") {
            int requestId, size;
            file >> requestId >> size;
            allocateMemory(requestId, size);
        }
        else if (request == "R") {
            int requestId;
            file >> requestId;
            releaseMemory(requestId);
        }
        else {
            std::cout << "Invalid request: " << request << "\n";
        }
    }

    file.close();
}

int main() {
    // Create the initial free memory linked list
    freeMemoryList = new MemoryBlock;
    freeMemoryList->blockId = 1;
    freeMemoryList->size = 1024;
    freeMemoryList->next = nullptr;
    MemoryBlock* current = freeMemoryList;
    for (int i = 2; i <= 1024; i++) {
        current->next = new MemoryBlock;
        current = current->next;
        current->blockId = i;
        current->size = 1024;
        current->next = nullptr;
    }

    // Process requests from the file
    processRequests("requests-1.txt");

    // Clean up allocated memory
    while (freeMemoryList != nullptr) {
        MemoryBlock* current = freeMemoryList;
        freeMemoryList = freeMemoryList->next;
        delete current;
    }

    while (allocationList != nullptr) {
        Allocation* current = allocationList;
        allocationList = allocationList->next;
        delete current;
    }

}





