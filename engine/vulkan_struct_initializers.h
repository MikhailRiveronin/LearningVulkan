#pragma once

#include "defines.h"
#include "vk_mem_alloc.h"

class Vulkan_Struct_Initializers
{
public:
    static VkDebugUtilsMessengerCreateInfoEXT debug_utils_messenger_create_info(PFN_vkDebugUtilsMessengerCallbackEXT user_callback);
    static VkApplicationInfo application_info();
    static VkInstanceCreateInfo instance_create_info(VkApplicationInfo const* app_info, std::vector<char const*> const& enabled_layer_names, std::vector<char const*> const& enabled_extension_names, void const* next = nullptr);

    static VkDeviceQueueCreateInfo device_queue_create_info(u32 queue_family_index);





    static VkWin32SurfaceCreateInfoKHR win32_surface_create_info(HWND h_wnd);
    static VkSwapchainCreateInfoKHR swapchain_create_info(VkSurfaceKHR surface, u32 min_image_count, VkFormat image_format, VkColorSpaceKHR image_color_space, VkExtent2D image_extent, std::vector<u32> const& queue_family_indices, VkSurfaceTransformFlagBitsKHR pre_transform, VkPresentModeKHR present_mode);
    static VkImageCreateInfo image_create_info(VkFormat format, u32 width, u32 height, VkImageUsageFlags usage);
    static VkImageViewCreateInfo image_view_create_info(VkImage image, VkFormat format);
    static VkSamplerCreateInfo sampler_create_info();
    static VkFramebufferCreateInfo framebuffer_create_info(VkRenderPass render_pass, std::vector<VkImageView> const& attachments, VkExtent2D const& extent);
    static VkAttachmentDescription attachment_description(VkFormat format, VkAttachmentLoadOp load_op, VkAttachmentStoreOp store_op, VkImageLayout initial_layout, VkImageLayout final_layout);
    static VkAttachmentReference attachment_reference(u32 attachment, VkImageLayout layout);
    static VkSubpassDescription subpass_description(std::vector<VkAttachmentReference> const& color_attachments, VkAttachmentReference const* depth_stencil_attachment, std::vector<VkAttachmentReference> const& input_attachments = std::vector<VkAttachmentReference>());
    static VkSubpassDependency subpass_dependency(u32 src_subpass, u32 dst_subpass, VkPipelineStageFlags src_stage_mask, VkPipelineStageFlags dst_stage_mask, VkAccessFlags src_access_mask, VkAccessFlags dst_access_mask);
    static VkRenderPassCreateInfo render_pass_create_info(std::vector<VkAttachmentDescription> const& attachments, std::vector<VkSubpassDescription> const& subpasses, std::vector<VkSubpassDependency> const& dependencies);

    static VkCommandBufferBeginInfo command_buffer_begin_info();
    static VkViewport viewport(VkExtent2D extent);
    static VkRect2D scissor(VkExtent2D extent);
    static VkSubmitInfo submit_info(VkSemaphore const* wait_semaphore, VkPipelineStageFlags const* wait_dst_stage_mask, VkCommandBuffer const* command_buffer, VkSemaphore const* signal_semaphore);
    static VkPresentInfoKHR present_info(VkSemaphore const* wait_semaphore, VkSwapchainKHR const* swapchain, u32 const* image_index);










    

    static VkBufferCreateInfo buffer_create_info(VkDeviceSize size, VkBufferUsageFlags usage);


    


    static VkShaderModuleCreateInfo shaderModuleCreateInfo(std::vector<u32> const& code);

    static VkVertexInputBindingDescription vertexInputBindingDescription(u32 binding, u32 stride, VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX);
    static VkVertexInputAttributeDescription vertexInputAttributeDescription(u32 location, u32 binding, VkFormat format, u32 offset);


    static VkDescriptorSetLayoutBinding descriptorSetLayoutBinding(u32 binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags);
    static VkDescriptorPoolSize descriptorPoolSize(VkDescriptorType type, u32 descriptorCount);
    static VkDescriptorPoolCreateInfo descriptorPoolCreateInfo(u32 maxSets, std::vector<VkDescriptorPoolSize> const& poolSizes);
    static VkDescriptorSetLayoutBindingFlagsCreateInfo descriptorSetLayoutBindingFlagsCreateInfo(std::vector<VkDescriptorBindingFlags> const& bindingFlags);
    static VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo(std::vector<VkDescriptorSetLayoutBinding> const& bindings, void const* next = nullptr);
    static VkDescriptorSetVariableDescriptorCountAllocateInfo descriptorSetVariableDescriptorCountAllocateInfo(std::vector<u32> const& descriptorCounts);
    static VkDescriptorSetAllocateInfo descriptorSetAllocateInfo(
        VkDescriptorPool descriptorPool,
        u32 descriptorSetCount,
        std::vector<VkDescriptorSetLayout> const& setLayouts,
        void const* next = nullptr);
    static VkDescriptorBufferInfo descriptorBufferInfo(VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range = VK_WHOLE_SIZE);
    static VkDescriptorImageInfo descriptorImageInfo(VkSampler sampler, VkImageView imageView, VkImageLayout imageLayout);
    static VkWriteDescriptorSet writeDescriptorSet(
        VkDescriptorSet dstSet,
        u32 dstBinding,
        VkDescriptorType descriptorType,
        VkDescriptorImageInfo const* imageInfo,
        VkDescriptorBufferInfo const* bufferInfo);

    static VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule module);
    static VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo(
        std::vector<VkVertexInputBindingDescription> const& vertexBindingDescriptions,
        std::vector<VkVertexInputAttributeDescription> const& vertexAttributeDescriptions);
    static VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo(
        VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        VkBool32 primitiveRestartEnable = VK_FALSE);
    static VkPipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo();
    static VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo(std::vector<VkViewport> const& viewports, std::vector<VkRect2D> const& scissors);
    static VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo(
        VkBool32 depthClampEnable = VK_FALSE,
        VkBool32 rasterizerDiscardEnable = VK_FALSE,
        VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL,
        VkCullModeFlags cullMode = VK_CULL_MODE_NONE,
        VkFrontFace frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        VkBool32 depthBiasEnable = VK_FALSE,
        float depthBiasConstantFactor = 0.f,
        float depthBiasClamp = 0.f,
        float depthBiasSlopeFactor = 0.f,
        float lineWidth = 1.f);
    static VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo(
        VkSampleCountFlagBits rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        VkBool32 sampleShadingEnable = VK_FALSE,
        float minSampleShading = 0.f,
        VkSampleMask const* sampleMask = nullptr,
        VkBool32 alphaToCoverageEnable = VK_FALSE,
        VkBool32 alphaToOneEnable = VK_FALSE);
    static VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo(
        VkBool32 depthTestEnable = VK_TRUE,
        VkBool32 depthWriteEnable = VK_TRUE,
        VkCompareOp depthCompareOp = VK_COMPARE_OP_LESS,
        VkBool32 depthBoundsTestEnable = VK_FALSE,
        VkBool32 stencilTestEnable = VK_FALSE,
        VkStencilOpState front = {},
        VkStencilOpState back = {});
    static VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState(
        VkBool32 blendEnable = VK_FALSE,
        VkBlendFactor srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
        VkBlendFactor dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        VkBlendOp colorBlendOp = VK_BLEND_OP_ADD,
        VkBlendFactor srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        VkBlendFactor dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        VkBlendOp alphaBlendOp = VK_BLEND_OP_ADD,
        VkColorComponentFlags colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);
    static VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo(std::vector<VkPipelineColorBlendAttachmentState> const& colorBlendAttachments);
    static VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo(std::vector<VkDynamicState> const& dynamicStates);
    static VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo(
        std::vector<VkDescriptorSetLayout> const& setLayouts,
        std::vector<VkPushConstantRange> const& pushConstantRanges);
    static VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo(
        std::vector<VkPipelineShaderStageCreateInfo> const& stages,
        VkPipelineVertexInputStateCreateInfo const* vertexInputState,
        VkPipelineInputAssemblyStateCreateInfo const* inputAssemblyState,
        VkPipelineTessellationStateCreateInfo const* tessellationState,
        VkPipelineViewportStateCreateInfo const* viewportState,
        VkPipelineRasterizationStateCreateInfo const* rasterizationState,
        VkPipelineMultisampleStateCreateInfo const* multisampleState,
        VkPipelineDepthStencilStateCreateInfo const* depthStencilState,
        VkPipelineColorBlendStateCreateInfo const* colorBlendState,
        VkPipelineDynamicStateCreateInfo const* dynamicState,
        VkPipelineLayout layout,
        VkRenderPass renderPass,
        u32 subpass = 0,
        VkPipeline basePipelineHandle = VK_NULL_HANDLE,
        i32 basePipelineIndex = 0);

    
    static VkRenderPassBeginInfo renderPassBeginInfo(
        VkRenderPass renderPass,
        VkFramebuffer framebuffer,
        VkRect2D renderArea,
        std::vector<VkClearValue> const& clearValues = std::vector<VkClearValue>());

    static VmaAllocationCreateInfo allocation_create_info(VmaAllocationCreateFlags flags);
};
