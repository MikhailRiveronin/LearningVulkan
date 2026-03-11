#pragma once

#include <vulkan/vulkan.h>
#include <vector>

struct RenderPass {
    VkRenderPass handle;
    std::vector<VkAttachmentDescription> colorAttachments;
    VkAttachmentDescription depthStencilAttachment;

};
