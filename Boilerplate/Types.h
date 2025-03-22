#pragma once

#include "Defines.h"

#include <vulkan/vulkan.h>

#include <optional>
#include <vector>

struct QueueFamilyIndices {
    std::optional<u32> graphics;
    std::optional<u32> compute;
    std::optional<u32> transfer;
    std::optional<u32> present;
};

struct SwapchainSupport {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct Context {
    VkAllocationCallbacks* allocator = nullptr;
    VkInstance instance;
    VkSurfaceKHR surface;
    VkRenderPass renderPass;

    struct {
        VkDevice device;
        QueueFamilyIndices queueFamilyIndices;

        struct {
            VkQueue graphics;
            VkQueue compute;
            VkQueue transfer;
            VkQueue present;
        } queues;
    } device;

    struct {
        SwapchainSupport support;
        VkSwapchainKHR swapchain;
        std::vector<VkImage> images;
        std::vector<VkImageView> imageViews;
        std::vector<VkFramebuffer> framebuffers;
        VkFormat format;
        VkExtent2D extent;
    } swapchain;

    struct {
        VkPipelineLayout layout;
        VkPipeline pipeline;
    } graphicsPipeline;

    struct {
        VkCommandPool pool;
        std::vector<VkCommandBuffer> buffers;
    } commandBuffer;

    struct {
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> fences;
    } synchronization;
};
