#pragma once

#include "defines.h"
#include "vk_mem_alloc.h"

class Device
{
public:
    VkInstance instance;
    VkPhysicalDevice physical_device;
    VkDevice handle;
    VmaAllocator allocator;

    std::vector<VkQueueFamilyProperties> queue_family_properties;

    struct
    {
        struct
        {
            VkQueue handle;
            std::optional<u32> family_index;
        } graphics;

        struct
        {
            VkQueue handle;
            std::optional<u32> family_index;
        } compute;

        struct
        {
            VkQueue handle;
            std::optional<u32> family_index;
        } transfer;

        struct
        {
            VkQueue handle;
            std::optional<u32> family_index;
        } present;
    } queues;








    Device();

    void create(Context& globals);
    void destroy(Context& globals);

private:
    VkDebugUtilsMessengerEXT debug_utils_messenger = VK_NULL_HANDLE;





    VkPhysicalDeviceProperties physical_device_properties;
    VkPhysicalDeviceFeatures physical_device_features;
    VkPhysicalDeviceMemoryProperties physical_device_memory_properties;

};
