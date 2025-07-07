#include "RenderLayerSkybox.h"
#include "Boilerplate/Initializer.h"
#include "Boilerplate/Utils.h"

RenderLayerSkybox::RenderLayerSkybox(
    Context const& context,
    char const* front,
    char const* back,
    char const* left,
    char const* right,
    char const* up,
    char const* down)
{
    createCubeTexture(context, front, back, left, right, up, down, cubeTexture);

}

void RenderLayerSkybox::createRenderPass(Context const& context)
{
    std::vector<VkAttachmentDescription> attachmentDescriptions(2);
    attachmentDescriptions[0] = Initializer::attachmentDescription(
        context.swapchain.format.format,
        VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_STORE,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    attachmentDescriptions[1] = Initializer::attachmentDescription(
        context.swapchain.depthStencilBuffer.format,
        VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_STORE,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    std::vector<VkAttachmentReference> colorAttachmentReferences(1);
    colorAttachmentReferences[0] = Initializer::attachmentReference(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    auto depthStencilAttachmentReference = Initializer::attachmentReference(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    std::vector<VkSubpassDescription> subpasses(1);
    subpasses[0] = Initializer::subpassDescription(colorAttachmentReferences, &depthStencilAttachmentReference);

    std::vector<VkSubpassDependency> dependencies(1);
    dependencies[1] = Initializer::subpassDependency(
        VK_SUBPASS_EXTERNAL, 0,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);

    auto renderPassCreateInfo = Initializer::renderPassCreateInfo(attachmentDescriptions, subpasses, dependencies);
    THROW_IF_FAILED(vkCreateRenderPass(context.device.handle, &renderPassCreateInfo, context.allocator, &renderPass));
}
