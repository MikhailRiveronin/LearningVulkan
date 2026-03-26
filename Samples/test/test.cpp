#include "engine/sample.h"
#include "engine/structures.h"
#include "engine/utils.h"

#include "engine/scene_manager.h"

#include "engine/vulkan_struct_initializers.h"


#include "engine/Application.h"
#include "engine/Entry.h"
#include "engine/EventManager.h"
#include "engine/ProceduralMeshes/Box.h"
#include "engine/ProceduralMeshes/Sphere.h"


class Test : public Sample
{
public:
    Test(u32 width, u32 height, std::string const& name);

    void setup_scene();



    void create_descriptor_pool();
    void create_pipelines();





private:
    std::array<VkDescriptorSet, FRAMES_IN_FLIGHT> g_buffer_descriptor_sets;

    std::array<Buffer, FRAMES_IN_FLIGHT> world_view_proj_buffers;
    std::array<Buffer, FRAMES_IN_FLIGHT> material_data_buffers;
    std::array<Buffer, FRAMES_IN_FLIGHT> draw_data_buffers;

    struct
    {
        struct
        {
            VkPipeline handle;
            VkPipelineLayout layout;
            VkRenderPass render_pass;
            u32 subpass;
        } g_buffer;


    } pipelines;





    struct
    {
        Attachment position;
        Attachment normal;
        Attachment albedo;
    } framebuffer_attachments;

    void create_attachments();
    void create_framebuffer();
    void create_renderpass();










    std::vector<Mesh> meshes;
    std::vector<Texture> textures;
    std::vector<Material> materials;
    std::vector<RenderObject> renderObjects;
    std::vector<FrameResource> frameResources;
    std::vector<DescriptorSets> resourceDescriptors;
    std::vector<VkPushConstantRange> pushConstantRanges;
    std::vector<VkPipelineLayout> pipelineLayouts;

    struct {
        DirLight direct;
        SpotLight spot;
        std::vector<PointLight> points;
    } lights;
    std::vector<glm::vec3> pointLightPositions;

    Camera camera;
    glm::vec3 lightPos = glm::vec3(1.2f, 1.f, 2.f);
    glm::vec3 lightColor = glm::vec3(1.f);
    glm::vec3 diffuseColor;
    glm::vec3 ambientColor;

    void createMeshes() override;
    void createTextures() override;
    void createMaterials() override;
    void createRenderObjects() override;
    void createLights() override;
    void createFrameResources() override;
    void createResourceDescriptors() override;
    void createPushConstantRanges() override;
    void createPipelines() override;

    void updateFrameResources(u32 frameIndex) override;
    void recordCommandBuffer(VkCommandBuffer commandBuffer, u32 imageIndex, u32 frameIndex, ImDrawData* draw_data) override;

    void destroyPipelines() override;
    void destroyPushConstantRanges() override;
    void destroyResourceDescriptors() override;
    void destroyFrameResources() override;
    void destroyTextures() override;
    void destroyMeshes() override;
};

void Test::create_attachments()
{
    attachments.position.format = VK_FORMAT_R16G16B16A16_SFLOAT;
    attachments.position.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    attachments.position.view.aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;

    attachments.normal.format = VK_FORMAT_R16G16B16A16_SFLOAT;
    attachments.normal.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    attachments.normal.view.aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;

    attachments.albedo.format = VK_FORMAT_R8G8B8A8_UNORM;
    attachments.albedo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    attachments.albedo.view.aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;

    create_attachment(context, framebuffer_attachments.position);
    create_attachment(context, framebuffer_attachments.normal);
    create_attachment(context, framebuffer_attachments.albedo);

    // TODO: create descriptor sets
}

void Test::create_framebuffer()
{
    context.swapchain.framebuffers.resize(context.swapchain.image_views.size());
    std::vector<VkImageView> attachments(4);
    auto framebuffer_create_info = Vulkan_Struct_Initializers::framebuffer_create_info(context.render_pass, attachments, context.swapchain.extent);
    for (u32 i = 0; i < context.swapchain.framebuffers.size(); ++i)
    {
        attachments[0] = context.swapchain.image_views[i];
        attachments[1] = framebuffer_attachments.position.view.handle;
        attachments[2] = framebuffer_attachments.normal.view.handle;
        attachments[3] = framebuffer_attachments.albedo.view.handle;
        attachments[4] = context.swapchain.depthStencilBuffer.view.handle;
        VK_CHECK(vkCreateFramebuffer(context.device->handle, &framebuffer_create_info, nullptr, &context.swapchain.framebuffers[i]));
    }
}

void Test::create_renderpass()
{
    std::vector<VkAttachmentDescription> attachments(5);

    // Swapchain image attachment
    attachments[0] = Vulkan_Struct_Initializers::attachment_description(context.swapchain.format, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    // G-buffer attachments
    attachments[1] = Vulkan_Struct_Initializers::attachment_description(framebuffer_attachments.position.format, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    attachments[2] = Vulkan_Struct_Initializers::attachment_description(framebuffer_attachments.normal.format, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    attachments[3] = Vulkan_Struct_Initializers::attachment_description(framebuffer_attachments.albedo.format, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    // Depth-stencil buffer attachment
    attachments[4] = Vulkan_Struct_Initializers::attachment_description(context.swapchain.depthStencilBuffer.format, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    std::vector<VkSubpassDescription> subpasses(2);
    VkAttachmentReference depth_stencil_attachment = Vulkan_Struct_Initializers::attachment_reference(4, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    // First subpass
    {
        std::vector<VkAttachmentReference> color_attachments(3);
        color_attachments[0] = Vulkan_Struct_Initializers::attachment_reference(1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        color_attachments[1] = Vulkan_Struct_Initializers::attachment_reference(2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        color_attachments[2] = Vulkan_Struct_Initializers::attachment_reference(3, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        subpasses[0] = Vulkan_Struct_Initializers::subpass_description(color_attachments, depth_stencil_attachment);
    }

    // Second subpass
    {
        std::vector<VkAttachmentReference> color_attachments(1);
        color_attachments[0] = Vulkan_Struct_Initializers::attachment_reference(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        std::vector<VkAttachmentReference> input_attachments(3);
        input_attachments[0] = Vulkan_Struct_Initializers::attachment_reference(1, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        input_attachments[1] = Vulkan_Struct_Initializers::attachment_reference(2, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        input_attachments[2] = Vulkan_Struct_Initializers::attachment_reference(3, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        subpasses[1] = Vulkan_Struct_Initializers::subpass_description(color_attachments, depth_stencil_attachment, input_attachments);
    }

    std::vector<VkSubpassDependency> dependencies(3);
    dependencies[0] = Vulkan_Struct_Initializers::subpass_dependency(VK_SUBPASS_EXTERNAL, 0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
    dependencies[1] = Vulkan_Struct_Initializers::subpass_dependency(0, 1, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_INPUT_ATTACHMENT_READ_BIT);
    dependencies[2] = Vulkan_Struct_Initializers::subpass_dependency(1, VK_SUBPASS_EXTERNAL, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);

    auto render_pass_create_info = Vulkan_Struct_Initializers::render_pass_create_info(attachments, subpasses, dependencies);
    VK_CHECK(vkCreateRenderPass(context.device->handle, &render_pass_create_info, nullptr, &context.render_pass));
}

void Test::setup_scene()
{
    load_gltf();

}

void Test::create_descriptor_pool()
{
    std::vector<VkDescriptorPoolSize> descriptor_pool_sizes = { Vulkan_Struct_Initializers::descriptor_pool_size(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, FRAMES_IN_FLIGHT * 4) };
    auto descriptor_pool_create_info = Vulkan_Struct_Initializers::descriptor_pool_create_info(2, descriptor_pool_sizes)
    VK_CHECK(vkCreateDescriptorPool(context.device->handle, &descriptor_pool_create_info, context.allocation_callbacks, &context.descriptor_pool));
}

void Test::create_pipelines()
{
    // Compute


    // Graphics
    // G-buffer
    {
        VkDescriptorSetLayout descriptor_set_layout;
        std::vector<VkDescriptorSetLayoutBinding> descriptor_set_layout_bindings = { Vulkan_Struct_Initializers::descriptor_set_layout_binding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT) /* world-view-proj */, Vulkan_Struct_Initializers::descriptor_set_layout_binding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT) /* material data */, Vulkan_Struct_Initializers::descriptor_set_layout_binding(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT) /* draw data */, Vulkan_Struct_Initializers::descriptor_set_layout_binding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT) /* large texture description array */ };
        auto descriptor_set_layout_create_info = Vulkan_Struct_Initializers::descriptor_set_layout_create_info(descriptor_set_layout_bindings);
        VK_CHECK(vkCreateDescriptorSetLayout(context.device->handle, &descriptor_set_layout_create_info, context.allocation_callbacks, &descriptor_set_layout));
    
        std::vector<VkDescriptorSetLayout> set_layouts(FRAMES_IN_FLIGHT, descriptor_set_layout);
        auto descriptor_set_allocate_info = Vulkan_Struct_Initializers::descriptor_set_allocate_info(context.descriptor_pool, set_layouts);
        VK_CHECK(vkAllocateDescriptorSets(context.device->handle, &descriptor_set_allocate_info, g_buffer_descriptor_sets.data()));

        for (u32 i = 0; i < FRAMES_IN_FLIGHT; ++i)
        {
            std::vector<VkWriteDescriptorSet> descriptor_writes = { Vulkan_Struct_Initializers::write_descriptor_set(g_buffer_descriptor_sets[i], 0, Vulkan_Struct_Initializers::descriptor_buffer_info(world_view_proj_buffers[i].handle)), Vulkan_Struct_Initializers::write_descriptor_set(g_buffer_descriptor_sets[i], 0, Vulkan_Struct_Initializers::descriptor_buffer_info(material_data_buffers[i].handle)), Vulkan_Struct_Initializers::write_descriptor_set(g_buffer_descriptor_sets[i], 0, Vulkan_Struct_Initializers::descriptor_buffer_info(draw_data_buffers[i].handle)), Vulkan_Struct_Initializers::write_descriptor_set(g_buffer_descriptor_sets[i], 0, Vulkan_Struct_Initializers::descriptor_image_info( /* of all textures */)) };
            vkUpdateDescriptorSets(context.device->handle, descriptor_writes.size(), descriptor_writes.data(), 0, nullptr);
        }

        // Pipeline
        std::vector<VkPipelineShaderStageCreateInfo> pipeline_shader_stage_create_infos(2);
        std::vector<VkShaderModule> shader_modules(2);
        {
            auto code = load_shader_code("g_buffer.vert.spv");
            auto shader_module_create_info = Vulkan_Struct_Initializers::shader_module_create_info(code);
            VK_CHECK(vkCreateShaderModule(context.device->handle, &shader_module_create_info, context.allocation_callbacks, &shader_modules[0]));
            pipeline_shader_stage_create_infos[0] = Vulkan_Struct_Initializers::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, shader_modules[0]);
        }
        {
            auto code = load_shader_code("g_buffer.frag.spv");
            auto shader_module_create_info = Vulkan_Struct_Initializers::shader_module_create_info(code);
            VK_CHECK(vkCreateShaderModule(context.device->handle, &shader_module_create_info, context.allocation_callbacks, &shader_modules[0]));
            pipeline_shader_stage_create_infos[1] = Vulkan_Struct_Initializers::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, shader_modules[1]);
        }

        auto pipeline_input_assembly_state_create_info = Vulkan_Struct_Initializers::pipeline_input_assembly_state_create_info();
        auto pipeline_tessellation_state_create_info = Vulkan_Struct_Initializers::pipeline_tessellation_state_create_info();

        std::vector<VkViewport> viewports = { Vulkan_Struct_Initializers::viewport(context.swapchain.extent.width, context.swapchain.extent.height) };
        std::vector<VkRect2D> scissors = { Vulkan_Struct_Initializers::scissor(context.swapchain.extent.width, context.swapchain.extent.height) };
        auto pipeline_viewport_state_create_info = Vulkan_Struct_Initializers::pipeline_viewport_state_create_info(viewports, scissors);

        auto pipeline_rasterization_state_create_info = Vulkan_Struct_Initializers::pipeline_rasterization_state_create_info();
        auto pipeline_multisample_state_create_info = Vulkan_Struct_Initializers::pipeline_multisample_state_create_info();
        auto pipeline_depth_stencil_state_create_info = Vulkan_Struct_Initializers::pipeline_depth_stencil_state_create_info();

        std::vector<VkPipelineColorBlendAttachmentState> color_blend_attachment_states = { Vulkan_Struct_Initializers::pipeline_color_blend_attachment_state() };
        auto pipeline_color_blend_state_create_info = Vulkan_Struct_Initializers::pipeline_color_blend_state_create_info(color_blend_attachment_states);

        std::vector<VkDynamicState> dynamic_states = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        auto pipeline_dynamic_state_create_info = Vulkan_Struct_Initializers::pipeline_dynamic_state_create_info(dynamic_states);

        std::vector<VkDescriptorSetLayout> descriptor_set_layouts = { descriptor_set_layout };
        auto pipeline_layout_create_info = Vulkan_Struct_Initializers::pipeline_layout_create_info(descriptor_set_layouts);
        VK_CHECK(vkCreatePipelineLayout(context.device->handle, &pipeline_layout_create_info, context.allocation_callbacks, &pipelines.g_buffer.layout));

        auto graphics_pipeline_create_info = Vulkan_Struct_Initializers::graphics_pipeline_create_info(pipeline_shader_stage_create_infos, &pipeline_input_assembly_state_create_info, &pipeline_tessellation_state_create_info, &pipeline_viewport_state_create_info, &pipeline_rasterization_state_create_info, &pipeline_multisample_state_create_info, &pipeline_depth_stencil_state_create_info, &pipeline_color_blend_state_create_info, &pipeline_dynamic_state_create_info, pipelines.g_buffer.layout, pipelines.g_buffer.render_pass, pipelines.g_buffer.subpass);

        VK_CHECK(vkCreateGraphicsPipelines(context.device->handle, VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, context.allocation_callbacks, &pipelines.g_buffer.handle));

        vkDestroyShaderModule(context.device->handle, shader_modules[0], context.allocation_callbacks);
        vkDestroyShaderModule(context.device->handle, shader_modules[1], context.allocation_callbacks);
    }



}
