#pragma once

#include "Structures.h"

#include <vulkan/vulkan.h>

class Device {
public:
    void create(Globals& globals);
    void destroy(Globals& globals);

private:
    VkPhysicalDevice physicalDevice;

    void findPhysicalDevice(Globals& globals);
};
