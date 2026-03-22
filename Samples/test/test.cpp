#include "engine/sample.h"
#include "engine/structures.h"
#include "engine/utils.h"

#include "engine/scene_manager.h"



#include "engine/Application.h"
#include "engine/Entry.h"
#include "engine/EventManager.h"
#include "engine/Initializer.h"
#include "engine/ProceduralMeshes/Box.h"
#include "engine/ProceduralMeshes/Sphere.h"


class Test : public Sample
{
public:
    Test(u32 width, u32 height, std::string const& name);

    void setup_scene();





private:
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
    std::vector<VkPipeline> pipelines;

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
    auto 

}
