#pragma once

#include "RenderLayerBase.h"
#include "Structures.h"

class RenderLayerMesh : public RenderLayerBase {
public:
    RenderLayerMesh(
        Globals const& globals,
        std::vector<DescriptorSetBinding> const& descriptorSetBindings,
        std::vector<PipelineCreateInfo> const& pipelineCreateInfos);

    void recordCommandBuffer(Globals const& globals, VkCommandBuffer commandBuffer, u32 swapchainImage) override;

private:
    Buffer vertexBuffer;
    Buffer indexBuffer;
    DescriptorSets descriptorSets; // global, per-frame data
    std::vector<Pipeline> pipelines;

    void createRenderPass(Globals const& globals) override;
    void createFramebuffers(Globals const& globals) override;
    void createPipelines(Globals const& globals, std::vector<PipelineCreateInfo> const& pipelineCreateInfos);
};
