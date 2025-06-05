#pragma once

#include "Defines.h"
#include "Structures.h"

#include <vulkan/vulkan.h>
#include <vector>

class RenderLayerBase {
public:
    virtual void recordRenderPass(Globals const& globals, VkCommandBuffer commandBuffer, u32 swapchainImageIndex) = 0;
    virtual void recordCommandBuffer(Globals const& globals, VkCommandBuffer commandBuffer, u32 swapchainImage) = 0;

protected:
    VkRenderPass renderPass;
    std::vector<VkFramebuffer> framebuffers;

private:
    std::vector<DescriptorSets> resourceDescriptors;
    std::vector<VkPushConstantRange> pushConstantRanges;
    std::vector<VkPipelineLayout> pipelineLayouts;
    std::vector<VkPipeline> pipelines;

    virtual void createRenderPass(Globals const& globals) = 0;
    virtual void createFramebuffers(Globals const& globals) = 0;
};
