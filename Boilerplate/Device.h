#pragma once

#include "Types.h"

#include <vulkan/vulkan.h>

#include <optional>

class Device {
public:
    void create(Context& context);
    void destroy(Context& context);

private:
    VkPhysicalDevice physicalDevice;

    void selectPhysicalDevice(Context& context);
};
