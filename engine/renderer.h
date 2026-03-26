#pragma once

#include "defines.h"

#include "third_party/vk_mem_alloc.h"

class Renderer
{
public:
    VkInstance instance;
    VkPhysicalDevice physical_device;
    VkDevice device;
    VmaAllocator allocator;

    struct Per_Frame_Objects
    {
        VkSemaphore image_available_semaphore;
        VkSemaphore render_finished_semaphore;
        VkFence in_flight_fence;
        VkCommandBuffer command_buffer;
    };

    std::vector<Per_Frame_Objects> per_frame_resources;
    u32 current_frame_index;

    struct {
        VkSwapchainKHR handle = VK_NULL_HANDLE;

        struct {
            VkSurfaceCapabilitiesKHR capabilities = {};
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
        } support;

        VkExtent2D extent = {};
        VkSurfaceFormatKHR format = {};
        VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;

        std::vector<VkImage> images;
        std::vector<VkImageView> image_views;
        std::vector<VkFramebuffer> framebuffers;

        u32 next_image_index;

        Image depthStencilBuffer;
    } swapchain;

    struct
    {
        struct
        {
            VkQueue handle;
            std::optional<u32> family_index;
        } graphics;

        struct
        {
            VkQueue handle;
            std::optional<u32> family_index;
        } compute;

        struct
        {
            VkQueue handle;
            std::optional<u32> family_index;
        } transfer;

        struct
        {
            VkQueue handle;
            std::optional<u32> family_index;
        } present;
    } queues;

    std::vector<Texture> textures;




    void begin_frame();

    void end_frame();
};
