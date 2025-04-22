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
    // void onRender();
    void onDestroy();
    void onNotify(EventType type, EventContext context) override;

protected:
    Globals globals;
    Buffer vertexBuffer;
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
    virtual void createDescriptorSetLayouts() = 0;
    virtual void createPipelineLayouts() = 0;
    virtual void createPipelines() = 0;
    virtual void createMesh() = 0;
    virtual void createIndexBuffer() = 0;
    virtual void createUniformBuffers() = 0;
    virtual void createTexture() = 0;
    virtual void createDescriptorSets() = 0;
    virtual void createRenderLayers() = 0;
    virtual void createRenderLayers() = 0;
    virtual void createRenderObjects() = 0;
    virtual void createFrameResources() = 0;

    void drawFrame();
    virtual void updateFrameResources(u32 frameIndex) = 0;
    virtual void recordCommandBuffer(VkCommandBuffer commandBuffer, u32 imageIndex, u32 frameIndex) = 0;

    virtual void destroyFrameResources() = 0;
    virtual void destroyRenderObjects() = 0;
    virtual void destroyDescriptorSets() = 0;
    virtual void destroyTextureImage() = 0;
    virtual void destroyUniformBuffers() = 0;
    virtual void destroyIndexBuffer() = 0;
    virtual void destroyVertexBuffer() = 0;
    virtual void destroyGraphicsPipeline() = 0;
    virtual void destroyPipelineLayout() = 0;
    virtual void destroyDescriptorSetLayouts() = 0;
    void destroySynchronizationObjects();
    void destroyGraphicsCommandBuffers();
    void destroyRenderPass();
    void destroySurface();
    void destroyInstance();
};
