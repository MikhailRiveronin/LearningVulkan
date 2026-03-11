#pragma once

#include "vk_mem_alloc.h"

class Render_System
{
public:
    struct
    {
        VmaAllocator allocator;
    } context;

    static Render_System* get_instance();


private:

};
