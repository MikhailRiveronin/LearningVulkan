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
    static VkBufferCreateInfo buffer_create_info(VkDeviceSize size, VkBufferUsageFlags usage);
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
    static VkRenderPassBeginInfo render_pass_begin_info(VkRenderPass render_pass, VkFramebuffer framebuffer, u32 width, u32 height, std::vector<VkClearValue> const& clear_values);


















    
    static VkSubmitInfo submit_info(VkSemaphore const* wait_semaphore, VkPipelineStageFlags const* wait_dst_stage_mask, VkCommandBuffer const* command_buffer, VkSemaphore const* signal_semaphore);
    static VkPresentInfoKHR present_info(VkSemaphore const* wait_semaphore, VkSwapchainKHR const* swapchain, u32 const* image_index);

    static VkImageMemoryBarrier image_memory_barrier(VkAccessFlags src_access_mask, VkAccessFlags dst_access_mask, VkImageLayout old_layout, VkImageLayout new_layout, VkImage image);
    static VkBufferImageCopy buffer_image_copy(u32 width, u32 height);





    static VkMemoryAllocateInfo memory_allocate_info(VkDeviceSize allocation_size, u32 memory_type_index);





    

    


    static VkShaderModuleCreateInfo shader_module_create_info(std::vector<u32 const> const& byte_code);
    static VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info(VkShaderStageFlagBits stage, VkShaderModule module);
    static VkVertexInputBindingDescription vertex_input_binding_description(u32 binding, u32 stride);
    static VkVertexInputAttributeDescription vertex_input_attribute_description(u32 location, u32 binding, VkFormat format, u32 offset);
    static VkPipelineVertexInputStateCreateInfo pipeline_vertex_input_state_create_info(std::vector<VkVertexInputBindingDescription> const& vertex_binding_descriptions, std::vector<VkVertexInputAttributeDescription> const& vertex_attribute_descriptions);
    static VkPipelineInputAssemblyStateCreateInfo pipeline_input_assembly_state_create_info(VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    static VkPipelineTessellationStateCreateInfo pipeline_tessellation_state_create_info();
    static VkViewport viewport(float width, float height);
    static VkRect2D scissor(u32 width, u32 height);
    static VkPipelineViewportStateCreateInfo pipeline_viewport_state_create_info(std::vector<VkViewport> const& viewports, std::vector<VkRect2D> const& scissors);
    static VkPipelineRasterizationStateCreateInfo pipeline_rasterization_state_create_info(VkBool32 depth_clamp_enable = VK_FALSE, VkBool32 rasterizer_discard_enable = VK_FALSE, VkPolygonMode polygon_mode = VK_POLYGON_MODE_FILL, VkCullModeFlags cull_mode = VK_CULL_MODE_NONE, VkFrontFace front_face = VK_FRONT_FACE_COUNTER_CLOCKWISE, VkBool32 depth_bias_enable = VK_FALSE, float depth_bias_constant_factor = 0.f, float depth_bias_clamp = 0.f, float depth_bias_slope_factor = 0.f, float line_width = 1.f);
    static VkPipelineMultisampleStateCreateInfo pipeline_multisample_state_create_info(VkSampleCountFlagBits rasterization_samples = VK_SAMPLE_COUNT_1_BIT, VkBool32 sample_shading_enable = VK_FALSE, float min_sample_shading = 0.f, VkSampleMask const* sample_mask = nullptr, VkBool32 alpha_to_coverage_enable = VK_FALSE, VkBool32 alpha_to_one_enable = VK_FALSE);
    static VkPipelineDepthStencilStateCreateInfo pipeline_depth_stencil_state_create_info(VkBool32 depth_test_enable = VK_TRUE, VkBool32 depth_write_enable = VK_TRUE, VkCompareOp depth_compare_op = VK_COMPARE_OP_LESS, VkBool32 depth_bounds_test_enable = VK_FALSE, VkBool32 stencil_test_enable = VK_FALSE, VkStencilOpState front = {}, VkStencilOpState back = {});
    static VkPipelineColorBlendAttachmentState pipeline_color_blend_attachment_state(VkBool32 blend_enable, VkBlendFactor src_color_blend_factor, VkBlendFactor dst_color_blend_factor, VkBlendOp color_blend_op, VkBlendFactor src_alpha_blend_factor, VkBlendFactor dst_alpha_blend_factor, VkBlendOp alpha_blend_op);
    static VkPipelineColorBlendStateCreateInfo pipeline_color_blend_state_create_info(std::vector<VkPipelineColorBlendAttachmentState> const& color_blend_attachments);
    static VkPipelineDynamicStateCreateInfo pipeline_dynamic_state_create_info(std::vector<VkDynamicState> const& dynamic_states);
    static VkPipelineLayoutCreateInfo pipeline_layout_create_info(std::vector<VkDescriptorSetLayout> const& set_layouts);
    static VkGraphicsPipelineCreateInfo graphics_pipeline_create_info(std::vector<VkPipelineShaderStageCreateInfo> const& stages, VkPipelineInputAssemblyStateCreateInfo const* input_assembly_state, VkPipelineTessellationStateCreateInfo const* tessellation_state, VkPipelineViewportStateCreateInfo const* viewport_state, VkPipelineRasterizationStateCreateInfo const* rasterization_state, VkPipelineMultisampleStateCreateInfo const* multisample_state, VkPipelineDepthStencilStateCreateInfo const* depth_stencil_state, VkPipelineColorBlendStateCreateInfo const* color_blend_state, VkPipelineDynamicStateCreateInfo const* dynamic_state, VkPipelineLayout layout, VkRenderPass render_pass, u32 subpass = 0, VkPipeline base_pipeline_handle = VK_NULL_HANDLE, i32 base_pipeline_index = 0);

    static VkSpecializationMapEntry specialization_map_entry(u32 constant_id, u32 offset, size_t size);
    static VkSpecializationInfo specialization_info(std::vector<VkSpecializationMapEntry> const& map_entries, size_t data_size, void const* data);










    static VkDescriptorSetLayoutBinding descriptor_set_layout_binding(u32 binding, VkDescriptorType descriptor_type, VkShaderStageFlags stage_flags, u32 descriptor_count = 1);
    static VkDescriptorPoolSize descriptor_pool_size(VkDescriptorType type, u32 descriptor_count);
    static VkDescriptorPoolCreateInfo descriptor_pool_create_info(u32 max_sets, std::vector<VkDescriptorPoolSize> const& pool_sizes);
    static VkDescriptorSetLayoutBindingFlagsCreateInfo descriptor_set_layout_binding_flags_create_info(std::vector<VkDescriptorBindingFlags> const& binding_flags);
    static VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info(std::vector<VkDescriptorSetLayoutBinding> const& bindings, void const* next = nullptr);
    static VkDescriptorSetVariableDescriptorCountAllocateInfo descriptor_set_variable_descriptor_count_allocate_info(std::array<u32, FRAMES_IN_FLIGHT> const& descriptor_counts);
    static VkDescriptorSetAllocateInfo descriptor_set_allocate_info(VkDescriptorPool descriptor_pool, std::array<VkDescriptorSetLayout, FRAMES_IN_FLIGHT> const& set_layouts, void const* next = nullptr);
    static VkDescriptorBufferInfo descriptor_buffer_info(VkBuffer buffer, VkDeviceSize offset = 0, VkDeviceSize range = VK_WHOLE_SIZE);
    static VkDescriptorImageInfo descriptor_image_info(VkImageView image_view, VkImageLayout image_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VkSampler sampler = VK_NULL_HANDLE);

    static VkWriteDescriptorSet write_descriptor_set(VkDescriptorSet dst_set, u32 dst_binding, std::vector<VkDescriptorBufferInfo> buffer_infos, VkDescriptorType descriptor_type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    static VkWriteDescriptorSet write_descriptor_set(VkDescriptorSet dst_set, u32 dst_binding, std::vector<VkDescriptorImageInfo> image_infos, VkDescriptorType descriptor_type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);


    
    
    
    
    
    
    
    
    
    
    

    
    

    static VmaAllocationCreateInfo allocation_create_info(VmaAllocationCreateFlags flags);











    static glslang_resource_t glslang_resource(VkPhysicalDeviceLimits const& limits);
    static glslang_input_t glslang_input(VkShaderStageFlagBits shader_stage, std::string const& code, glslang_resource_t const* resource);
    static glslang_spv_options_t glslang_spv_options();
};
