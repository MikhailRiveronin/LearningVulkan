#include "Swapchain.h"

#include "Utils.h"

#include <glfw/glfw3.h>

#include <algorithm>

static VkExtent2D selectSwapchainExtent(VkSurfaceCapabilitiesKHR const& capabilities, GLFWwindow* window);
static VkSurfaceFormatKHR selectSwapchainSurfaceFormat(std::vector<VkSurfaceFormatKHR> const& formats);
static VkPresentModeKHR selectSwapchainPresentMode(std::vector<VkPresentModeKHR> const& presentModes);

void Swapchain::create(Context& context, GLFWwindow* window)
{
    auto extent = selectSwapchainExtent(context.swapchain.support.capabilities, window);
    auto format = selectSwapchainSurfaceFormat(context.swapchain.support.formats);
    auto presentMode = selectSwapchainPresentMode(context.swapchain.support.presentModes);

    auto imageCount = context.swapchain.support.capabilities.minImageCount + 1;
    if (context.swapchain.support.capabilities.maxImageCount > 0) {
        imageCount = std::clamp(imageCount, context.swapchain.support.capabilities.minImageCount, context.swapchain.support.capabilities.maxImageCount);
    }

    u32 uniqueQueueFamilyIndices[] = { context.device.queueFamilyIndices.graphics.value(), context.device.queueFamilyIndices.present.value() };

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.surface = context.surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = format.format;
    createInfo.imageColorSpace = format.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (context.device.queueFamilyIndices.graphics != context.device.queueFamilyIndices.present) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = uniqueQueueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }
    createInfo.preTransform = context.swapchain.support.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    THROW_IF_FAILED(vkCreateSwapchainKHR(context.device.handle, &createInfo, context.allocator, &context.swapchain.swapchain), __FILE__, __LINE__, "Failed to create swap chain");
    LOG_DEBUG("Swapchain successfully created");

    imageCount = 0;
    THROW_IF_FAILED(vkGetSwapchainImagesKHR(context.device.handle, context.swapchain.swapchain, &imageCount, nullptr), __FILE__, __LINE__, "Failed to get swap chain images");
    context.swapchain.images.resize(imageCount);
    THROW_IF_FAILED(vkGetSwapchainImagesKHR(context.device.handle, context.swapchain.swapchain, &imageCount, context.swapchain.images.data()), __FILE__, __LINE__, "Failed to get swap chain images");

    context.swapchain.format = format.format;
    context.swapchain.extent = extent;

    context.swapchain.imageViews.resize(imageCount);
    for (size_t i = 0; i < imageCount; ++i) {
        VkImageViewCreateInfo imageViewCreateInfo = {};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.pNext = nullptr;
        imageViewCreateInfo.flags = 0;
        imageViewCreateInfo.image = context.swapchain.images[i];
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = context.swapchain.format;
        VkComponentMapping componentMapping = {};
        imageViewCreateInfo.components = componentMapping;
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        THROW_IF_FAILED(vkCreateImageView(context.device.handle, &imageViewCreateInfo, context.allocator, &context.swapchain.imageViews[i]), __FILE__, __LINE__, "Failed to create image view");
    }
}

void Swapchain::destroy(Context& context)
{
    for (auto& imageView : context.swapchain.imageViews) {
        vkDestroyImageView(context.device.handle, imageView, context.allocator);
    }
    LOG_DEBUG("Image views destroyed");

    vkDestroySwapchainKHR(context.device.handle, context.swapchain.swapchain, context.allocator);
    LOG_DEBUG("Swapchain destroyed");
}

void Swapchain::recreate(Context& context, GLFWwindow* window)
{
    LOG_DEBUG("Swapchain recreating...");

    vkDeviceWaitIdle(context.device.handle);

    for (auto& framebuffer : context.swapchain.framebuffers) {
        vkDestroyFramebuffer(context.device.handle, framebuffer, context.allocator);
    }
    LOG_DEBUG("Framebuffers destroyed");

    destroy(context);
    create(context, window);

    LOG_DEBUG("Swapchain recreating complete");
}

VkExtent2D selectSwapchainExtent(VkSurfaceCapabilitiesKHR const& capabilities, GLFWwindow* window)
{
    if ((capabilities.currentExtent.width != 0xFFFFFFFF) && (capabilities.currentExtent.height != 0xFFFFFFFF)) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D extent = { static_cast<u32>(width), static_cast<u32>(height) };

        extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return extent;
    }
}

VkSurfaceFormatKHR selectSwapchainSurfaceFormat(std::vector<VkSurfaceFormatKHR> const& formats)
{
    for (auto& format : formats) {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return format;
        }
    }

    return formats[0];
}

VkPresentModeKHR selectSwapchainPresentMode(const std::vector<VkPresentModeKHR>& presentModes)
{
    for (auto& presentMode : presentModes) {
        if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return presentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}
