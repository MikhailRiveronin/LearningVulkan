#pragma once

#include "defines.h"

namespace engine
{

template <typename Object_Type>
class Handle
{
public:
    u32 index = 0;
    u32 gen = 0;

    Handle(u32 index, u32 generation) : index(index), gen(generation)
    {
    };

    bool valid() const
    {
        return gen != 0;
    }

private:
};

template <typename Object_Type>
class Pool
{
public:
    u32 object_count = 0;

    Handle<Object_Type> create(Object_Type&& object);
    void destroy(Handle<Object_Type> handle);

    Object_Type* get(Handle<Object_Type> handle)
    {
        if (!handle.valid())
        {
            return nullptr;
        }

        assert(handle.index < entries.size());
        assert(handle.gen == entries[handle.index].gen);
        return &entries[handle.index].object;
    }

    Object_Type const* get(Handle<Object_Type> handle) const
    {
        if (!handle.valid())
        {
            return nullptr;
        }

        assert(handle.index < entries.size());
        assert(handle.gen == entries[handle.index].gen);
        return &entries[handle.index].object;
    }

private:
    static constexpr u32 end_sentinel = 0xffffffff;
    class Pool_Entry
    {
    public:
        Object_Type object = {};
        u32 gen = 1;
        u32 next_free = end_sentinel;

        explicit Pool_Entry(Object_Type&& object) : object(std::move(object))
        {
        }
    };
    std::vector<Pool_Entry> entries;

    u32 free_slot = end_sentinel;
};

template <typename Object_Type>
Handle<Object_Type> Pool<Object_Type>::create(Object_Type&& object)
{
    u32 index;
    if (free_slot == end_sentinel)
    {
        index = static_cast<u32>(entries.size());
        entries.emplace_back(object);
    }
    else
    {
        index = free_slot;
        free_slot = entries[index].next_free;
        entries[index].object = std::move(object);
    }

    object_count++;
    return Handle<Object_Type>(index, entries[index].gen);
}

template <typename Object_Type>
void Pool<Object_Type>::destroy(Handle<Object_Type> handle)
{
    u32 index = handle.index;
    assert(index < entries.size());
    assert(handle.gen == entries[index].gen);

    entries[index].object = Object_Type{};
    entries[index].gen++;
    entries[index].next_free = free_slot;

    free_slot = index;
    object_count--;
}

struct Shader_Module_Desc
{
    size_t data_size = 0;
    void const* data = nullptr;
};

struct Shader_Module_State
{
    VkShaderModule module = VK_NULL_HANDLE;
    u32 push_constants_size = 0;
};





class Context;

using Shader_Module_Handle = Handle<Shader_Module_State>;
using Graphics_Pipeline_Handle = Handle<Graphics_Pipeline_State>;

template <typename Handle_Type>
class RAII_Wrapper
{
public:
    RAII_Wrapper(Context* context, Handle_Type const& handle) : context(context), handle(handle)
    {
    }


private:
    Context* context = nullptr;
    Handle_Type handle = {};
};














struct Specialization_Constant_Desc
{
    struct Specialization_Constant_Entry
    {
        u32 constant_id = 0;
        u32 offset = 0;
        u32 size = 0;
    };
    std::vector<Specialization_Constant_Entry> entries;

    u32 data_size = 0;
    void const* data = nullptr;
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


struct Graphics_Pipeline_Desc
{
    Shader_Module_Handle vert;
    Shader_Module_Handle frag;

    VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    Specialization_Constant_Desc spec_constant_desc = {};

    struct Color_Attachment
    {
        VkFormat format = VK_FORMAT_UNDEFINED;

        VkBool32 blend_enable = VK_FALSE;
        VkBlendFactor src_color_blend_factor = VK_BLEND_FACTOR_ONE;
        VkBlendFactor dst_color_blend_factor = VK_BLEND_FACTOR_ZERO;
        VkBlendOp color_blend_op = VK_BLEND_OP_ADD;
        VkBlendFactor src_alpha_blend_factor = VK_BLEND_FACTOR_ONE;
        VkBlendFactor dst_alpha_blend_factor = VK_BLEND_FACTOR_ZERO;
        VkBlendOp alpha_blend_op = VK_BLEND_OP_ADD;
    };
    std::vector<Color_Attachment> color_attachments;
    VkFormat depth_format = VK_FORMAT_UNDEFINED;
    VkFormat stencil_format = VK_FORMAT_UNDEFINED;

    VkCullModeFlags cull_mode = VK_CULL_MODE_NONE;
    VkFrontFace front_face = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    VkPolygonMode polygon_mode = VK_POLYGON_MODE_FILL;
};

struct Graphics_Pipeline_State
{
    Graphics_Pipeline_Desc desc;

    VkPipeline handle = VK_NULL_HANDLE;
    VkShaderStageFlags stages = 0;
  VkPipelineLayout pipelineLayout_ = VK_NULL_HANDLE;
}

struct Context
{
public:
    VkInstance instance = VK_NULL_HANDLE;
    VkDevice device;

    VkPhysicalDeviceDepthStencilResolveProperties physical_device_depth_stencil_resolve_properties = Vulkan_Struct_Initializers::physical_device_depth_stencil_resolve_properties();
    VkPhysicalDeviceDriverProperties physical_device_driver_properties = Vulkan_Struct_Initializers::physical_device_driver_properties(&physical_device_depth_stencil_resolve_properties);
    VkPhysicalDeviceVulkan12Properties physical_device_vulkan_1_2_properties = Vulkan_Struct_Initializers::physical_device_vulkan_1_2_properties(&physical_device_driver_properties);
    VkPhysicalDeviceProperties2 physical_device_properties_2 = Vulkan_Struct_Initializers::physical_device_properties_2(&physical_device_vulkan_1_2_properties);



    Context();
    ~Context();



    RAII_Wrapper<Shader_Module_Handle> create_shader_module(Shader_Module_Desc const& desc);

    VkPipeline create_graphics_pipeline(Graphics_Pipeline_Desc const& desc);

private:
    Pool<Shader_Module_State> shader_module_pool;
    Pool<Graphics_Pipeline_Handle> graphics_pipeline_pool;




    Resource_Manager resource_manager;



    VmaAllocator allocator;
    VkAllocationCallbacks* allocation_callbacks;

#ifdef _DEBUG
    VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = {};
#endif
    
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

    // struct {
    //     VkCommandPool pool;
    //     std::vector<VkCommandBuffer> buffers;
    // } graphicsCommandBuffer;

    struct {
        struct {
            std::vector<VkSemaphore> imageAcquired;
            std::vector<VkSemaphore> renderFinished;
        } semaphores;

        struct {
            std::vector<VkFence> previousFrameFinished;
        } fences;
    } synchronization;

    u32 current_frame_index;



    struct
    {
        struct Per_Frame_Objects
        {
            // VkCommandBuffer command_buffer;
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
            // VkCommandBuffer command_buffer;
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

    std::array<VkCommandBuffer, FRAMES_IN_FLIGHT> graphics_command_buffers;


    std::array<VkFramebuffer, FRAMES_IN_FLIGHT> framebuffers;

    VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;
};








} // namespace engine












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

struct Image_View
{
    VkImageView handle = VK_NULL_HANDLE;
    VkImageViewType view_type = VK_IMAGE_VIEW_TYPE_2D;
    VkImageAspectFlags aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;
};

struct Sampler
{
    VkSampler handle = VK_NULL_HANDLE;
    VkFilter magFilter = VK_FILTER_LINEAR;
    VkFilter minFilter = VK_FILTER_LINEAR;
    VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    VkSamplerAddressMode addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    VkSamplerAddressMode addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
};

struct Texture
{
    std::string name;

    Image image;
    Image_View image_view;
    Sampler sampler;

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

    // struct
    // {
    //     VkImageView handle = VK_NULL_HANDLE;
    //     VkImageViewType view_type = VK_IMAGE_VIEW_TYPE_2D;
    //     VkImageAspectFlags aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;
    // } image_view;

    // struct
    // {
    //     VkSampler handle = VK_NULL_HANDLE;
    //     VkFilter magFilter = VK_FILTER_LINEAR;
    //     VkFilter minFilter = VK_FILTER_LINEAR;
    //     VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    //     VkSamplerAddressMode addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    //     VkSamplerAddressMode addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    // } sampler;

    VkAttachmentLoadOp loadOp;



};


struct ShaderStage {
    VkShaderStageFlagBits stage;
    std::string filename;
    VkShaderModule module;
    VkPipelineShaderStageCreateInfo createInfo;
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
