#pragma once

#include "Types.h"

#include <vulkan/vulkan.h>

#include <optional>

class Device {
public:
    void create(Context& context);
    void destroy(Context& context);

private:
    void findPhysicalDevice(Context& context);
};
