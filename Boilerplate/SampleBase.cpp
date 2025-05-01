#include "SampleBase.h"
#include "Logger.h"
#include "Utils.h"

#include <chrono>
#include <stdio.h>
#include <vector>

static void initRequiredLayers(std::vector<char const*>& requiredLayers);
static void checkRequiredLayersSupport(std::vector<char const*> const& requiredLayers);
static void initRequiredInstanceExtensions(std::vector<char const*>& requiredInstanceExtensions);
static void checkRequiredInstanceExtensionsSupport(std::vector<char const*> const& requiredInstanceExtensions);

SampleBase::SampleBase(u32 width, u32 height, std::string const& name) :
    width(width),
    height(height),
    name(name)
{
    globals.allocator = nullptr;
    globals.swapchain.extent = { width, height };
}

void SampleBase::onInit(HINSTANCE hInstance, HWND hWnd)
{
    createInstance();
#ifdef _DEBUG
    debugMessenger.create(globals);
#endif
    createSurface(hInstance, hWnd);
    device.create(globals);
    createRenderPass();
    swapchain.create(globals);
    createGraphicsCommandBuffers();
    createSynchronizationObjects();
    createMeshes();
    createTextures();
    createMaterials();
    createRenderObjects();
    createFrameResources();
    createResourceDescriptors();
    createPushConstantRanges();
    createPipelines();
}

void SampleBase::onUpdate()
{
    drawFrame();
}

void SampleBase::onDestroy()
{
    vkDeviceWaitIdle(globals.device.handle);

    destroyPipelines();
    destroyPushConstantRanges();
    destroyResourceDescriptors();
    destroyFrameResources();
    destroyTextures();
    destroyMeshes();
    destroySynchronizationObjects();
    destroyGraphicsCommandBuffers();
    swapchain.destroy(globals);
    destroyRenderPass();
    device.destroy(globals);
    destroySurface();
#ifdef _DEBUG
    debugMessenger.destroy(globals);
#endif
    destroyInstance();
}

void SampleBase::onNotify(EventType type, EventContext context)
{
    switch (type) {
    case EventType::WINDOW_RESIZE:
        if (globals.device.handle != VK_NULL_HANDLE) {
            vkDeviceWaitIdle(globals.device.handle);
            globals.swapchain.extent = { (u16)context.i16[0], (u16)context.i16[1] };
            destroyRenderPass();
            createRenderPass();
            swapchain.destroy(globals);
            swapchain.create(globals);
            destroyPipelines();
            createPipelines();
            LOG_DEBUG("Swapchain recreated");
        }
        return;

    default:
        return;
    }
}

void SampleBase::createInstance()
{
#ifdef _DEBUG
    debugMessenger.initCreateInfo(globals);
#endif

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = "Application";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    std::vector<char const*> requiredLayers;
    initRequiredLayers(requiredLayers);
    checkRequiredLayersSupport(requiredLayers);

    std::vector<char const*> requiredInstanceExtensions;
    initRequiredInstanceExtensions(requiredInstanceExtensions);
    checkRequiredInstanceExtensionsSupport(requiredInstanceExtensions);

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
#ifdef _DEBUG
    createInfo.pNext = &globals.debugMessengerCreateInfo;
#else
    createInfo.pNext = nullptr;
#endif
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledLayerCount = requiredLayers.size();
    createInfo.ppEnabledLayerNames = requiredLayers.data();
    createInfo.enabledExtensionCount = requiredInstanceExtensions.size();
    createInfo.ppEnabledExtensionNames = requiredInstanceExtensions.data();

    THROW_IF_FAILED(
        vkCreateInstance(&createInfo, globals.allocator, &globals.instance),
        __FILE__, __LINE__,
        "Failed to create an instance");
    LOG_DEBUG("Instance successfully created");
}

void SampleBase::createSurface(HINSTANCE hInstance, HWND hWnd)
{
    VkWin32SurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.hinstance = hInstance;
    createInfo.hwnd = hWnd;
    THROW_IF_FAILED(
        vkCreateWin32SurfaceKHR(globals.instance, &createInfo, globals.allocator, &globals.surface),
        __FILE__, __LINE__,
        "Failed to create window surface");
    LOG_DEBUG("Surface successfully created");
}

void SampleBase::createRenderPass()
{
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.flags = 0;
    colorAttachment.format = globals.swapchain.format.format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription depthAttachment = {};
    depthAttachment.flags = 0;
    depthAttachment.format = globals.swapchain.depthStencilBuffer.format;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    
    std::vector<VkAttachmentDescription> attachments;
    attachments.push_back(std::move(colorAttachment));
    attachments.push_back(std::move(depthAttachment));

    VkAttachmentReference colorAttachmentReference;
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentReference;
    depthAttachmentReference.attachment = 1;
    depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass;
    subpass.flags = 0;
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = nullptr;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentReference;
    subpass.pResolveAttachments = nullptr;
    subpass.pDepthStencilAttachment = &depthAttachmentReference;
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = nullptr;

    VkSubpassDependency subpassDependency = {};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpassDependency.srcAccessMask = 0;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    subpassDependency.dependencyFlags = 0;

    VkRenderPassCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.attachmentCount = attachments.size();
    createInfo.pAttachments = attachments.data();
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpass;
    createInfo.dependencyCount = 1;
    createInfo.pDependencies = &subpassDependency;

    THROW_IF_FAILED(
        vkCreateRenderPass(globals.device.handle, &createInfo, globals.allocator, &globals.renderPass),
        __FILE__, __LINE__,
        "Failed to create render pass");
    LOG_DEBUG("Render pass successfully created");
}

void SampleBase::createGraphicsCommandBuffers()
{
    VkCommandPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    createInfo.queueFamilyIndex = globals.device.queues.graphics.index;
    THROW_IF_FAILED(
        vkCreateCommandPool(globals.device.handle, &createInfo, globals.allocator, &globals.graphicsCommandBuffer.pool),
        __FILE__, __LINE__,
        "Failed to create command pool");

    globals.graphicsCommandBuffer.buffers.resize(framesInFlight);
    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.commandPool = globals.graphicsCommandBuffer.pool;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = globals.graphicsCommandBuffer.buffers.size();
    THROW_IF_FAILED(
        vkAllocateCommandBuffers(globals.device.handle, &allocateInfo, globals.graphicsCommandBuffer.buffers.data()),
        __FILE__, __LINE__,
        "Failed to allocate command buffers");
    LOG_DEBUG("Graphics command buffer successfully created");
}

void SampleBase::createSynchronizationObjects()
{
    globals.synchronization.semaphores.imageAcquired.resize(framesInFlight);
    globals.synchronization.semaphores.renderFinished.resize(framesInFlight);
    globals.synchronization.fences.previousFrameFinished.resize(framesInFlight);

    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCreateInfo.pNext = nullptr;
    semaphoreCreateInfo.flags = 0;
    for (u32 i = 0; i < framesInFlight; ++i) {
        THROW_IF_FAILED(
            vkCreateSemaphore(globals.device.handle, &semaphoreCreateInfo, globals.allocator, &globals.synchronization.semaphores.imageAcquired[i]),
            __FILE__, __LINE__,
            "Failed to create semaphore");
        THROW_IF_FAILED(
            vkCreateSemaphore(globals.device.handle, &semaphoreCreateInfo, globals.allocator, &globals.synchronization.semaphores.renderFinished[i]),
            __FILE__, __LINE__,
            "Failed to create semaphore");
    }

    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.pNext = nullptr;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    for (u32 i = 0; i < framesInFlight; ++i) {
        THROW_IF_FAILED(
            vkCreateFence(globals.device.handle, &fenceCreateInfo, globals.allocator, &globals.synchronization.fences.previousFrameFinished[i]),
            __FILE__, __LINE__,
            "Failed to create fence");
    }
    LOG_DEBUG("Synchronization objects successfully created");
}

void SampleBase::drawFrame()
{
    static u32 frameIndex = 0;
    updateFrameResources(frameIndex);

    THROW_IF_FAILED(
        vkWaitForFences(globals.device.handle, 1, &globals.synchronization.fences.previousFrameFinished[frameIndex], VK_TRUE, UINT64_MAX),
        __FILE__, __LINE__,
        "Failed to wait for fences");

    u32 imageIndex;
    auto result = vkAcquireNextImageKHR(
        globals.device.handle, globals.swapchain.handle,
        UINT64_MAX,
        globals.synchronization.semaphores.imageAcquired[frameIndex], VK_NULL_HANDLE,
        &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        return;
    } else {
        THROW_IF_FAILED(result, __FILE__, __LINE__, "Failed to acquire next image");
    }

    THROW_IF_FAILED(
        vkResetFences(globals.device.handle, 1, &globals.synchronization.fences.previousFrameFinished[frameIndex]),
        __FILE__, __LINE__,
        "Failed to reset fences");

    vkResetCommandBuffer(globals.graphicsCommandBuffer.buffers[frameIndex], 0);
    recordCommandBuffer(globals.graphicsCommandBuffer.buffers[frameIndex], imageIndex, frameIndex);

    VkPipelineStageFlags waitPipelineStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &globals.synchronization.semaphores.imageAcquired[frameIndex];
    submitInfo.pWaitDstStageMask = &waitPipelineStage;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &globals.graphicsCommandBuffer.buffers[frameIndex];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &globals.synchronization.semaphores.renderFinished[frameIndex];

    THROW_IF_FAILED(
        vkQueueSubmit(globals.device.queues.graphics.handle, 1, &submitInfo, globals.synchronization.fences.previousFrameFinished[frameIndex]),
        __FILE__, __LINE__,
        "Failed to queue submit");

    VkPresentInfoKHR presentInfo;
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &globals.synchronization.semaphores.renderFinished[frameIndex];
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &globals.swapchain.handle;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

    frameIndex = (frameIndex + 1) % framesInFlight;

    result = vkQueuePresentKHR(globals.device.queues.present.handle, &presentInfo);
}

void SampleBase::destroySynchronizationObjects()
{
    for (u32 i = 0; i < framesInFlight; ++i) {
        vkDestroySemaphore(globals.device.handle, globals.synchronization.semaphores.imageAcquired[i], globals.allocator);
        vkDestroySemaphore(globals.device.handle, globals.synchronization.semaphores.renderFinished[i], globals.allocator);
        vkDestroyFence(globals.device.handle, globals.synchronization.fences.previousFrameFinished[i], globals.allocator);
    }

    LOG_DEBUG("Synchronization objects destroyed");
}

void SampleBase::destroyGraphicsCommandBuffers()
{
    vkFreeCommandBuffers(
        globals.device.handle,
        globals.graphicsCommandBuffer.pool,
        globals.graphicsCommandBuffer.buffers.size(),
        globals.graphicsCommandBuffer.buffers.data());

    vkDestroyCommandPool(globals.device.handle, globals.graphicsCommandBuffer.pool, globals.allocator);
    LOG_DEBUG("Graphics command buffer destroyed");
}

void SampleBase::destroyRenderPass()
{
    vkDestroyRenderPass(globals.device.handle, globals.renderPass, globals.allocator);
    LOG_DEBUG("Render pass destroyed");
}

void SampleBase::destroySurface()
{
    vkDestroySurfaceKHR(globals.instance, globals.surface, globals.allocator);
    LOG_DEBUG("Surface destroyed");
}

void SampleBase::destroyInstance()
{
    vkDestroyInstance(globals.instance, globals.allocator);
    LOG_DEBUG("Instance destroyed");
}

void initRequiredLayers(std::vector<char const*>& requiredLayers)
{
#ifdef _DEBUG
    requiredLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif
}

void checkRequiredLayersSupport(std::vector<char const*> const& requiredLayers)
{
    char buffer[16384];
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
    THROW_IF_FAILED(
        vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr),
        __FILE__, __LINE__,
        "Failed to enumerate instance layer properties");
    std::vector<VkLayerProperties> availableLayers(availableLayerCount);
    THROW_IF_FAILED(
        vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data()),
        __FILE__, __LINE__,
        "Failed to enumerate instance layer properties");

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

    LOG_INFO("All required layers supported");
}

void initRequiredInstanceExtensions(std::vector<char const*>& requiredInstanceExtensions)
{
    requiredInstanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#ifdef _WIN32
    requiredInstanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif
#ifdef _DEBUG
    requiredInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
}

void checkRequiredInstanceExtensionsSupport(std::vector<char const*> const& requiredExtensions)
{
    char buffer[16384];
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
    THROW_IF_FAILED(
        vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr),
        __FILE__, __LINE__,
        "Failed to enumerate instance extension properties");
    std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
    THROW_IF_FAILED(
        vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, availableExtensions.data()),
        __FILE__, __LINE__,
        "Failed to enumerate instance extension properties");

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

    LOG_INFO("All required instance extensions supported");
}
