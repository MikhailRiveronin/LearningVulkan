#include "window.h"

#include "vulkan_struct_initializers.h"
#include "glm/glm.hpp"

LRESULT CALLBACK window_proc(HWND h_wnd, UINT message, WPARAM w_param, LPARAM l_param);

Window::Window(u32 width, u32 height) : width(width), height(height)
{
    WNDCLASSEX wnd_class = {};
    wnd_class.cbSize = sizeof(wnd_class);
    wnd_class.style = 0;
    wnd_class.lpfnWndProc = windowProc;
    wnd_class.cbClsExtra = 0;
    wnd_class.cbWndExtra = 0;
    wnd_class.hInstance = GetModuleHandle(nullptr);
    wnd_class.hIcon = LoadIconA(nullptr, IDI_APPLICATION);
    wnd_class.hCursor = LoadCursorA(nullptr, IDC_ARROW);
    wnd_class.hbrBackground = nullptr;
    wnd_class.lpszMenuName = nullptr;
    wnd_class.lpszClassName = L"Class Name";
    wnd_class.hIconSm = nullptr;
    RegisterClassEx(&wnd_class);

    RECT rect = { 0, 0, width, height };
    DWORD style = WS_OVERLAPPEDWINDOW;
    DWORD ex_style = WS_EX_APPWINDOW;
    AdjustWindowRect(&rect, style, FALSE);
    auto h_wnd = CreateWindowEx(ex_style, L"Class Name", L"Window Name", style, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, h_instance, nullptr);

    VkWin32SurfaceCreateInfoKHR win32_surface_create_info = Vulkan_Struct_Initializers::win32_surface_create_info(h_wnd);
    VK_CHECK(vkCreateWin32SurfaceKHR(device->instance, &win32_surface_create_info, nullptr, &surface));

    VkBool32 is_present_supported = VK_FALSE;
    for (u32 i = 0; i < device->queue_family_properties.size(); ++i)
    {
        VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(device->physical_device, i, surface, &is_present_supported));
        if (is_present_supported)
        {
            device->queues.present.family_index = i;
        }
    }

    vkGetDeviceQueue(device->handle, device->queues.present.family_index, 0, &device->queues.present.handle);

    // Choose surface format
    u32 surface_format_count;
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device->physical_device, surface, &surface_format_count, nullptr));
    std::vector<VkSurfaceFormatKHR> surface_formats(surface_format_count);
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device->physical_device, surface, &surface_format_count, surface_formats.data()));

    surface_format = surface_formats[0];
    for (auto& format : surface_formats)
    {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            surface_format = format;
            break;
        }
    }

    ShowWindow(h_wnd, SW_SHOWNORMAL);







}

void Window::pump_messages()
{
    MSG msg;
    while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}


void create_swapchain()
{
    VkSurfaceCapabilitiesKHR surface_capabilities;
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &surface_capabilities));
    width = glm::clamp(width, surface_capabilities.minImageExtent.width, surface_capabilities.maxImageExtent.width);
    height = glm::clamp(height, surface_capabilities.minImageExtent.height, surface_capabilities.maxImageExtent.height);

    uint32_t present_mode_count;
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, nullptr));
    std::vector<VkPresentModeKHR> present_modes(present_mode_count);
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, present_modes.data()));
    VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;

    std::set<u32> unique_queue_family_indices = { device->queues.graphics.family_index, device->queues.present.family_index };
    std::vector<u32> queue_family_indices(unique_queue_family_indices.begin(), unique_queue_family_indices.end());
    auto create_info = Vulkan_Struct_Initializers::swapchain_create_info(surface, std::min(surface_capabilities.minImageCount + 1, surface_capabilities.maxImageCount), surface_format.format, surface_format.colorSpace, { width, height }, queue_family_indices, surface_capabilities.currentTransform, present_mode);
    VK_CHECK(vkCreateSwapchainKHR(device->handle, &create_info, nullptr, &swapchain));

    u32 image_count = 0;
    VK_CHECK(vkGetSwapchainImagesKHR(device->handle, swapchain, &image_count, nullptr));
    swapchain_images.resize(image_count);
    VK_CHECK(vkGetSwapchainImagesKHR(device->handle, swapchain, &image_count, swapchain_images.data()));

    swapchain_image_views.resize(swapchain_images.size());
    for (u32 i = 0; i < swapchain_image_views.size(); ++i)
    {
        auto image_view_create_info = Vulkan_Struct_Initializers::image_view_create_info(swapchain_images[i], surface_format.format);
        VK_CHECK(vkCreateImageView(device->handle, &image_view_create_info, nullptr, &swapchain_image_views[i]));
    }

    auto depth_stencil_buffer_create_info = Vulkan_Struct_Initializers::image_create_info(VK_FORMAT_D24_UNORM_S8_UINT, width, height, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    VmaAllocationCreateInfo depth_stencil_buffer_allocation_create_info = {};
    depth_stencil_buffer_allocation_create_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    VK_CHECK(vmaCreateImage(device->allocator, &depth_stencil_buffer_create_info, &depth_stencil_buffer_allocation_create_info, &depth_stencil_buffer.handle, &depth_stencil_buffer.allocation, 0));

    auto depth_stencil_buffer_view_create_info = Vulkan_Struct_Initializers::image_view_create_info(depth_stencil_buffer.handle, VK_FORMAT_D24_UNORM_S8_UINT);
    VK_CHECK(vkCreateImageView(device->handle, &depth_stencil_buffer_view_create_info, nullptr, &depth_stencil_buffer.view));

    framebuffers.resize(swapchain_image_views.size());
    for (u32 i = 0; i < framebuffers.size(); ++i)
    {
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

        VK_CHECK(
            vkCreateFramebuffer(globals.device.handle, &createInfo, globals.allocator, &globals.swapchain.framebuffers[i]),
            __FILE__, __LINE__,
            "Failed to create framebuffer");
    }
}














void Swapchain::create(Context& globals)
{
    auto imageCount = globals.swapchain.support.capabilities.minImageCount + 1;
    u32 uniqueQueueFamilyIndices[] = { globals.device.queues.graphics.familyIndex, globals.device.queues.present.familyIndex };
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
    if (globals.device.queues.graphics.familyIndex != globals.device.queues.present.familyIndex) {
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

    VK_CHECK(
        vkCreateSwapchainKHR(globals.device.handle, &createInfo, globals.allocator, &globals.swapchain.handle),
        __FILE__, __LINE__,
        "Failed to create swap chain");

    imageCount = 0;
    VK_CHECK(
        vkGetSwapchainImagesKHR(globals.device.handle, globals.swapchain.handle, &imageCount, nullptr),
        __FILE__, __LINE__,
        "Failed to get swap chain images");
    globals.swapchain.images.resize(imageCount);
    VK_CHECK(
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
        VK_CHECK(
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
    globals.swapchain.depthStencilBuffer.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

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

        VK_CHECK(
            vkCreateFramebuffer(globals.device.handle, &createInfo, globals.allocator, &globals.swapchain.framebuffers[i]),
            __FILE__, __LINE__,
            "Failed to create framebuffer");
    }
    LOG_DEBUG("Framebuffer successfully created");
}

void Swapchain::destroy(Context const& globals)
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
