#pragma once

#include "RenderLayerBase.h"
#include "Boilerplate/Structures.h"

class RenderLayerFirst : public RenderLayerBase {
public:
    RenderLayerFirst(Context const& globals);
    void recordRenderPass(Context const& globals, VkCommandBuffer commandBuffer, u32 swapchainImageIndex) override;

private:
    void createRenderPass(Context const& context) override;
    void createFramebuffers(Context const& context) override;
};
