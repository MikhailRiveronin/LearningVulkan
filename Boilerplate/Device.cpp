#include "Device.h"
#include "Utils.h"
#include "Logger.h"

#include <optional>
#include <set>
#include <vector>

struct QueueFamilyIndices {
    std::optional<u32> graphics;
    std::optional<u32> compute;
    std::optional<u32> transfer;
    std::optional<u32> present;
};

struct SwapchainSupport {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

static void queryQueueFamilyIndices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, QueueFamilyIndices& queueFamilyIndices);
static void querySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface, SwapchainSupport& swapchainSupport);
static void initRequiredDeviceExtensions(std::vector<char const*>& requiredDeviceExtensions);
static void checkRequiredDeviceExtensionsSupport(VkPhysicalDevice physicalDevice, std::vector<char const*> const& requiredDeviceExtensions);
static VkSurfaceFormatKHR selectSwapchainFormat(std::vector<VkSurfaceFormatKHR> const& formats);
static VkPresentModeKHR selectSwapchainPresentMode(std::vector<VkPresentModeKHR> const& presentModes);
static VkFormat selectDepthStencilBufferFormat(VkPhysicalDevice physicalDevice);

void Device::create(Globals& globals)
{
    findPhysicalDevice(globals);

    std::set<u32> uniqueQueueFamilyIndices = { globals.device.queues.graphics.index, globals.device.queues.present.index };
    float queuePriority = 1.f;
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    for (auto index : uniqueQueueFamilyIndices)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.pNext = nullptr;
        queueCreateInfo.flags = 0;
        queueCreateInfo.queueFamilyIndex = index;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        queueCreateInfos.push_back(std::move(queueCreateInfo));
    }

    std::vector<char const*> requiredDeviceExtensions;
    initRequiredDeviceExtensions(requiredDeviceExtensions);
    checkRequiredDeviceExtensionsSupport(physicalDevice, requiredDeviceExtensions);

    VkPhysicalDeviceFeatures physicalDeviceFeatures = {};
    physicalDeviceFeatures.samplerAnisotropy = VK_TRUE;
    // physicalDeviceFeatures.shaderSampledImageArrayDynamicIndexing = VK_TRUE;

    VkPhysicalDeviceDescriptorIndexingFeatures physicalDeviceDescriptorIndexingFeatures = {};
    physicalDeviceDescriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
    physicalDeviceDescriptorIndexingFeatures.pNext = nullptr;
    physicalDeviceDescriptorIndexingFeatures.runtimeDescriptorArray = VK_TRUE;
    physicalDeviceDescriptorIndexingFeatures.descriptorBindingPartiallyBound = VK_TRUE;
    physicalDeviceDescriptorIndexingFeatures.descriptorBindingVariableDescriptorCount = VK_TRUE;

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = &physicalDeviceDescriptorIndexingFeatures;
    createInfo.flags = 0;
    createInfo.queueCreateInfoCount = queueCreateInfos.size();
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = nullptr;
    createInfo.enabledExtensionCount = requiredDeviceExtensions.size();
    createInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();
    createInfo.pEnabledFeatures = &physicalDeviceFeatures;

    THROW_IF_FAILED(
        vkCreateDevice(physicalDevice, &createInfo, globals.allocator, &globals.device.handle),
        __FILE__, __LINE__,
        "Failed to create device");
    LOG_INFO("Device successfully created");

    vkGetDeviceQueue(globals.device.handle, globals.device.queues.graphics.index, 0, &globals.device.queues.graphics.handle);
    vkGetDeviceQueue(globals.device.handle, globals.device.queues.present.index, 0, &globals.device.queues.present.handle);
}

void Device::destroy(Globals& globals)
{
    vkDestroyDevice(globals.device.handle, globals.allocator);
    LOG_DEBUG("Device destroyed");
}

void Device::findPhysicalDevice(Globals& globals)
{
    u32 deviceCount = 0;
    THROW_IF_FAILED(
        vkEnumeratePhysicalDevices(globals.instance, &deviceCount, nullptr),
        __FILE__, __LINE__,
        "No Vulkan devices found");
    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    THROW_IF_FAILED(
        vkEnumeratePhysicalDevices(globals.instance, &deviceCount, physicalDevices.data()),
        __FILE__, __LINE__,
        "No Vulkan devices found");

    bool found = false;
    for (u32 i = 0; i < deviceCount; ++i) {
        QueueFamilyIndices queueFamilyIndices;
        queryQueueFamilyIndices(physicalDevices[i], globals.surface, queueFamilyIndices);
        if (!queueFamilyIndices.graphics.has_value()
            || !queueFamilyIndices.compute.has_value()
            || !queueFamilyIndices.transfer.has_value()
            || !queueFamilyIndices.present.has_value()) {
            continue;
        }

        SwapchainSupport swapchainSupport;
        querySwapchainSupport(physicalDevices[i], globals.surface, swapchainSupport);
        if (swapchainSupport.formats.empty() || swapchainSupport.presentModes.empty()) {
            continue;
        }

        found = true;
        LOG_INFO("Suitable physical device found");

        physicalDevice = physicalDevices[i];

        vkGetPhysicalDeviceProperties(physicalDevice, &globals.device.support.properties);
        vkGetPhysicalDeviceFeatures(physicalDevice, &globals.device.support.features);
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &globals.device.support.memoryProperties);

        globals.device.queues.graphics.index = queueFamilyIndices.graphics.value();
        globals.device.queues.compute.index = queueFamilyIndices.compute.value();
        globals.device.queues.transfer.index = queueFamilyIndices.transfer.value();
        globals.device.queues.present.index = queueFamilyIndices.present.value();

        globals.swapchain.support.capabilities = swapchainSupport.capabilities;
        globals.swapchain.support.formats = swapchainSupport.formats;
        globals.swapchain.support.presentModes = swapchainSupport.presentModes;
        globals.swapchain.format = selectSwapchainFormat(globals.swapchain.support.formats);
        globals.swapchain.presentMode = selectSwapchainPresentMode(globals.swapchain.support.presentModes);

        globals.swapchain.depthStencilBuffer.format = selectDepthStencilBufferFormat(physicalDevice);

        break;
    }

    if (!found) {
        throw std::runtime_error("Failed to find suitable physical device");
    }
}

void queryQueueFamilyIndices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, QueueFamilyIndices& queueFamilyIndices)
{
    u32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamiliesProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamiliesProperties.data());

    for (u32 i = 0; i < queueFamilyCount; ++i) {
        if (queueFamiliesProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            queueFamilyIndices.graphics = i;
            break;
        }
    }

    for (u32 i = 0; i < queueFamilyCount; ++i) {
        if (queueFamiliesProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            if (queueFamilyIndices.graphics.has_value() && (i != queueFamilyIndices.graphics)) {
                queueFamilyIndices.compute = i;
                break;
            }

            queueFamilyIndices.compute = i;
        }
    }

    for (u32 i = 0; i < queueFamilyCount; ++i) {
        if (queueFamiliesProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            if ((queueFamilyIndices.graphics.has_value() && (i != queueFamilyIndices.graphics))
                && (queueFamilyIndices.compute.has_value() && (i != queueFamilyIndices.compute))) {
                queueFamilyIndices.transfer = i;
                break;
            }

            queueFamilyIndices.transfer = i;
        }
    }

    VkBool32 isPresentSupported = VK_FALSE;
    for (u32 i = 0; i < queueFamilyCount; ++i) {
        THROW_IF_FAILED(
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &isPresentSupported),
            __FILE__, __LINE__,
            "Failed to get physical device surface support");
        if (isPresentSupported) {
            if ((queueFamilyIndices.graphics.has_value() && (i != queueFamilyIndices.graphics))
                && (queueFamilyIndices.compute.has_value() && (i != queueFamilyIndices.compute))
                && (queueFamilyIndices.transfer.has_value() && (i != queueFamilyIndices.transfer))) {
                queueFamilyIndices.present = i;
                break;
            }

            queueFamilyIndices.present = i;
        }
    }
}

void querySwapchainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, SwapchainSupport& swapchainSupport)
{
    THROW_IF_FAILED(
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &swapchainSupport.capabilities),
        __FILE__, __LINE__,
        "Failed to get physical device surface capabilities");

    uint32_t formatCount;
    THROW_IF_FAILED(
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr),
        __FILE__, __LINE__,
        "Failed to get physical device surface formats");
    swapchainSupport.formats.resize(formatCount);
    THROW_IF_FAILED(
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, swapchainSupport.formats.data()),
        __FILE__, __LINE__,
        "Failed to get physical device surface formats");

    uint32_t modeCount;
    THROW_IF_FAILED(
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &modeCount, nullptr),
        __FILE__, __LINE__,
        "Failed to get physical device surface present modes");
    swapchainSupport.presentModes.resize(modeCount);
    THROW_IF_FAILED(
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &modeCount, swapchainSupport.presentModes.data()),
        __FILE__, __LINE__,
        "Failed to get physical device surface present modes");
}

void initRequiredDeviceExtensions(std::vector<char const*>& requiredDeviceExtensions)
{
    requiredDeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

void checkRequiredDeviceExtensionsSupport(VkPhysicalDevice physicalDevice, std::vector<char const*> const& requiredDeviceExtensions)
{
    char buffer[16384];
    sprintf(buffer, "Required device level extensions: ");
    char const* indent = "                                  ";
    for (u32 i = 0; i < requiredDeviceExtensions.size(); ++i) {
        if (i == 0) {
            sprintf(buffer + strlen(buffer), "%s\n", requiredDeviceExtensions[i]);
            continue;
        }

        sprintf(buffer + strlen(buffer), "%s%s%s\n", infoIndent, indent, requiredDeviceExtensions[i]);
    }
    LOG_INFO(buffer);

    u32 availableExtensionCount;
    THROW_IF_FAILED(
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &availableExtensionCount, nullptr),
        __FILE__, __LINE__,
        "Failed to enumerate device extension properties");
    std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
    THROW_IF_FAILED(
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &availableExtensionCount, availableExtensions.data()),
        __FILE__, __LINE__,
        "Failed to enumerate device extension properties");

    sprintf(buffer, "Available device level extensions: ");
    indent = "                                   ";
    for (u32 i = 0; i < availableExtensionCount; ++i) {
       if (i == 0) {
           sprintf(buffer + strlen(buffer), "%s\n", availableExtensions[i].extensionName);
           continue;
       }

       sprintf(buffer + strlen(buffer), "%s%s%s\n", infoIndent, indent, availableExtensions[i].extensionName);
    }
    LOG_INFO(buffer);

    for (auto& extension : requiredDeviceExtensions) {
        if (std::find_if(availableExtensions.begin(), availableExtensions.end(), [&](const VkExtensionProperties& extensionProperties) {
            return strcmp(extension, extensionProperties.extensionName) == 0;
        }) == availableExtensions.end()) {
            LOG_ERROR("Extension '%s' is not supported", extension);
            throw std::runtime_error("Required extension is not supported");
        }
    }
}

VkSurfaceFormatKHR selectSwapchainFormat(std::vector<VkSurfaceFormatKHR> const& formats)
{
    for (auto& format : formats) {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return format;
        }
    }

    return formats[0];
}

VkPresentModeKHR selectSwapchainPresentMode(const std::vector<VkPresentModeKHR>& presentModes)
{
    for (auto& presentMode : presentModes) {
        if (presentMode == VK_PRESENT_MODE_FIFO_KHR) {
            return presentMode;
        }
    }

    return presentModes[0];
}

VkFormat selectDepthStencilBufferFormat(VkPhysicalDevice physicalDevice)
{
    std::vector<VkFormat> candidates = { VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT };
    for (auto format : candidates) {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &properties);

        if (properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            return format;
        }
    }

    return candidates[0];
}
