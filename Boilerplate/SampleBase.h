#pragma once

#include "Defines.h"

#include "Camera.h"

#include "DebugMessenger.h"
#include "Device.h"
#include "Swapchain.h"

#include <windows.h>
#include <string>

class GLFWwindow;

class SampleBase {
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
    virtual void createDescriptorSetLayout() = 0;
    virtual void createPipelineLayout() = 0;
    virtual void createGraphicsPipeline() = 0;
    virtual void createVertexBuffer() = 0;
    virtual void createIndexBuffer() = 0;
    virtual void createUniformBuffers() = 0;
    virtual void createTextureImage() = 0;
    virtual void createDescriptorSets() = 0;

    void drawFrame();
    virtual void recordCommandBuffer(VkCommandBuffer commandBuffer, u32 imageIndex, u32 frameIndex) = 0;
    virtual void updateUniformBuffer(u32 frameIndex) = 0;

    virtual void destroyDescriptorSets() = 0;
    virtual void destroyTextureImage() = 0;
    virtual void destroyUniformBuffers() = 0;
    virtual void destroyIndexBuffer() = 0;
    virtual void destroyVertexBuffer() = 0;
    virtual void destroyGraphicsPipeline() = 0;
    virtual void destroyPipelineLayout() = 0;
    virtual void destroyDescriptorSetLayout() = 0;
    void destroySynchronizationObjects();
    void destroyGraphicsCommandBuffers();
    void destroyRenderPass();
    void destroySurface();
    void destroyInstance();
};
