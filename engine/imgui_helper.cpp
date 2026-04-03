#include "imgui_helper.h"

#include "third_party/imgui/backends/imgui.h"
#include "third_party/imgui/backends/imgui_impl_win32.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "third_party/imgui/backends/imgui_impl_vulkan.h"

ImGui_Helper::ImGui_Helper(Vulkan_Context const& context) : m_context(context)
{
    
}
