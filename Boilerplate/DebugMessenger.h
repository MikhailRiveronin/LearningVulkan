#pragma once

#ifdef _DEBUG
#include "Types.h"

#include <vulkan/vulkan.h>

class DebugMessenger {
public:
    void create(Context& context);
    void destroy(Context& context);

private:
    VkDebugUtilsMessengerEXT messenger;
};
#endif
