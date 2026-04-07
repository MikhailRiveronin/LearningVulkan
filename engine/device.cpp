#include "device.h"

#include "logger.h"
#include "vulkan_struct_initializers.h"

namespace engine
{

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_utils_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, VkDebugUtilsMessengerCallbackDataEXT const* callback_data, void* user_data);

Device::Device()
{
    auto debug_messenger_create_info = Vulkan_Struct_Initializers::debug_utils_messenger_create_info(debug_utils_messenger_callback);
    auto app_info = Vulkan_Struct_Initializers::application_info();

    std::vector<char const*> required_layers;
    required_layers.push_back("VK_LAYER_KHRONOS_validation");

    u32 available_layer_count;
    VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, nullptr));
    std::vector<VkLayerProperties> available_layers(available_layer_count);
    VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, available_layers.data()));

    for (auto layer : required_layers)
    {
        if (std::find_if(available_layers.begin(), available_layers.end(), [&](VkLayerProperties const& layer_properties)
            {
                return strcmp(layer, layer_properties.layerName) == 0;
            }) == available_layers.end())
        {
            std::string error = "Required layer " + std::string(layer) + " not supported";
            LOG_ERROR(error.c_str());
            throw std::runtime_error(error);
        }
    }

    std::vector<char const*> required_instance_extensions;
    required_instance_extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    required_instance_extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
    required_instance_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    u32 available_extension_count;
    VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &available_extension_count, nullptr));
    std::vector<VkExtensionProperties> available_instance_extensions(available_extension_count);
    VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &available_extension_count, available_instance_extensions.data()));

    for (auto extension : required_instance_extensions)
    {
        if (std::find_if(available_instance_extensions.begin(), available_instance_extensions.end(), [&](const VkExtensionProperties& extension_properties)
            {
                return strcmp(extension, extension_properties.extensionName) == 0;
            }) == available_instance_extensions.end())
        {
            std::string error = "Required extension " + std::string(extension) + " not supported";
            LOG_ERROR(error.c_str());
            throw std::runtime_error(error);
        }
    }

    auto instance_create_info = Vulkan_Struct_Initializers::instance_create_info(&app_info, required_layers, required_instance_extensions, &debug_messenger_create_info);
    VK_CHECK(vkCreateInstance(&instance_create_info, nullptr, &instance));

    auto create_debug_utils_messenger_proc = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (create_debug_utils_messenger_proc)
    {
        VK_CHECK(create_debug_utils_messenger_proc(instance, &debug_messenger_create_info, nullptr, &debug_utils_messenger));
    }
    else
    {
        std::string error = "Failed to get vkCreateDebugUtilsMessengerEXT proc address";
        LOG_ERROR(error.c_str());
        throw std::runtime_error(error);
    }

    u32 physical_device_count;
    VK_CHECK(vkEnumeratePhysicalDevices(instance, &physical_device_count, nullptr));
    std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
    VK_CHECK(vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_devices.data()));

    physical_device = physical_devices[0];

    u32 queue_family_property_count;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_property_count, nullptr);
    queue_family_properties.resize(queue_family_property_count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_property_count, queue_family_properties.data());

    for (u32 i = 0; i < queue_family_property_count; ++i)
    {
        if (queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            queue_family_indices.graphics = i;
        }

        if (queue_family_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
        {
            queue_family_indices.compute = i;
        }

        if (queue_family_properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
        {
            queue_family_indices.transfer = i;
        }

        if (queue_family_indices.graphics.has_value() && queue_family_indices.compute.has_value() && queue_family_indices.transfer.has_value())
        {
            break;
        }
    }

    

    

    vkGetPhysicalDeviceProperties(physical_device, &physical_device_properties);
    vkGetPhysicalDeviceFeatures(physical_device, &physical_device_features);
    vkGetPhysicalDeviceMemoryProperties(physical_device, &physical_device_memory_properties);

    depth_stencil_buffer_format = VK_FORMAT_D24_UNORM_S8_UINT;

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::set<u32> unique_queue_family_indices = { queue_family_indices.graphics.value(), queue_family_indices.present.value() };
    for (auto index : unique_queue_family_indices)
    {
        auto queue_create_info = Vulkan_Struct_Initializers::device_queue_create_info(index);
        queue_create_infos.push_back(queue_create_info);
    }

    
}

VkBool32 debug_utils_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, VkDebugUtilsMessengerCallbackDataEXT const* callback_data, void* user_data)
{
    switch (message_severity)
    {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            LOG_ERROR(callback_data->pMessage);
            break;

        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            LOG_WARNING(callback_data->pMessage);
            break;

        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            LOG_INFO(callback_data->pMessage);
            break;

        default:
            break;
    }

    return VK_FALSE;
}






VkCommandPool Device::create_command_pool(VkCommandPoolCreateInfo const& create_info)
{
    VkCommandPool command_pool;
    VK_CHECK(vkCreateCommandPool(handle, &create_info, &allocation_callbacks, &command_pool));
    return command_pool;
}

void Device::destroy_command_pool(VkCommandPool command_pool)
{
    vkDestroyCommandPool(handle, command_pool, &allocation_callbacks);
}

VkCommandBuffer Device::allocate_command_buffer(VkCommandBufferAllocateInfo const& allocate_info)
{
    VkCommandBuffer command_buffer;
    VK_CHECK(vkAllocateCommandBuffers(handle, &allocate_info, &command_buffer));
    return command_buffer;
}

void Device::free_command_buffer(VkCommandPool command_pool, VkCommandBuffer command_buffer)
{
    vkFreeCommandBuffers(handle, command_pool, 1, &command_buffer);
}










#include "Utils.h"
#include "Device.h"
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

void Context::create(Context& globals)
{
    findPhysicalDevice(globals);

    std::set<u32> uniqueQueueFamilyIndices = { globals.device.queues.graphics.familyIndex, globals.device.queues.present.familyIndex };
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

    VK_CHECK(
        vkCreateDevice(physicalDevice, &createInfo, globals.allocator, &globals.device.handle),
        __FILE__, __LINE__,
        "Failed to create device");
    LOG_INFO("Device successfully created");

    vkGetDeviceQueue(globals.device.handle, globals.device.queues.graphics.familyIndex, 0, &globals.device.queues.graphics.handle);
    vkGetDeviceQueue(globals.device.handle, globals.device.queues.present.familyIndex, 0, &globals.device.queues.present.handle);
}

void Context::destroy(Context& globals)
{
    vkDestroyDevice(globals.device.handle, globals.allocator);
    LOG_DEBUG("Device destroyed");
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
    VK_CHECK(
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &availableExtensionCount, nullptr),
        __FILE__, __LINE__,
        "Failed to enumerate device extension properties");
    std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
    VK_CHECK(
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

} // namespace engine
