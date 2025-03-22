#include "DebugMessenger.h"

#ifdef _DEBUG
#include "Utils.h"
#include "Logger.h"

#include <stdexcept>

static VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsMessengerCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    VkDebugUtilsMessengerCallbackDataEXT const* callbackData,
    void* userData);

void DebugMessenger::create(Context& context)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context.instance, "vkCreateDebugUtilsMessengerEXT");
    if (func) {
        VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugUtilsMessengerCallback;
        createInfo.pUserData = nullptr;

        THROW_IF_FAILED(func(context.instance, &createInfo, nullptr, &messenger), __FILE__, __LINE__, "Failed to create debug messenger");
        LOG_DEBUG("Debug messenger successfully created");
    } else {
        LOG_ERROR("Failed to load vkCreateDebugUtilsMessengerEXT");
        throw std::runtime_error("Failed to load vkCreateDebugUtilsMessengerEXT");
    }
}

void DebugMessenger::destroy(Context& context)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context.instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func) {
        func(context.instance, messenger, nullptr);
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
