#include "vulkan_classes.h"

#include "third_party/SPIRV-Reflect/spirv_reflect.h"

#include "vulkan_struct_initializers.h"
#include "utils.h"

namespace engine
{

Context::Context()
{
    VK_CHECK(volkInitialize());

    volkLoadInstance(instance);

    glslang_initialize_process();
}

Context::~Context()
{
    glslang_finalize_process();
}

RAII_Wrapper<Shader_Module_Handle> Context::create_shader_module(Shader_Module_Desc const& desc)
{
    Shader_Module_State state;
    auto create_info = Vulkan_Struct_Initializers::shader_module_create_info(desc.data_size, desc.data);
    VK_CHECK(vkCreateShaderModule(device, &create_info, allocation_callbacks, &state.module));

    SpvReflectShaderModule reflect_module;
    auto result = spvReflectCreateShaderModule(desc.data_size, desc.data, &reflect_module);
    if (spvReflectCreateShaderModule(desc.data_size, desc.data, &reflect_module) != SPV_REFLECT_RESULT_SUCCESS)
    {
        // TODO: Handle error
    }
    SCOPE_EXIT { spvReflectDestroyShaderModule(&reflect_module); };

    for (u32 i = 0; i < reflect_module.push_constant_block_count; ++i)
    {
        auto& block = reflect_module.push_constant_blocks[i];
        state.push_constants_size = std::max(state.push_constants_size, block.offset + block.size);
    }

    return { this, shader_module_pool.create(std::move(state)) };
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

    auto& spec_constant_desc = desc.spec_constant_desc;
    std::vector<VkSpecializationMapEntry> spec_map_entries(spec_constant_desc.entries.size());
    for (u32 i = 0; i < spec_map_entries.size(); ++i)
    {
        auto& entry = spec_constant_desc.entries[i];
        spec_map_entries[i] = Vulkan_Struct_Initializers::specialization_map_entry(entry.constant_id, entry.offset, entry.size);
    };
    auto spec_info = Vulkan_Struct_Initializers::specialization_info(spec_map_entries, spec_constant_desc.data_size, spec_constant_desc.data);

    Graphics_Pipeline_State pipeline_state;
#define UPDATE_PUSH_CONSTANT_SIZE(stage, shader_module)                                          \
    if (shader_module)                                                                           \
    {                                                                                            \
        pipeline_state.stages |= stage;                                                          \
        push_constants_size = std::max(push_constants_size, shader_module->push_constants_size); \
    }

    u32 push_constants_size = 0;
    UPDATE_PUSH_CONSTANT_SIZE(VK_SHADER_STAGE_VERTEX_BIT, vert_module);
    UPDATE_PUSH_CONSTANT_SIZE(VK_SHADER_STAGE_FRAGMENT_BIT, frag_module);
#undef UPDATE_PUSH_CONSTANT_SIZE
    assert(push_constants_size <= physical_device_properties_2.limits);

    


}

} // namespace engine
