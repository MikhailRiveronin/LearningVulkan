#pragma once

#include "DebugMessenger.h"
#include "Defines.h"
#include "Device.h"
#include "Swapchain.h"

#include <string>

class GLFWwindow;

class SampleBase {
public:
    bool framebufferResized;
    SampleBase(u32 width, u32 height, std::string const& name);

    void run();

protected:
    Context context;
    Buffer vertexBuffer;
    Image depthBuffer;

private:
    u32 width;
    u32 height;
    std::string name;
    GLFWwindow* window;
#ifdef _DEBUG
    DebugMessenger messenger;
#endif
    Device device;
    Swapchain swapchain;

    void startup(u32 width, u32 height, std::string const& name);
    void update();
    void shutdown();

    void createWindow(u32 width, u32 height, std::string const& name);
    void createInstance();
    void createSurface();
    void createRenderPass();
    virtual void createDescriptorSetLayout() = 0;
    virtual void createGraphicsPipeline() = 0;
    virtual void createUniformBuffers() = 0;
    virtual void createDescriptorPool() = 0;
    virtual void allocateDescriptorSets() = 0;
    void createFramebuffers();
    void createCommandPool();
    void allocateCommandBuffer();
    virtual void recordCommandBuffer(VkCommandBuffer commandBuffer, u32 imageIndex, u32 frameIndex) = 0;
    void createSynchronizationObjects();
    void drawFrame();

    virtual void createGeometry() = 0;
    virtual void createVertexBuffer() = 0;
    virtual void createIndexBuffer() = 0;
    virtual void sampleSpecificShutdown() = 0;

    virtual void updateUniformBuffer(u32 frameIndex) = 0;

    virtual void createTextureImage() = 0;
    void createDepthBuffer();

    void destroySynchronizationObjects();
};
