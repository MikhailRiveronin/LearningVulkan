#pragma once

#include "Boilerplate/RenderLayerBase.h"

class MainRenderLayer : public RenderLayerBase {
public:
    virtual void recordRenderPass(VkCommandBuffer commandBuffer, u32 swapchainImageIndex) = 0;
    virtual void updateFrameResources() = 0;

private:
    struct {

    } frameResources;


};
