#pragma once

#include "defines.h"

#include "vulkan_classes.h"

class ImGui_Helper
{
public:
    ImGui_Helper(Vulkan_Context const& context);

private:
    Vulkan_Context& m_context;

};
