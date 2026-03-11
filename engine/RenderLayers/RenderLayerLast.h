#pragma once

#include "RenderLayerBase.h"
#include "Boilerplate/Structures.h"

class RenderLayerLast : public RenderLayerBase {
public:
    RenderLayerLast(Context const& globals);
    void recordRenderPass(Context const& globals, VkCommandBuffer commandBuffer, u32 swapchainImageIndex) override;

private:
    void createRenderPass(Context const& globals);
};
