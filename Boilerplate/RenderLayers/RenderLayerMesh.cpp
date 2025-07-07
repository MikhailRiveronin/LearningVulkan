#include "RenderLayerMesh.h"
#include "Initializer.h"
#include "Utils.h"

RenderLayerMesh::RenderLayerMesh(
    Context const& globals,
    std::vector<DescriptorSetBinding> const& descriptorSetCreateInfos,
    std::vector<PipelineCreateInfo> const& pipelineCreateInfos)
{
    createRenderPass(globals);
    createFramebuffers(globals);
    createDescriptorSets(globals, descriptorSetCreateInfos, descriptorSets);
    createPipelines(globals, pipelineCreateInfos);
}

void RenderLayerMesh::recordCommandBuffer(Context const& globals, VkCommandBuffer commandBuffer, u32 swapchainImage)
{
    VkRect2D renderArea = { { 0, 0 }, globals.swapchain.extent };
    auto renderPassBeginInfo = Initializer::renderPassBeginInfo(renderPass, framebuffers[swapchainImage], renderArea);
    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    auto viewport = Initializer::viewport(globals.swapchain.extent);
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    auto scissor = Initializer::scissor(globals.swapchain.extent);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    std::vector<VkDeviceSize> offsets(1);
    offsets[0] = 0;
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer.handle, offsets.data());
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer.handle, 0, VK_INDEX_TYPE_UINT32);

    for (u32 i = 0; i < pipelines.size(); ++i) {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines[i].handle);        
        vkCmdBindDescriptorSets(
            commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines[i].layout,
            0, 1, &descriptorSets.handles[swapchainImage], 0, nullptr);
        

    }



}

void RenderLayerMesh::createRenderPass(Context const& globals)
{
    std::vector<VkAttachmentDescription> attachmentDescriptions(2);
    attachmentDescriptions[0] = Initializer::attachmentDescription(
        0, globals.swapchain.format.format, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    attachmentDescriptions[1] = Initializer::attachmentDescription(
        0, globals.swapchain.depthStencilBuffer.format, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

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
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);

    auto renderPassCreateInfo = Initializer::renderPassCreateInfo(attachmentDescriptions, subpasses, dependencies);
    THROW_IF_FAILED(
        vkCreateRenderPass(globals.device.handle, &renderPassCreateInfo, globals.allocator, &renderPass),
        __FILE__, __LINE__,
        "Failed to create render pass");
}

void RenderLayerMesh::createFramebuffers(Context const& globals)
{
    framebuffers.resize(globals.swapchain.imageViews.size());
    for (u32 i = 0; i < framebuffers.size(); ++i) {
        std::vector<VkImageView> attachments(2);
        attachments[0] = globals.swapchain.imageViews[i];
        attachments[1] = globals.swapchain.depthStencilBuffer.view;
        auto createInfo = Initializer::framebufferCreateInfo(renderPass, attachments, globals.swapchain.extent);
        THROW_IF_FAILED(
            vkCreateFramebuffer(globals.device.handle, &createInfo, globals.allocator, &framebuffers[i]),
            __FILE__, __LINE__,
            "Failed to create framebuffer");
    }
}

void RenderLayerMesh::createPipelines(Context const& globals, std::vector<PipelineCreateInfo> const& pipelineCreateInfos)
{
    pipelines.resize(pipelineCreateInfos.size());
    std::vector<VkGraphicsPipelineCreateInfo> createInfos(pipelineCreateInfos.size());
    for (u32 i = 0; i < pipelines.size(); ++i) {
        createDescriptorSets(globals, pipelineCreateInfos[i].descriptorSetCreateInfos, pipelines[i].descriptorSets);

        auto vertexInputState = Initializer::pipelineVertexInputStateCreateInfo(
            pipelineCreateInfos[i].vertexBindingDescriptions,
            pipelineCreateInfos[i].vertexAttributeDescriptions);

        auto inputAssemblyState = Initializer::pipelineInputAssemblyStateCreateInfo();
        auto tessellationState = Initializer::pipelineTessellationStateCreateInfo();

        std::vector<VkViewport> viewports(1);
        viewports[0] = Initializer::viewport(globals.swapchain.extent);
        std::vector<VkRect2D> scissors(1);
        scissors[0] = Initializer::scissor(globals.swapchain.extent);
        auto viewportState = Initializer::pipelineViewportStateCreateInfo(viewports, scissors);

        auto rasterizationState = Initializer::pipelineRasterizationStateCreateInfo();
        auto multisampleState = Initializer::pipelineMultisampleStateCreateInfo();
        auto depthStencilState = Initializer::pipelineDepthStencilStateCreateInfo();

        std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentStates(1);
        colorBlendAttachmentStates[0] = Initializer::pipelineColorBlendAttachmentState();
        auto colorBlendState = Initializer::pipelineColorBlendStateCreateInfo(colorBlendAttachmentStates);

        std::vector<VkDynamicState> dynamicStates(2);
        dynamicStates[0] = VK_DYNAMIC_STATE_VIEWPORT;
        dynamicStates[1] = VK_DYNAMIC_STATE_SCISSOR;
        auto dynamicState = Initializer::pipelineDynamicStateCreateInfo(dynamicStates);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts(2);
        descriptorSetLayouts[0] = descriptorSets.setLayout;
        descriptorSetLayouts[1] = pipelines[i].descriptorSets.setLayout;
        std::vector<VkPushConstantRange> pushConstantRanges(1);
        pushConstantRanges[0] = pipelines[i].pushConstantRanges;
        auto pipelineLayoutCreateInfo = Initializer::pipelineLayoutCreateInfo(descriptorSetLayouts, pushConstantRanges);
        THROW_IF_FAILED(
            vkCreatePipelineLayout(globals.device.handle, &pipelineLayoutCreateInfo, globals.allocator, &pipelines[i].layout),
            __FILE__, __LINE__,
            "Failed to create pipeline layout");

        createInfos[i] = Initializer::graphicsPipelineCreateInfo(
            pipelineCreateInfos[i].stages,
            &vertexInputState,
            &inputAssemblyState,
            &tessellationState,
            &viewportState,
            &rasterizationState,
            &multisampleState,
            &depthStencilState,
            &colorBlendState,
            &dynamicState,
            pipelines[i].layout,
            globals.renderPass);
        THROW_IF_FAILED(
            vkCreateGraphicsPipelines(globals.device.handle, VK_NULL_HANDLE, 1, &createInfos[i], globals.allocator, &pipelines[i].handle),
            __FILE__, __LINE__,
            "Failed to create graphics pipeline");
    }
}
