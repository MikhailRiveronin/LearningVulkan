#pragma once

#include "Boilerplate/Defines.h"

class Allocator {
public:
    virtual ~Allocator() {}

    virtual void init(u32 maxSize) = 0;
    virtual void destroy() = 0;

    virtual void* allocate(u32 size, u32 alignment = sizeof(void*)) = 0;
    virtual void deallocate(void* ptr) = 0;

protected:
    u8* memory;
    u32 maxSize;
};

class LinearAllocator : public Allocator {
public:
    void init(u32 maxSize) override;
    void destroy() override;

    void* allocate(u32 size, u32 alignment) override;
    void deallocate(void* ptr) override;

private:
    u32 offset;
};
