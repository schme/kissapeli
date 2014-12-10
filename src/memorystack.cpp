#include "memorystack.h"

void increaseStackPointer(MemoryStack* memory, uint64 bytes) {
    memory->top = (char*)memory->top + bytes;
}


void decreaseStackPointer(MemoryStack* memory, uint64 bytes) {
    if( (char*)memory->top - bytes < memory->memoryPool) {
        memory->top = memory->memoryPool;
        return;
    }
    memory->top = (char*)memory->top - bytes;
}


void* popMemoryStack(MemoryStack *memory, uint64 bytes) {
    if( (char*)memory->top + bytes >=
        (char*)memory->memoryPool + memory->stackSize) {
        return 0;
    }
    void* p = memory->top;
    increaseStackPointer( memory, bytes);
    return p;
}


void pushMemoryStack(MemoryStack* memory, uint64 bytes) {
    assert( (char*)memory->top - bytes >= memory->memoryPool && "Invalid MemoryStack free\n");

    memory->top = (char*)memory->top - bytes;
}
