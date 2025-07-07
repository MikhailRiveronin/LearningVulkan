#pragma once

#include "Structures.h"

#include <vulkan/vulkan.h>

class Device {
public:
    void create(Context& globals);
    void destroy(Context& globals);

    VkPhysicalDevice physicalDevice;
private:

    void findPhysicalDevice(Context& globals);
};
