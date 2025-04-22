#pragma once

#include "Defines.h"
#include "ProceduralMeshes/Vertex.h"

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <optional>
#include <vector>

struct Buffer {
    VkBuffer handle = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;

    VkDeviceSize size = 0;
    VkBufferUsageFlags usage = 0;
    VkMemoryPropertyFlags memoryProperties = 0;

    VkDescriptorBufferInfo descriptor = {};
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

    VkDescriptorImageInfo descriptor = {};
};

struct Sampler {
    VkSampler handle = VK_NULL_HANDLE;

    VkFilter magFilter = VK_FILTER_LINEAR;
    VkFilter minFilter = VK_FILTER_LINEAR;
    VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    VkSamplerAddressMode addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    VkSamplerAddressMode addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
};

struct Globals {
    VkAllocationCallbacks* allocator;

#ifdef _DEBUG
    VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = {};
#endif
    VkInstance instance = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;

    struct {
        VkDevice handle = VK_NULL_HANDLE;

        struct {
            VkPhysicalDeviceProperties properties;
            VkPhysicalDeviceFeatures features;
            VkPhysicalDeviceMemoryProperties memoryProperties;
        } support;

        struct {
            struct {
                VkQueue handle = VK_NULL_HANDLE;
                u32 index;
            } graphics;
            
            struct {
                VkQueue handle = VK_NULL_HANDLE;
                u32 index;
            } compute;
            
            struct {
                VkQueue handle = VK_NULL_HANDLE;
                u32 index;
            } transfer;
            
            struct {
                VkQueue handle = VK_NULL_HANDLE;
                u32 index;
            } present;
        } queues;
    } device;

    struct {
        VkSwapchainKHR handle = VK_NULL_HANDLE;

        struct {
            VkSurfaceCapabilitiesKHR capabilities = {};
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
        } support;

        VkExtent2D extent = {};
        VkSurfaceFormatKHR format = {};
        VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;

        std::vector<VkImage> images;
        std::vector<VkImageView> imageViews;
        std::vector<VkFramebuffer> framebuffers;

        Image depthStencilBuffer;
    } swapchain;

    VkRenderPass renderPass;

    struct {
        VkCommandPool pool;
        std::vector<VkCommandBuffer> buffers;
    } graphicsCommandBuffer;

    struct {
        VkPipelineLayout layout;
        VkPipeline pipeline;
        VkDescriptorSetLayout descriptorSetLayout;
    } graphicsPipeline;

    struct {
        struct {
            std::vector<VkSemaphore> imageAcquired;
            std::vector<VkSemaphore> renderFinished;
        } semaphores;

        struct {
            std::vector<VkFence> previousFrameFinished;
        } fences;
    } synchronization;
};

struct RenderObject {
    struct {
        std::vector<Vertex> vertices;
        std::vector<u16> indices;
    } mesh;

    struct {
        u32 indexCount;
        u32 firstIndex;
        i32 vertexOffset;
    } drawArgs;

    glm::mat4 world = glm::mat4(1.f);
};

struct RenderLayer {
    VkPipeline pipeline;
    std::vector<RenderObject> renderObjects;
    std::vector<VkDescriptorSetLayout> setLayouts;
    std::vector<VkDescriptorSet> descriptorSets;
};

struct RenderPassConstants {
    u32 binding = 0;
    glm::mat4 world;
    glm::mat4 proj;
};



struct Mesh {
    std::vector<RenderObject> items;

    std::vector<Submesh> submeshes;


    Buffer vertexBuffer;
    Buffer indexBuffer;

};
