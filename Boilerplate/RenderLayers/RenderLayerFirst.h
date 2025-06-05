#pragma once

#include "RenderLayerBase.h"
#include "Boilerplate/Structures.h"

class RenderLayerFirst : public RenderLayerBase {
public:
    RenderLayerFirst(Globals const& globals);
    void recordRenderPass(Globals const& globals, VkCommandBuffer commandBuffer, u32 swapchainImageIndex) override;

private:
    void createRenderPass(Globals const& globals) override;
    void createFramebuffers(Globals const& globals) override;
};
