#include "SampleBase.h"

#include "Logger.h"
#include "Utils.h"

#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

constexpr const u32 framesInFlight = 2;

static void glfwErrorCallback(int error, char const* description);
static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
static void checkLayerSupport(std::vector<char const*> const& requiredLayers);
static void checkExtensionSupport(std::vector<char const*> const& requiredExtensions);

SampleBase::SampleBase(u32 width, u32 height, std::string const& name) :
    width(width),
    height(height),
    name(name),
    framebufferResized(false)
{
}

void SampleBase::run()
{
    startup(width, height, name);
    update();
    shutdown();
}

void SampleBase::startup(u32 width, u32 height, std::string const& name)
{
    createWindow(width, height, name);
    createInstance();
#ifdef _DEBUG
    messenger.create(context);
#endif
    createSurface();
    device.create(context);
    swapchain.create(context, window);
    createRenderPass();
    createFramebuffers();
    createGraphicsPipeline();
    createCommandPool();
    allocateCommandBuffer();
    createSynchronizationObjects();

}

void SampleBase::update()
{
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        drawFrame();
    }
}

void SampleBase::shutdown()
{
    vkDeviceWaitIdle(context.device.device);

    destroySynchronizationObjects();
    vkDestroyCommandPool(context.device.device, context.commandBuffer.pool, context.allocator);

    for (auto& framebuffer : context.swapchain.framebuffers) {
        vkDestroyFramebuffer(context.device.device, framebuffer, context.allocator);
    }
    LOG_DEBUG("Framebuffers destroyed");

    vkDestroyRenderPass(context.device.device, context.renderPass, context.allocator);
    vkDestroyPipelineLayout(context.device.device, context.graphicsPipeline.layout, context.allocator);
    vkDestroyPipeline(context.device.device, context.graphicsPipeline.pipeline, context.allocator);

    swapchain.destroy(context);
    device.destroy(context);

    vkDestroySurfaceKHR(context.instance, context.surface, context.allocator);
    LOG_DEBUG("Surface destroyed");

#ifdef _DEBUG
    messenger.destroy(context);
#endif

    vkDestroyInstance(context.instance, context.allocator);
    LOG_DEBUG("Instance destroyed");

    glfwDestroyWindow(window);
    LOG_DEBUG("Window destroyed");

    glfwTerminate();
    LOG_DEBUG("GLFW terminate");
}

void SampleBase::createWindow(u32 width, u32 height, std::string const& name)
{
    glfwSetErrorCallback(glfwErrorCallback);

    if (!glfwInit()) {
        LOG_ERROR("Failed to initialize GLFW");
        throw std::runtime_error("Failed to initialize GLFW");
    }
    LOG_DEBUG("GLFW successfully initialized");

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
    if (!window) {
        LOG_ERROR("Failed to create window");
        glfwTerminate();
        throw std::runtime_error("Failed to create window");
    }
    LOG_DEBUG("Window successfully created");

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
}

void SampleBase::createInstance()
{
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = "Application";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    std::vector<char const*> requiredLayers;
#ifdef _DEBUG
    requiredLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif

    checkLayerSupport(requiredLayers);

    std::vector<char const*> requiredExtensions;
    u32 glfwExtensionCount = 0;
    char const** glfwExtensionNames = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    for (size_t i = 0; i < glfwExtensionCount; ++i) {
        requiredExtensions.push_back(glfwExtensionNames[i]);
    }
#ifdef _DEBUG
    requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    checkExtensionSupport(requiredExtensions);

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledLayerCount = requiredLayers.size();
    createInfo.ppEnabledLayerNames = requiredLayers.data();
    createInfo.enabledExtensionCount = requiredExtensions.size();
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();

    THROW_IF_FAILED(vkCreateInstance(&createInfo, context.allocator, &context.instance), __FILE__, __LINE__, "Failed to create an instance");
    LOG_DEBUG("Instance successfully created");
}

void SampleBase::createSurface()
{
    THROW_IF_FAILED(glfwCreateWindowSurface(context.instance, window, context.allocator, &context.surface), __FILE__, __LINE__, "Failed to create window surface");
    LOG_DEBUG("Surface successfully created");
}

void SampleBase::createRenderPass()
{
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.flags = 0;
    colorAttachment.format = context.swapchain.format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentReference;
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass;
    subpass.flags = 0;
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = nullptr;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentReference;
    subpass.pResolveAttachments = nullptr;
    subpass.pDepthStencilAttachment = nullptr;
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = nullptr;

    VkSubpassDependency subpassDependency = {};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.srcAccessMask = 0;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpassDependency.dependencyFlags = 0;

    VkRenderPassCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = &colorAttachment;
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpass;
    createInfo.dependencyCount = 1;
    createInfo.pDependencies = &subpassDependency;

    THROW_IF_FAILED(vkCreateRenderPass(context.device.device, &createInfo, context.allocator, &context.renderPass), __FILE__, __LINE__, "Failed to create render pass");
}

void SampleBase::createFramebuffers()
{
    context.swapchain.framebuffers.resize(context.swapchain.imageViews.size());

    for (u32 i = 0; i < context.swapchain.imageViews.size(); ++i) {
        VkFramebufferCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.renderPass = context.renderPass;
        createInfo.attachmentCount = 1;
        createInfo.pAttachments = &context.swapchain.imageViews[i];
        createInfo.width = context.swapchain.extent.width;
        createInfo.height = context.swapchain.extent.height;
        createInfo.layers = 1;

        THROW_IF_FAILED(vkCreateFramebuffer(context.device.device, &createInfo, context.allocator, &context.swapchain.framebuffers[i]), __FILE__, __LINE__, "Failed to create framebuffer");
    }
}

void SampleBase::createCommandPool()
{
    VkCommandPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    createInfo.queueFamilyIndex = context.device.queueFamilyIndices.graphics.value();

    THROW_IF_FAILED(vkCreateCommandPool(context.device.device, &createInfo, context.allocator, &context.commandBuffer.pool), __FILE__, __LINE__, "Failed to create command pool");
}

void SampleBase::allocateCommandBuffer()
{
    context.commandBuffer.buffers.resize(framesInFlight);

    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.commandPool = context.commandBuffer.pool;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = context.commandBuffer.buffers.size();

    THROW_IF_FAILED(vkAllocateCommandBuffers(context.device.device, &allocateInfo, context.commandBuffer.buffers.data()), __FILE__, __LINE__, "Failed to allocate command buffers");
}

void SampleBase::createSynchronizationObjects()
{
    context.synchronization.imageAvailableSemaphores.resize(framesInFlight);
    context.synchronization.renderFinishedSemaphores.resize(framesInFlight);
    context.synchronization.fences.resize(framesInFlight);

    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCreateInfo.pNext = nullptr;
    semaphoreCreateInfo.flags = 0;

    for (u32 i = 0; i < framesInFlight; ++i) {
        THROW_IF_FAILED(vkCreateSemaphore(context.device.device, &semaphoreCreateInfo, context.allocator, &context.synchronization.imageAvailableSemaphores[i]), __FILE__, __LINE__, "Failed to create semaphore");
        THROW_IF_FAILED(vkCreateSemaphore(context.device.device, &semaphoreCreateInfo, context.allocator, &context.synchronization.renderFinishedSemaphores[i]), __FILE__, __LINE__, "Failed to create semaphore");
    }

    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.pNext = nullptr;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (u32 i = 0; i < framesInFlight; ++i) {
        THROW_IF_FAILED(vkCreateFence(context.device.device, &fenceCreateInfo, context.allocator, &context.synchronization.fences[i]), __FILE__, __LINE__, "Failed to create fence");
    }
}

void SampleBase::drawFrame()
{
    static u32 frameIndex = 0;

    if (framebufferResized) {
        swapchain.recreate(context, window);
        createFramebuffers();
        framebufferResized = false;
    }

    THROW_IF_FAILED(vkWaitForFences(context.device.device, 1, &context.synchronization.fences[frameIndex], VK_TRUE, UINT64_MAX), __FILE__, __LINE__, "Failed to wait for fences");
    
    u32 imageIndex;
    auto result = vkAcquireNextImageKHR(context.device.device, context.swapchain.swapchain, UINT64_MAX, context.synchronization.imageAvailableSemaphores[frameIndex], VK_NULL_HANDLE, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        return;
    } else {
        THROW_IF_FAILED(result, __FILE__, __LINE__, "Failed to acquire next image");
    }

    THROW_IF_FAILED(vkResetFences(context.device.device, 1, &context.synchronization.fences[frameIndex]), __FILE__, __LINE__, "Failed to reset fences");

    vkResetCommandBuffer(context.commandBuffer.buffers[frameIndex], 0);
    recordCommandBuffer(context.commandBuffer.buffers[frameIndex], imageIndex);

    std::vector<VkPipelineStageFlags> pipelineStageFlags;
    pipelineStageFlags.push_back(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &context.synchronization.imageAvailableSemaphores[frameIndex];
    submitInfo.pWaitDstStageMask = pipelineStageFlags.data();
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &context.commandBuffer.buffers[frameIndex];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &context.synchronization.renderFinishedSemaphores[frameIndex];

    THROW_IF_FAILED(vkQueueSubmit(context.device.queues.graphics, 1, &submitInfo, context.synchronization.fences[frameIndex]), __FILE__, __LINE__, "Failed to queue submit");

    VkPresentInfoKHR presentInfo;
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &context.synchronization.renderFinishedSemaphores[frameIndex];
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &context.swapchain.swapchain;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;
    
    frameIndex = (frameIndex + 1) % framesInFlight;

    result = vkQueuePresentKHR(context.device.queues.present, &presentInfo);
    // if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
    //     swapchain.recreate(context, window);
    //     createFramebuffers();
    // } else {
    //     THROW_IF_FAILED(result, __FILE__, __LINE__, "Failed to present");
    // }
}

void SampleBase::destroySynchronizationObjects()
{
    for (u32 i = 0; i < framesInFlight; ++i) {
        vkDestroySemaphore(context.device.device, context.synchronization.imageAvailableSemaphores[i], context.allocator);
        vkDestroySemaphore(context.device.device, context.synchronization.renderFinishedSemaphores[i], context.allocator);
        vkDestroyFence(context.device.device, context.synchronization.fences[i], context.allocator);
    }
}

void glfwErrorCallback(int error, char const* message)
{
    std::cerr << error << ": " << message << std::endl;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    auto app = (SampleBase*)glfwGetWindowUserPointer(window);
    app->framebufferResized = true;
}

void checkLayerSupport(std::vector<char const*> const& requiredLayers)
{
    char buffer[1024];
    sprintf(buffer, "Required layers: ");
    char const* indent = "                 ";
    for (u32 i = 0; i < requiredLayers.size(); ++i) {
        if (i == 0) {
            sprintf(buffer + strlen(buffer), "%s\n", requiredLayers[i]);
            continue;
        }

        sprintf(buffer + strlen(buffer), "%s%s%s\n", infoIndent, indent, requiredLayers[i]);
    }
    LOG_INFO(buffer);

    u32 availableLayerCount;
    THROW_IF_FAILED(vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr), __FILE__, __LINE__, "Failed to enumerate instance layer properties");
    std::vector<VkLayerProperties> availableLayers(availableLayerCount);
    THROW_IF_FAILED(vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data()), __FILE__, __LINE__, "Failed to enumerate instance layer properties");

    sprintf(buffer, "Available layers: ");
    indent = "                  ";
    for (u32 i = 0; i < availableLayerCount; ++i) {
        if (i == 0) {
            sprintf(buffer + strlen(buffer), "%s\n", availableLayers[i].layerName);
            continue;
        }

        sprintf(buffer + strlen(buffer), "%s%s%s\n", infoIndent, indent, availableLayers[i].layerName);
    }
    LOG_INFO(buffer);

    for (auto& layer : requiredLayers) {
        if (std::find_if(availableLayers.begin(), availableLayers.end(), [&](VkLayerProperties const& layerProperties) {
            return strcmp(layer, layerProperties.layerName) == 0;
        }) == availableLayers.end()) {
            LOG_ERROR("Layer '%s' is not supported", layer);
            throw std::runtime_error("Required layer is not supported");
        }
    }
}

void checkExtensionSupport(std::vector<char const*> const& requiredExtensions)
{
    char buffer[4096];
    sprintf(buffer, "Required instance level extensions: ");
    char const* indent = "                                    ";
    for (u32 i = 0; i < requiredExtensions.size(); ++i) {
        if (i == 0) {
            sprintf(buffer + strlen(buffer), "%s\n", requiredExtensions[i]);
            continue;
        }

        sprintf(buffer + strlen(buffer), "%s%s%s\n", infoIndent, indent, requiredExtensions[i]);
    }
    LOG_INFO(buffer);

    u32 availableExtensionCount;
    THROW_IF_FAILED(vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr), __FILE__, __LINE__, "Failed to enumerate instance extension properties");
    std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
    THROW_IF_FAILED(vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, availableExtensions.data()), __FILE__, __LINE__, "Failed to enumerate instance extension properties");

    sprintf(buffer, "Available instance level extensions: ");
    indent = "                                     ";
    for (u32 i = 0; i < availableExtensionCount; ++i) {
        if (i == 0) {
            sprintf(buffer + strlen(buffer), "%s\n", availableExtensions[i].extensionName);
            continue;
        }

        sprintf(buffer + strlen(buffer), "%s%s%s\n", infoIndent, indent, availableExtensions[i].extensionName);
    }
    LOG_INFO(buffer);

    for (auto& extension : requiredExtensions) {
        if (std::find_if(availableExtensions.begin(), availableExtensions.end(), [&](const VkExtensionProperties& extensionProperties) {
            return strcmp(extension, extensionProperties.extensionName) == 0;
        }) == availableExtensions.end()) {
            LOG_ERROR("Extension '%s' is not supported", extension);
            throw std::runtime_error("Required extension is not supported");
        }
    }
}
