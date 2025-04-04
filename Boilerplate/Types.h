#pragma once

#include "Defines.h"

#include <vulkan/vulkan.h>

#include <optional>
#include <vector>

struct Buffer {
    VkBuffer handle;
    VkDeviceSize size = 0;
    VkBufferUsageFlags usage = 0;
    VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    u32 queueFamilyIndexCount = 0;
    u32* queueFamilyIndices = nullptr;

    VkDeviceMemory memory;
    VkDeviceSize memoryOffset = 0;

    VkMemoryPropertyFlags memoryProperties = 0;
    VkDescriptorSet descriptorSet;
    VkDescriptorBufferInfo descriptorBufferInfo = {};
    void* mapped = nullptr;
};

struct Image {
    VkImage handle = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkImageView view = VK_NULL_HANDLE;

    VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
    VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
    VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
    VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
    VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT;

    u32 width = 0;
    u32 height = 0;
    u32 mipLevels = 1;
    u32 arrayLayers = 1;

    VkImageUsageFlags usage;
    VkMemoryPropertyFlags memoryProperties;

    u32 queueCount = 0;
    u32* queueIndices = nullptr;
};

struct Sampler {
    VkSampler handle = VK_NULL_HANDLE;

    VkFilter magFilter = VK_FILTER_LINEAR;
    VkFilter minFilter = VK_FILTER_LINEAR;
    VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    VkSamplerAddressMode addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    VkSamplerAddressMode addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
};

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

struct Context {
    VkAllocationCallbacks* allocator = nullptr;
    VkInstance instance;
    VkSurfaceKHR surface;
    VkRenderPass renderPass;

    struct {
        VkDevice handle;
        VkPhysicalDevice physicalDevice;
        QueueFamilyIndices queueFamilyIndices;

        struct {
            VkQueue graphics;
            VkQueue compute;
            VkQueue transfer;
            VkQueue present;
        } queues;
    } device;

    struct {
        SwapchainSupport support;
        VkSwapchainKHR swapchain;
        std::vector<VkImage> images;
        std::vector<VkImageView> imageViews;
        std::vector<VkFramebuffer> framebuffers;
        VkFormat format;
        VkExtent2D extent;
    } swapchain;

    struct {
        VkPipelineLayout layout;
        VkPipeline pipeline;
        VkDescriptorSetLayout descriptorSetLayout;
    } graphicsPipeline;

    struct {
        VkCommandPool pool;
        std::vector<VkCommandBuffer> buffers;
    } commandBuffer;

    struct {
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> fences;
    } synchronization;

    std::vector<Buffer> uniformBuffers;

    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;
};
