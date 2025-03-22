#include "Device.h"

#include "Defines.h"
#include "Logger.h"
#include "Types.h"
#include "Utils.h"

#include <set>
#include <vector>

static bool isPhysicalDeviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
static void queryQueueFamilyIndices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, QueueFamilyIndices& queueFamilyIndices);
static void querySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface, SwapchainSupport& swapchainSupport);
static void checkExtensionSupport(VkPhysicalDevice physicalDevice, std::vector<char const*> const& requiredExtensions);

void Device::create(Context& context)
{
    selectPhysicalDevice(context);

    std::set<u32> uniqueQueueFamilyIndices = { context.device.queueFamilyIndices.graphics.value(), context.device.queueFamilyIndices.present.value() };
    float queuePriority = 1.f;
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    for (auto queueFamilyIndex : uniqueQueueFamilyIndices)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.pNext = nullptr;
        queueCreateInfo.flags = 0;
        queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(std::move(queueCreateInfo));
    }

    std::vector<char const*> requiredExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    checkExtensionSupport(physicalDevice, requiredExtensions);

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = nullptr;
    deviceCreateInfo.flags = 0;
    deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = nullptr;
    deviceCreateInfo.enabledExtensionCount = requiredExtensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();
    deviceCreateInfo.pEnabledFeatures = nullptr;

    THROW_IF_FAILED(vkCreateDevice(physicalDevice, &deviceCreateInfo, context.allocator, &context.device.device), __FILE__, __LINE__, "Failed to create device");
    LOG_INFO("Device successfully created");

    vkGetDeviceQueue(context.device.device, context.device.queueFamilyIndices.graphics.value(), 0, &context.device.queues.graphics);
    // vkGetDeviceQueue(context.device.device, context.device.queueFamilyIndices.transfer.value(), 0, &context.device.queues.transfer);
    vkGetDeviceQueue(context.device.device, context.device.queueFamilyIndices.present.value(), 0, &context.device.queues.present);
}

void Device::destroy(Context& context)
{
    vkDestroyDevice(context.device.device, context.allocator);
    LOG_DEBUG("Device destroyed");
}

void Device::selectPhysicalDevice(Context& context)
{
    u32 deviceCount = 0;
    THROW_IF_FAILED(vkEnumeratePhysicalDevices(context.instance, &deviceCount, nullptr), __FILE__, __LINE__, "No Vulkan devices found");
    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    THROW_IF_FAILED(vkEnumeratePhysicalDevices(context.instance, &deviceCount, physicalDevices.data()), __FILE__, __LINE__, "No Vulkan devices found");

    for (u32 i = 0; i < deviceCount; ++i) {
        if (isPhysicalDeviceSuitable(physicalDevices[i], context.surface)) {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(physicalDevices[i], &properties);

            LOG_INFO("Suitable physical device found: %s", properties.deviceName);

            // VkPhysicalDeviceFeatures features;
            // vkGetPhysicalDeviceFeatures(physicalDevice, &features);
        
            // VkPhysicalDeviceMemoryProperties memoryProperties;
            // vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

            physicalDevice = physicalDevices[i];
            queryQueueFamilyIndices(physicalDevice, context.surface, context.device.queueFamilyIndices);
            querySwapchainSupport(physicalDevice, context.surface, context.swapchain.support);

            char buffer[1024];
            char const* indent = "                      ";
            sprintf(buffer, "Queue family indices: graphics: %u\n", context.device.queueFamilyIndices.graphics.value());
            sprintf(buffer + strlen(buffer), "%s%scompute: %u\n", infoIndent, indent, context.device.queueFamilyIndices.compute.value());
            sprintf(buffer + strlen(buffer), "%s%stransfer: %u\n", infoIndent, indent, context.device.queueFamilyIndices.transfer.value());
            sprintf(buffer + strlen(buffer), "%s%spresent: %u\n", infoIndent, indent, context.device.queueFamilyIndices.present.value());
            LOG_INFO(buffer);

            break;
        }
    }
}

bool isPhysicalDeviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);

    QueueFamilyIndices queueFamilyIndices;
    queryQueueFamilyIndices(physicalDevice, surface, queueFamilyIndices);
    if (!queueFamilyIndices.graphics.has_value()
        || !queueFamilyIndices.compute.has_value()
        || !queueFamilyIndices.transfer.has_value()
        || !queueFamilyIndices.present.has_value()) {
        LOG_INFO("Required queue families for '%s' not found. Skipping...", properties.deviceName);
        return false;
    }

    SwapchainSupport swapchainSupport;
    querySwapchainSupport(physicalDevice, surface, swapchainSupport);
    if (swapchainSupport.formats.empty() || swapchainSupport.presentModes.empty()) {
        LOG_INFO("Can't create required swapchain for '%s'. Skipping...", properties.deviceName);
        return false;
    }

    return true;
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
        THROW_IF_FAILED(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &isPresentSupported), __FILE__, __LINE__, "Failed to get physical device surface support");
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
    THROW_IF_FAILED(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &swapchainSupport.capabilities), __FILE__, __LINE__, "Failed to get physical device surface capabilities");

    uint32_t formatCount;
    THROW_IF_FAILED(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr), __FILE__, __LINE__, "Failed to get physical device surface formats");
    swapchainSupport.formats.resize(formatCount);
    THROW_IF_FAILED(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, swapchainSupport.formats.data()), __FILE__, __LINE__, "Failed to get physical device surface formats");

    uint32_t modeCount;
    THROW_IF_FAILED(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &modeCount, nullptr), __FILE__, __LINE__, "Failed to get physical device surface present modes");
    swapchainSupport.presentModes.resize(modeCount);
    THROW_IF_FAILED(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &modeCount, swapchainSupport.presentModes.data()), __FILE__, __LINE__, "Failed to get physical device surface present modes");
}

void checkExtensionSupport(VkPhysicalDevice physicalDevice, std::vector<char const*> const& requiredExtensions)
{
    char buffer[4096];
    sprintf(buffer, "Required device level extensions: ");
    char const* indent = "                                  ";
    for (u32 i = 0; i < requiredExtensions.size(); ++i) {
        if (i == 0) {
            sprintf(buffer + strlen(buffer), "%s\n", requiredExtensions[i]);
            continue;
        }

        sprintf(buffer + strlen(buffer), "%s%s%s\n", infoIndent, indent, requiredExtensions[i]);
    }
    LOG_INFO(buffer);

    u32 availableExtensionCount;
    THROW_IF_FAILED(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &availableExtensionCount, nullptr), __FILE__, __LINE__, "Failed to enumerate device extension properties");
    std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
    THROW_IF_FAILED(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &availableExtensionCount, availableExtensions.data()), __FILE__, __LINE__, "Failed to enumerate device extension properties");

    //sprintf(buffer, "Available device level extensions: ");
    //indent = "                                   ";
    //for (u32 i = 0; i < availableExtensionCount; ++i) {
    //    if (i == 0) {
    //        sprintf(buffer + strlen(buffer), "%s\n", availableExtensions[i].extensionName);
    //        continue;
    //    }

    //    sprintf(buffer + strlen(buffer), "%s%s%s\n", infoIndent, indent, availableExtensions[i].extensionName);
    //}
    //LOG_INFO(buffer);

    for (auto& extension : requiredExtensions) {
        if (std::find_if(availableExtensions.begin(), availableExtensions.end(), [&](const VkExtensionProperties& extensionProperties) {
            return strcmp(extension, extensionProperties.extensionName) == 0;
        }) == availableExtensions.end()) {
            LOG_ERROR("Extension '%s' is not supported", extension);
            throw std::runtime_error("Required extension is not supported");
        }
    }
}
