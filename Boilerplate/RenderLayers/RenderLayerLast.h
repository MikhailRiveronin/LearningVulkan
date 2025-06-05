#pragma once

#include "RenderLayerBase.h"
#include "Boilerplate/Structures.h"

class RenderLayerLast : public RenderLayerBase {
public:
    RenderLayerLast(Globals const& globals);
    void recordRenderPass(Globals const& globals, VkCommandBuffer commandBuffer, u32 swapchainImageIndex) override;

private:
    void createRenderPass(Globals const& globals);
};
