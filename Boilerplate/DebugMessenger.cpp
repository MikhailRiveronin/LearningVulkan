#include "Utils.h"
#include "DebugMessenger.h"
#include "Logger.h"

#include <stdexcept>

#ifdef _DEBUG
static VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsMessengerCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    VkDebugUtilsMessengerCallbackDataEXT const* callbackData,
    void* userData);

void DebugMessenger::initCreateInfo(Globals& globals)
{
    globals.debugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    globals.debugMessengerCreateInfo.pNext = nullptr;
    globals.debugMessengerCreateInfo.flags = 0;
    globals.debugMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    globals.debugMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    globals.debugMessengerCreateInfo.pfnUserCallback = debugUtilsMessengerCallback;
    globals.debugMessengerCreateInfo.pUserData = nullptr;
}

void DebugMessenger::create(Globals const& globals)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(globals.instance, "vkCreateDebugUtilsMessengerEXT");
    if (func) {
        THROW_IF_FAILED(
            func(globals.instance, &globals.debugMessengerCreateInfo, globals.allocator, &debugUtilsMessenger),
            __FILE__, __LINE__,
            "Failed to create debug messenger");
        LOG_DEBUG("Debug messenger successfully created");
    } else {
        LOG_ERROR("Failed to load vkCreateDebugUtilsMessengerEXT");
        throw std::runtime_error("Failed to load vkCreateDebugUtilsMessengerEXT");
    }
}

void DebugMessenger::destroy(Globals const& globals)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(globals.instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func) {
        func(globals.instance, debugUtilsMessenger, globals.allocator);
        LOG_DEBUG("Debug messenger destroyed");
    } else {
        LOG_ERROR("Failed to load vkDestroyDebugUtilsMessengerEXT");
        throw std::runtime_error("Failed to load vkDestroyDebugUtilsMessengerEXT");
    }
}

VkBool32 debugUtilsMessengerCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    VkDebugUtilsMessengerCallbackDataEXT const* callbackData,
    void* userData)
{
    LOG_DEBUG(callbackData->pMessage);

    return VK_FALSE;
}
#endif
