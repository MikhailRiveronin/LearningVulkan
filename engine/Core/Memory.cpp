#include "Memory.h"

#include <stdlib.h>

void LinearAllocator::init(u32 maxSize)
{
    memory = (u8*)malloc(maxSize);
    this->maxSize = maxSize;
    offset = 0;
}

void LinearAllocator::destroy()
{
    free(memory);
    offset = 0;
}

void* LinearAllocator::allocate(u32 size, u32 alignment)
{
    if ((maxSize - size) > 0) {
        offset += size;
        maxSize -= size;
        return (void*)memory[offset];
    }

    return nullptr;
}

void LinearAllocator::deallocate(void* ptr)
{
}
