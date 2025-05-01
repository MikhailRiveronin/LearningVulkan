#pragma once

#include "Defines.h"
#include "Camera.h"
#include "DebugMessenger.h"
#include "Device.h"
#include "EventManager.h"
#include "Swapchain.h"

#include <windows.h>
#include <string>

class GLFWwindow;

class SampleBase : public Listener {
public:
    u32 width;
    u32 height;
    std::string name;

    bool framebufferResized;
    SampleBase(u32 width, u32 height, std::string const& name);

    void onInit(HINSTANCE hInstance, HWND hWnd);
    void onUpdate();
    void onDestroy();
    void onNotify(EventType type, EventContext context) override;

protected:
    Globals globals;
    Image depthBuffer;

private:
    DebugMessenger debugMessenger;
    Device device;
    Swapchain swapchain;

    void createInstance();
    void createSurface(HINSTANCE hInstance, HWND hWnd);
    void createRenderPass();
    void createGraphicsCommandBuffers();
    void createSynchronizationObjects();
    virtual void createMeshes() = 0;
    virtual void createTextures() = 0;
    virtual void createMaterials() = 0;
    virtual void createRenderObjects() = 0;
    virtual void createFrameResources() = 0;
    virtual void createResourceDescriptors() = 0;
    virtual void createPushConstantRanges() = 0;
    virtual void createPipelines() = 0;

    void drawFrame();
    virtual void updateFrameResources(u32 frameIndex) = 0;
    virtual void recordCommandBuffer(VkCommandBuffer commandBuffer, u32 imageIndex, u32 frameIndex) = 0;

    virtual void destroyPipelines() = 0;
    virtual void destroyPushConstantRanges() = 0;
    virtual void destroyResourceDescriptors() = 0;
    virtual void destroyFrameResources() = 0;
    virtual void destroyTextures() = 0;
    virtual void destroyMeshes() = 0;
    void destroySynchronizationObjects();
    void destroyGraphicsCommandBuffers();
    void destroyRenderPass();
    void destroySurface();
    void destroyInstance();
};
