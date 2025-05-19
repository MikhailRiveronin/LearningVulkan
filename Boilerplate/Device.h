#pragma once

#include "Structures.h"

#include <vulkan/vulkan.h>

class Device {
public:
    void create(Globals& globals);
    void destroy(Globals& globals);

    VkPhysicalDevice physicalDevice;
private:

    void findPhysicalDevice(Globals& globals);
};
