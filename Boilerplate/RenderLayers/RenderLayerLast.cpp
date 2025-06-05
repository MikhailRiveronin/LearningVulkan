#include "RenderLayerLast.h"
#include "Boilerplate/Initializer.h"
#include "Boilerplate/Utils.h"

RenderLayerLast::RenderLayerLast(Globals const& globals)
{
    createRenderPass(globals);
}

void RenderLayerLast::recordRenderPass(Globals const& globals, VkCommandBuffer commandBuffer, u32 swapchainImageIndex)
{
    VkRect2D renderArea;
    renderArea.offset = { 0, 0 };
    renderArea.extent = globals.swapchain.extent;

    std::vector<VkClearValue> clearValues(2);
    clearValues[0].color = {{ 1.f, 1.f, 1.f, 1.f }};
    clearValues[1].depthStencil = { 1.f, 0 };

    auto renderPassBeginInfo = Initializer::renderPassBeginInfo(
        renderPass,
        globals.swapchain.framebuffers[swapchainImageIndex],
        renderArea, clearValues);
    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdEndRenderPass(commandBuffer);
}

void RenderLayerLast::createRenderPass(Globals const& globals)
{
    std::vector<VkAttachmentDescription> attachmentDescriptions(2);
    attachmentDescriptions[0] = Initializer::attachmentDescription(
        0, globals.swapchain.format.format, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    attachmentDescriptions[1] = Initializer::attachmentDescription(
        0, globals.swapchain.depthStencilBuffer.format, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    std::vector<VkAttachmentReference> inputAttachmentReferences;
    std::vector<VkAttachmentReference> colorAttachmentReferences(1);
    std::vector<VkAttachmentReference> resolveAttachmentReferences;
    std::vector<u32> preserveAttachmentReferences;
    colorAttachmentReferences[0] = Initializer::attachmentReference(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    auto depthStencilAttachmentReference = Initializer::attachmentReference(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    std::vector<VkSubpassDescription> subpasses(1);
    subpasses[0] = Initializer::subpassDescription(
        0, VK_PIPELINE_BIND_POINT_GRAPHICS,
        inputAttachmentReferences, colorAttachmentReferences, resolveAttachmentReferences,
        &depthStencilAttachmentReference, preserveAttachmentReferences);

    std::vector<VkSubpassDependency> dependencies(1);
    dependencies[1] = Initializer::subpassDependency(
        VK_SUBPASS_EXTERNAL, 0,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);

    auto renderPassCreateInfo = Initializer::renderPassCreateInfo(attachmentDescriptions, subpasses, dependencies);
    THROW_IF_FAILED(
        vkCreateRenderPass(globals.device.handle, &renderPassCreateInfo, globals.allocator, &renderPass),
        __FILE__, __LINE__,
        "Failed to create render pass");
}
