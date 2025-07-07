#include "RenderLayerFirst.h"
#include "Boilerplate/Initializer.h"
#include "Boilerplate/Utils.h"

RenderLayerFirst::RenderLayerFirst(Context const& globals)
{
    createRenderPass(globals);
    createFramebuffers(globals);
}

void RenderLayerFirst::recordRenderPass(Context const& globals, VkCommandBuffer commandBuffer, u32 swapchainImageIndex)
{
    VkRect2D renderArea;
    renderArea.offset = { 0, 0 };
    renderArea.extent = globals.swapchain.extent;

    std::vector<VkClearValue> clearValues(2);
    clearValues[0].color = {{ 0.f, 0.f, 0.f, 1.f }};
    clearValues[1].depthStencil = { 1.f, 0 };

    auto renderPassBeginInfo = Initializer::renderPassBeginInfo(
        renderPass,
        globals.swapchain.framebuffers[swapchainImageIndex],
        renderArea, clearValues);
    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdEndRenderPass(commandBuffer);
}

void RenderLayerFirst::createRenderPass(Context const& context)
{
    std::vector<VkAttachmentDescription> attachmentDescriptions(2);
    attachmentDescriptions[0] = Initializer::attachmentDescription(
        context.swapchain.format.format,
        VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    attachmentDescriptions[1] = Initializer::attachmentDescription(
        context.swapchain.depthStencilBuffer.format,
        VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    std::vector<VkAttachmentReference> colorAttachmentReferences(1);
    colorAttachmentReferences[0] = Initializer::attachmentReference(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    auto depthStencilAttachmentReference = Initializer::attachmentReference(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    std::vector<VkSubpassDescription> subpasses(1);
    subpasses[0] = Initializer::subpassDescription(colorAttachmentReferences, &depthStencilAttachmentReference);

    std::vector<VkSubpassDependency> dependencies(1);
    dependencies[1] = Initializer::subpassDependency(
        VK_SUBPASS_EXTERNAL, 0,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);

    auto renderPassCreateInfo = Initializer::renderPassCreateInfo(attachmentDescriptions, subpasses, dependencies);
    THROW_IF_FAILED(vkCreateRenderPass(context.device.handle, &renderPassCreateInfo, context.allocator, &renderPass));
}

void RenderLayerFirst::createFramebuffers(Context const& context)
{
    framebuffers.resize(context.swapchain.imageViews.size());
    for (u32 i = 0; i < framebuffers.size(); ++i) {
        std::vector<VkImageView> attachments(2);
        attachments[0] = context.swapchain.imageViews[i];
        attachments[1] = context.swapchain.depthStencilBuffer.view.handle;
        auto createInfo = Initializer::framebufferCreateInfo(renderPass, attachments, context.swapchain.extent);
        THROW_IF_FAILED(
            vkCreateFramebuffer(context.device.handle, &createInfo, context.allocator, &framebuffers[i]),
            __FILE__, __LINE__,
            "Failed to create framebuffer");
    }
}
