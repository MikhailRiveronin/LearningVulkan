#include "vulkan_classes.h"

#include "vulkan_struct_initializers.h"
#include "utils.h"

namespace engine
{

Context::Context()
{
    glslang_initialize_process();
}

Context::~Context()
{
    glslang_finalize_process();
}

VkShaderModule Context::create_shader_module_from_SPIRV(std::vector<u32 const> byte_code)
{
    VkShaderModule shader_module;
    auto create_info = Vulkan_Struct_Initializers::shader_module_create_info(byte_code);
    VK_CHECK(vkCreateShaderModule(device, &create_info, allocation_callbacks, &shader_module));
    assert(shader_module != VK_NULL_HANDLE);
    return shader_module;
}

VkShaderModule Context::create_shader_module_from_GLSL(VkShaderStageFlagBits shader_stage, std::string const& source_code)
{
    auto glslang_resource = Vulkan_Struct_Initializers::glslang_resource(physical_device_properties.properties.limits);
    auto spirv = compile_GLSL_to_SPIRV(shader_stage, source_code, &glslang_resource);
    return create_shader_module_from_SPIRV(spirv);
}

VkPipeline Context::create_graphics_pipeline(Graphics_Pipeline_Desc const& desc)
{
    if (desc.color_attachments.empty())
    {
        // TODO: Handle error
        // Need at least one color attachment
    }

    if (!desc.vert.valid())
    {
        // TODO: Handle error
    }

    if (!desc.frag.valid())
    {
        // TODO: Handle error
    }

    std::vector<VkPipelineColorBlendAttachmentState> color_blend_attachment_states(desc.color_attachments.size());
    std::vector<VkFormat> color_attachment_formats(desc.color_attachments.size());
    for (u32 i = 0; i < desc.color_attachments.size(); ++i)
    {
        auto& attachment = desc.color_attachments[i];
        assert(attachment.format != VK_FORMAT_UNDEFINED);

        color_blend_attachment_states[i] = Vulkan_Struct_Initializers::pipeline_color_blend_attachment_state(attachment.blend_enable, attachment.src_color_blend_factor, attachment.dst_color_blend_factor, attachment.color_blend_op, attachment.src_alpha_blend_factor, attachment.dst_alpha_blend_factor, attachment.alpha_blend_op);
        color_attachment_formats[i] = desc.color_attachments[i].format;
    }

    auto vert_module = shader_module_pool.get(desc.vert);
    auto frag_module = shader_module_pool.get(desc.frag);

    std::vector<VkSpecializationMapEntry> specialization_map_entries;
    for (auto& entry : desc.spec_constant_desc.entries)
    {
        specialization_map_entries.push_back(Vulkan_Struct_Initializers::specialization_map_entry(entry.constant_id, entry.offset, entry.size));
    };

    auto specialization_info = Vulkan_Struct_Initializers::specialization_info(specialization_map_entries, desc.spec_constant_desc.data_size, desc.spec_constant_desc.data);


}

} // namespace engine
