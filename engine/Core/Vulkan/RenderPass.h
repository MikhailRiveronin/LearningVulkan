#pragma once

#include "Limits.h"

#include <vulkan/vulkan.h>

class RenderPass {
public:
    struct {
        VkFormat format;
        VkAttachmentLoadOp loadOp;
        VkImageLayout finalLayout;
    } colorAttachments[MAX_ATTACHMENT_COUNT];

    

};
