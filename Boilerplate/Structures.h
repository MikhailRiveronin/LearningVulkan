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

    void* mapped = nullptr;
    u32 alignment = 0;
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

    VkImageUsageFlags usage = 0;
    VkMemoryPropertyFlags memoryProperties = 0;

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

struct Texture {
    Image image;
    Sampler sampler;
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
        struct {
            std::vector<VkSemaphore> imageAcquired;
            std::vector<VkSemaphore> renderFinished;
        } semaphores;

        struct {
            std::vector<VkFence> previousFrameFinished;
        } fences;
    } synchronization;
};

struct Mesh {
    struct DrawArgs {
        u32 indexCount = 0;
        u32 firstIndex = 0;
        i32 vertexOffset = 0;
    };
    std::vector<DrawArgs> submeshes;

    Buffer vertexBuffer;
    Buffer indexBuffer;
};

struct DirLight {
    glm::vec3 dir;
    u32 padding0;
    glm::vec3 ambient;
    u32 padding1;
    glm::vec3 diffuse;
    u32 padding2;
    glm::vec3 specular;
    u32 padding3;
};

struct SpotLight {
    glm::vec3 pos;
    u32 padding0;
    glm::vec3 dir;
    u32 padding1;
    glm::vec3 ambient;
    u32 padding2;
    glm::vec3 diffuse;
    u32 padding3;
    glm::vec3 specular;
    float cutOff;
    float outerCutOff;
    float constAtt;
    float linearAtt;
    float quadAtt;
    u32 padding4[3];
};

struct PointLight {
    glm::vec3 pos;
    u32 padding0;
    glm::vec3 ambient;
    u32 padding1;
    glm::vec3 diffuse;
    u32 padding2;
    glm::vec3 specular;
    float constAtt;
    float linearAtt;
    float quadAtt;
    u32 padding3[2];
};

struct Pass {
    glm::mat4 view;
    glm::mat4 proj;
    glm::vec3 viewPos;
    u32 padding;
};

struct Material {
    glm::vec3 ambient = glm::vec3(1.f);
    u32 padding0;
    glm::vec3 diffuse = glm::vec3(1.f);
    u32 padding1;
    glm::vec3 specular = glm::vec3(1.f);
    float shininess = 0.f;
    u32 diffuseTexIndex = 0;
    u32 specularTexIndex = 0;
    u32 padding3[2];
};

struct RenderObject {
    glm::mat4 world = glm::mat4(1.f);
    glm::mat4 texTransform = glm::mat4(1.f);

    u32 meshIndex = 0;
    u32 submeshIndex = 0;
    u32 materialIndex = 0;
    u32 padding;
};

struct FrameResource {
    Buffer passBuffer;
    Buffer dirLightBuffer;
    Buffer spotLightBuffer;
    Buffer pointLightBuffer;
    Buffer materialBuffer;
    Buffer renderObjectBuffer;
};

struct ResourceDescriptor {
    VkDescriptorPool pool;
    VkDescriptorSetLayout setLayout;
    std::vector<VkDescriptorSet> sets;
};
