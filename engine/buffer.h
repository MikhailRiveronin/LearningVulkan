#pragma once

#include "defines.h"
#include "vk_mem_alloc.h"

class Buffer
{
public:
    virtual ~Buffer();

protected:
    VkBuffer handle = VK_NULL_HANDLE;
    VmaAllocation allocation = VK_NULL_HANDLE;
};

using Buffer_Shared_Ptr = std::shared_ptr<Buffer>;
