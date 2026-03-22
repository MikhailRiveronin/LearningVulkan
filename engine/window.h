#pragma once

#include "defines.h"

#include "device.h"
#include "texture.h"

class Window
{
public:
    Window(u32 width, u32 height);

    void pump_messages();



    // void create(Context& globals);
    // void destroy(Context const& globals);





private:
    u32 width;
    u32 height;

    Device* device;
    VkSurfaceKHR surface;
    VkSurfaceFormatKHR surface_format;
    VkSwapchainKHR swapchain;
    std::vector<VkImage> swapchain_images;
    std::vector<VkImageView> swapchain_image_views;
    std::vector<VkFramebuffer> framebuffers;

    struct
    {
        VkImage handle;
        VkImageView view;
        VmaAllocation allocation;
    } depth_stencil_buffer;

    struct Frame_Syncronization
    {
        VkSemaphore image_acquired_semaphore;
        VkSemaphore render_finished_semaphore;
        VkFence in_flight_fence;
    };
    std::vector<Frame_Syncronization> frame_syncronization_data;



    void create_swapchain();


};
