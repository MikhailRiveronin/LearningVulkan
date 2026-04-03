#pragma once

#include "Defines.h"
#include "CommandBuffer.h"
#include "vulkan_classes.h"

#include <vulkan/vulkan.h>
#include <vector>

struct ContextConfig {
    void* allocator;

    void* window;
    u32 width;
    u32 height;

    u32 threadCount;
};

class Context {
public:
    u32 framesInFlight;
    u32 taskThreadCount;
    VkAllocationCallbacks allocationCallbacks;

    struct {
        VkDevice handle = VK_NULL_HANDLE;

        struct {
            VkPhysicalDeviceProperties properties;
            VkPhysicalDeviceFeatures features;
            VkPhysicalDeviceMemoryProperties memoryProperties;
        } support;

        struct {
            struct {
                VkQueue handle = VK_NULL_HANDLE;
                u32 familyIndex;
            } graphics;
            
            struct {
                VkQueue handle = VK_NULL_HANDLE;
                u32 familyIndex;
            } compute;
            
            struct {
                VkQueue handle = VK_NULL_HANDLE;
                u32 familyIndex;
            } transfer;
            
            struct {
                VkQueue handle = VK_NULL_HANDLE;
                u32 familyIndex;
            } present;
        } queues;
    } 
    device;

    std::vector<Pipeline> pipelines;

    Context(ContextConfig const& config = ContextConfig());


private:
    CommandBufferManager commandBufferManager;

    void createPipelines();


};
