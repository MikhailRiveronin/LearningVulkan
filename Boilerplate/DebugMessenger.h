#pragma once

#include "Structures.h"

#include <vulkan/vulkan.h>

#ifdef _DEBUG
class DebugMessenger {
public:
    void initCreateInfo(Context& globals);

    void create(Context const& globals);
    void destroy(Context const& globals);

private:
    VkDebugUtilsMessengerEXT debugUtilsMessenger;
};
#endif
