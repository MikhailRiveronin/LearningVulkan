#pragma once

#include "defines.h"

#include "device.h"
#include "resource_manager.h"

// #include <glm/glm.hpp>
// #include <vulkan/vulkan.h>
// #include <optional>
// #include <vector>




// enum class PhysicalDeviceType {
//     DISCRETE,
//     INTEGRATED,
//     SOFTWARE
// };

// struct ContextConfig {
//     bool enableValidation = true;
//     PhysicalDeviceType physicalDeviceType = PhysicalDeviceType::DISCRETE;
// };

struct Buffer
{
    VkBuffer handle = VK_NULL_HANDLE;
    VmaAllocation allocation = VK_NULL_HANDLE;
    VmaAllocationInfo allocation_info = {};
    // VkDeviceSize size;
    // VkBufferUsageFlags usage;
    // VmaAllocationCreateFlags allocation_create;
    // VkDeviceMemory memory = VK_NULL_HANDLE;
    // VkMemoryPropertyFlags memory_properties = 0;
    // i32 memory_index;
    // void* mapped = nullptr;
    // u32 alignment = 0;
    // bool locked;

};

struct Image
{
    VkImage handle;
    VmaAllocation allocation;
    VmaAllocationInfo allocation_info;



    VkImageCreateFlags flags = 0;
    VkImageType image_type = VK_IMAGE_TYPE_2D;
    VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
    u32 width = 0;
    u32 height = 0;
    u32 mip_levels = 1;
    u32 array_layers = 1;
    VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
    VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
    VkImageUsageFlags usage = 0;
    VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE;
    u32 queue_family_index_count = 0;
    u32* queue_family_indices = nullptr;
    VkImageLayout initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;


};

struct Texture
{
    std::string name;

    Image image;

    // struct
    // {
    //     VkImage handle;
    //     // VkImageCreateFlags flags = 0;
    //     // VkImageType image_type = VK_IMAGE_TYPE_2D;
    //     VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
    //     u32 width = 0;
    //     u32 height = 0;
    //     u32 mip_levels = 1;
    //     u32 array_layers = 1;
    //     VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
    //     VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
    //     VkImageUsageFlags usage = 0;
    //     VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE;
    //     u32 queue_family_index_count = 0;
    //     u32* queue_family_indices = nullptr;
    //     VkImageLayout initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;


    //     VmaAllocation allocation;
    //     VmaAllocationInfo allocation_info;
    // } image;

    struct
    {
        VkImageView handle = VK_NULL_HANDLE;
        VkImageViewType view_type = VK_IMAGE_VIEW_TYPE_2D;
        VkImageAspectFlags aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;
    } image_view;

    struct
    {
        VkSampler handle = VK_NULL_HANDLE;
        VkFilter magFilter = VK_FILTER_LINEAR;
        VkFilter minFilter = VK_FILTER_LINEAR;
        VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        VkSamplerAddressMode addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        VkSamplerAddressMode addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    } sampler;

    VkAttachmentLoadOp loadOp;



};

struct Attachment
{
    VkImage image = VK_NULL_HANDLE;
    VkImageView image_view = VK_NULL_HANDLE;
    VkAttachmentDescription description = {};
    VmaAllocation allocation = VK_NULL_HANDLE;
    VmaAllocationInfo allocation_info = {};









    // VkImage handle = VK_NULL_HANDLE;
    // VkImageCreateFlags flags = 0;
    // VkImageType imageType = VK_IMAGE_TYPE_2D;
    // VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
    // u32 width = 0;
    // u32 height = 0;
    // u32 mipLevels = 1;
    // u32 arrayLayers = 1;
    // VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
    // VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
    // VkImageUsageFlags usage = 0;
    // VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    // u32 queueFamilyIndexCount = 0;
    // u32* queueFamilyIndices = nullptr;
    // VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    // VkMemoryPropertyFlags memoryProperties = 0;

    // struct {
    //     VkImageView handle = VK_NULL_HANDLE;
    //     VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
    //     VkImageAspectFlags aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;
    // } view;

    // VkAttachmentLoadOp loadOp;


    // VmaAllocation allocation;
};

struct ShaderStage {
    VkShaderStageFlagBits stage;
    std::string filename;
    VkShaderModule module;
    VkPipelineShaderStageCreateInfo createInfo;
};



struct Context
{
    Device* device;

    Resource_Manager resource_manager;



    VmaAllocator allocator;
    VkAllocationCallbacks* allocation_callbacks;

#ifdef _DEBUG
    VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = {};
#endif
    VkInstance instance = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;

    // struct {
    //     VkDevice handle = VK_NULL_HANDLE;

    //     struct {
    //         VkPhysicalDeviceProperties properties;
    //         VkPhysicalDeviceFeatures features;
    //         VkPhysicalDeviceMemoryProperties memoryProperties;
    //     } support;

    //     struct {
    //         struct {
    //             VkQueue handle = VK_NULL_HANDLE;
    //             u32 familyIndex;
    //         } graphics;
            
    //         struct {
    //             VkQueue handle = VK_NULL_HANDLE;
    //             u32 familyIndex;
    //         } compute;
            
    //         struct {
    //             VkQueue handle = VK_NULL_HANDLE;
    //             u32 familyIndex;
    //         } transfer;
            
    //         struct {
    //             VkQueue handle = VK_NULL_HANDLE;
    //             u32 familyIndex;
    //         } present;
    //     } queues;
    // } 
    // device;

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
        std::vector<VkImageView> image_views;
        std::vector<VkFramebuffer> framebuffers;

        Image depthStencilBuffer;
    } swapchain;

    VkRenderPass render_pass;

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



    struct
    {
        struct Per_Frame_Objects
        {
            VkCommandBuffer command_buffer;
            VkFence in_flight_fence;
            VkDescriptorSet descriptor_set;

            struct
            {
                VkSemaphore ready; // TODO: check if realy needed
                VkSemaphore finished;
            } semaphores;
        };

        std::array<Per_Frame_Objects, FRAMES_IN_FLIGHT> per_frame_objects;

    } graphics;

    struct
    {
        VkQueue queue;
        VkCommandPool command_pool;
        VkDescriptorSetLayout descriptorSetLayout;
        VkPipelineLayout pipelineLayout;
        VkPipeline pipeline;

        struct Per_Frame_Objects
        {
            VkCommandBuffer command_buffer;
            VkFence in_flight_fence;
            VkDescriptorSet descriptor_set;

            struct
            {
                VkSemaphore ready; // TODO: check if realy needed
                VkSemaphore finished;
            } semaphores;
        };

        std::array<Per_Frame_Objects, FRAMES_IN_FLIGHT> per_frame_objects;

    } compute;



    VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;
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

struct DescriptorSetBinding {
    VkDescriptorSetLayoutBinding binding;
    VkWriteDescriptorSet descriptorWrite;
};

struct DescriptorSets
{
    std::vector<VkDescriptorSet> handles;
    VkDescriptorSetLayout setLayout = VK_NULL_HANDLE;
};

struct PipelineCreateInfo {
    std::vector<VkPipelineShaderStageCreateInfo> stages;
    std::vector<VkVertexInputBindingDescription> vertexBindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions;

    std::vector<DescriptorSetBinding> descriptorSetCreateInfos;
};

// struct Pipeline {
//     VkPipeline handle;
//     VkPipelineLayout layout;

//     DescriptorSets descriptorSets; // per-draw data
//     // VkPushConstantRange pushConstantRanges;
//     // std::vector<VkPushConstantRange> pushConstantRanges;
// };

struct VertexInputState {
    std::vector<VkVertexInputBindingDescription> vertexBindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions;
};
