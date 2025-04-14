#pragma once

#include "Structures.h"

#include <vulkan/vulkan.h>

#ifdef _DEBUG
class DebugMessenger {
public:
    void initCreateInfo(Globals& globals);

    void create(Globals const& globals);
    void destroy(Globals const& globals);

private:
    VkDebugUtilsMessengerEXT debugUtilsMessenger;
};
#endif
