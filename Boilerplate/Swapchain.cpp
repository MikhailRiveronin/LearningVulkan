#include "Swapchain.h"
#include "Utils.h"

#include <set>

// #include <algorithm>

void Swapchain::create(Globals& globals)
{
    auto imageCount = globals.swapchain.support.capabilities.minImageCount + 1;
    u32 uniqueQueueFamilyIndices[] = { globals.device.queues.graphics.index, globals.device.queues.present.index };
    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.surface = globals.surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = globals.swapchain.format.format;
    createInfo.imageColorSpace = globals.swapchain.format.colorSpace;
    createInfo.imageExtent = globals.swapchain.extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (globals.device.queues.graphics.index != globals.device.queues.present.index) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = uniqueQueueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }
    createInfo.preTransform = globals.swapchain.support.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = globals.swapchain.presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    THROW_IF_FAILED(
        vkCreateSwapchainKHR(globals.device.handle, &createInfo, globals.allocator, &globals.swapchain.handle),
        __FILE__, __LINE__,
        "Failed to create swap chain");

    imageCount = 0;
    THROW_IF_FAILED(
        vkGetSwapchainImagesKHR(globals.device.handle, globals.swapchain.handle, &imageCount, nullptr),
        __FILE__, __LINE__,
        "Failed to get swap chain images");
    globals.swapchain.images.resize(imageCount);
    THROW_IF_FAILED(
        vkGetSwapchainImagesKHR(globals.device.handle, globals.swapchain.handle, &imageCount, globals.swapchain.images.data()),
        __FILE__, __LINE__,
        "Failed to get swap chain images");

    globals.swapchain.imageViews.resize(imageCount);
    for (size_t i = 0; i < imageCount; ++i) {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.image = globals.swapchain.images[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = globals.swapchain.format.format;
        VkComponentMapping componentMapping = {};
        createInfo.components = componentMapping;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        THROW_IF_FAILED(
            vkCreateImageView(globals.device.handle, &createInfo, globals.allocator, &globals.swapchain.imageViews[i]),
            __FILE__, __LINE__,
            "Failed to create image view");
    }
    LOG_DEBUG("Swapchain successfully created");

    globals.swapchain.depthStencilBuffer.width = globals.swapchain.extent.width;
    globals.swapchain.depthStencilBuffer.height = globals.swapchain.extent.height;
    globals.swapchain.depthStencilBuffer.format = globals.swapchain.depthStencilBuffer.format;
    globals.swapchain.depthStencilBuffer.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    globals.swapchain.depthStencilBuffer.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    globals.swapchain.depthStencilBuffer.aspect = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

    createImage(globals, globals.swapchain.depthStencilBuffer);
    createImageView(globals, globals.swapchain.depthStencilBuffer);
    LOG_DEBUG("Depth buffer successfully created");

    globals.swapchain.framebuffers.resize(globals.swapchain.imageViews.size());

    for (u32 i = 0; i < globals.swapchain.imageViews.size(); ++i) {
        std::vector<VkImageView> attachments = { globals.swapchain.imageViews[i], globals.swapchain.depthStencilBuffer.view };
        VkFramebufferCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.renderPass = globals.renderPass;
        createInfo.attachmentCount = attachments.size();
        createInfo.pAttachments = attachments.data();
        createInfo.width = globals.swapchain.extent.width;
        createInfo.height = globals.swapchain.extent.height;
        createInfo.layers = 1;

        THROW_IF_FAILED(
            vkCreateFramebuffer(globals.device.handle, &createInfo, globals.allocator, &globals.swapchain.framebuffers[i]),
            __FILE__, __LINE__,
            "Failed to create framebuffer");
    }
    LOG_DEBUG("Framebuffer successfully created");
}

void Swapchain::destroy(Globals const& globals)
{
    for (auto framebuffer : globals.swapchain.framebuffers) {
        vkDestroyFramebuffer(globals.device.handle, framebuffer, globals.allocator);
    }
    LOG_DEBUG("Framebuffer destroyed");
    destroyImage(globals, globals.swapchain.depthStencilBuffer);
    vkDestroyImageView(globals.device.handle, globals.swapchain.depthStencilBuffer.view, globals.allocator);
    LOG_DEBUG("Depth buffer destroyed");
    for (auto imageView : globals.swapchain.imageViews) {
        vkDestroyImageView(globals.device.handle, imageView, globals.allocator);
    }
    LOG_DEBUG("Swapchain image views destroyed");
    vkDestroySwapchainKHR(globals.device.handle, globals.swapchain.handle, globals.allocator);
    LOG_DEBUG("Swapchain destroyed");
}
