#include "Boilerplate/Application.h"
#include "Boilerplate/Entry.h"
#include "Boilerplate/EventManager.h"
#include "Boilerplate/Initializer.h"
#include "Boilerplate/ProceduralMeshes/Box.h"
#include "Boilerplate/ProceduralMeshes/Sphere.h"
#include "Boilerplate/SampleBase.h"
#include "Boilerplate/Structures.h"
#include "Boilerplate/Utils.h"
#include "Boilerplate/GltfModel.h"

#include <glm/gtc/matrix_transform.hpp>

class GltfTest : public SampleBase {
public:
    GltfTest(u32 width, u32 height, std::string const& name);

private:
    std::vector<Mesh> meshes;
    std::vector<Texture> textures;
    std::vector<Material> materials;
    std::vector<RenderObject> renderObjects;
    std::vector<FrameResource> frameResources;
    std::vector<DescriptorSets> resourceDescriptors;
    std::vector<VkPushConstantRange> pushConstantRanges;
    std::vector<VkPipelineLayout> pipelineLayouts;
    std::vector<VkPipeline> pipelines;

    struct {
        DirLight direct;
        SpotLight spot;
        std::vector<PointLight> points;
    } lights;
    std::vector<glm::vec3> pointLightPositions;

    Camera camera;
    glm::vec3 lightPos = glm::vec3(1.2f, 1.f, 2.f);
    glm::vec3 lightColor = glm::vec3(1.f);
    glm::vec3 diffuseColor;
    glm::vec3 ambientColor;

    GltfModel gltfModel;

    void createMeshes() override;
    void createTextures() override;
    void createMaterials() override;
    void createRenderObjects() override;
    void createLights() override;
    void createFrameResources() override;
    void createResourceDescriptors() override;
    void createPushConstantRanges() override;
    void createPipelines() override;

    void updateFrameResources(u32 frameIndex) override;
    void recordCommandBuffer(VkCommandBuffer commandBuffer, u32 imageIndex, u32 frameIndex, ImDrawData* draw_data) override;

    void destroyPipelines() override;
    void destroyPushConstantRanges() override;
    void destroyResourceDescriptors() override;
    void destroyFrameResources() override;
    void destroyTextures() override;
    void destroyMeshes() override;
};

GltfTest::GltfTest(u32 width, u32 height, std::string const& name) :
    SampleBase(width, height, name),
    camera(globals, glm::vec3(0.f, 0.f, 5.f))
{
    EventManager::subscribe(EventType::LEFT_BUTTON_DOWN, &camera);
    EventManager::subscribe(EventType::MOUSE_MOVE, &camera);
    EventManager::subscribe(EventType::KEY_DOWN, &camera);
    EventManager::subscribe(EventType::WINDOW_RESIZE, this);

    diffuseColor = lightColor * glm::vec3(0.5f);
    ambientColor = diffuseColor * glm::vec3(0.2f);

    pointLightPositions.resize(4);
    pointLightPositions[0] = glm::vec3(0.7f, 0.2f, 2.f);
    pointLightPositions[1] = glm::vec3(2.3f, -3.3f, -4.f);
    pointLightPositions[2] = glm::vec3(-4.f,  2.f, -12.f);
    pointLightPositions[3] = glm::vec3(0.f,  0.f, -3.f);
}

void GltfTest::createMeshes()
{
    gltfModel.load("Assets/Cube/glTF/Cube.gltf");
    gltfModel.loadNodes();
    gltfModel.loadMeshes(globals);
}

void GltfTest::createTextures()
{
    gltfModel.loadSamplers(globals);
    gltfModel.loadImages(globals);
}

void GltfTest::createMaterials()
{
    gltfModel.loadMaterials();
}

void GltfTest::createRenderObjects()
{
}

void GltfTest::createLights()
{
}

void GltfTest::createFrameResources()
{
    frameResources.resize(framesInFlight);
    for (u32 i = 0; i < frameResources.size(); ++i) {
        {
            frameResources[i].passBuffer.size = sizeof(Pass);
            frameResources[i].passBuffer.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            frameResources[i].passBuffer.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            createBuffer(globals, frameResources[i].passBuffer);
            THROW_IF_FAILED(
                vkMapMemory(globals.device.handle, frameResources[i].passBuffer.memory, 0, frameResources[i].passBuffer.size, 0, &frameResources[i].passBuffer.mapped),
                __FILE__, __LINE__,
                "Failed to map memory");
        }
    }

    gltfModel.createFrameResources(globals);
}

void GltfTest::createResourceDescriptors()
{
    resourceDescriptors.resize(1);
    {
        std::vector<VkDescriptorPoolSize> poolSizes(1);
        poolSizes[0] = Initializer::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, framesInFlight);
        auto descriptorPoolCreateInfo = Initializer::descriptorPoolCreateInfo(framesInFlight, poolSizes);
        THROW_IF_FAILED(
            vkCreateDescriptorPool(globals.device.handle, &descriptorPoolCreateInfo, globals.allocator, &resourceDescriptors[0].pool),
            __FILE__, __LINE__,
            "Failed to create descriptor pool");

        std::vector<VkDescriptorSetLayoutBinding> bindings(1);
        bindings[0] = Initializer::descriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
        auto descriptorSetLayoutCreateInfo = Initializer::descriptorSetLayoutCreateInfo(bindings);
        THROW_IF_FAILED(
            vkCreateDescriptorSetLayout(globals.device.handle, &descriptorSetLayoutCreateInfo, globals.allocator, &resourceDescriptors[0].setLayout),
            __FILE__, __LINE__,
            "Failed to create descriptor set layout");

        std::vector<VkDescriptorSetLayout> setLayouts(framesInFlight, resourceDescriptors[0].setLayout);
        auto descriptorSetAllocateInfo = Initializer::descriptorSetAllocateInfo(resourceDescriptors[0].pool, framesInFlight, setLayouts);
        resourceDescriptors[0].handles.resize(framesInFlight);
        THROW_IF_FAILED(
            vkAllocateDescriptorSets(globals.device.handle, &descriptorSetAllocateInfo, resourceDescriptors[0].handles.data()),
            __FILE__, __LINE__,
            "Failed to allocate descriptor sets");

        for (u32 i = 0; i < framesInFlight; ++i) {
            std::vector<VkDescriptorBufferInfo> uniformBufferDescriptors(1);
            uniformBufferDescriptors[0] = Initializer::descriptorBufferInfo(frameResources[i].passBuffer.handle, 0);
            std::vector<VkWriteDescriptorSet> descriptorWrites(1);
            descriptorWrites[0] = Initializer::writeDescriptorSet(
                resourceDescriptors[0].handles[i],
                0, 0, uniformBufferDescriptors.size(),
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                nullptr, uniformBufferDescriptors.data(), nullptr);
            vkUpdateDescriptorSets(globals.device.handle, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
        }
    }
    
    gltfModel.createDescriptors(globals);
}

void GltfTest::createPushConstantRanges()
{
    pushConstantRanges.resize(1);
    pushConstantRanges[0].offset = 0;
    pushConstantRanges[0].size = sizeof(u32);
    pushConstantRanges[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
}

void GltfTest::createPipelines()
{
    pipelines.resize(1);
    pipelineLayouts.resize(1);
    {
        std::vector<VkPipelineShaderStageCreateInfo> stages(2);
        VkShaderModule shaderModule[2];
        {
            auto code = loadShaderCode("GltfTest/CubeVertex.spv");
            auto shaderModuleCreateInfo = Initializer::shaderModuleCreateInfo(code);
            THROW_IF_FAILED(
                vkCreateShaderModule(globals.device.handle, &shaderModuleCreateInfo, globals.allocator, &shaderModule[0]),
                __FILE__, __LINE__,
                "Failed to create shader module");
            stages[0] = Initializer::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, shaderModule[0]);
        }
        {
            auto code = loadShaderCode("GltfTest/CubeFragment.spv");
            auto shaderModuleCreateInfo = Initializer::shaderModuleCreateInfo(code);
            THROW_IF_FAILED(
                vkCreateShaderModule(globals.device.handle, &shaderModuleCreateInfo, globals.allocator, &shaderModule[1]),
                __FILE__, __LINE__,
                "Failed to create shader module");
            stages[1] = Initializer::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, shaderModule[1]);
        }

        std::vector<VkVertexInputBindingDescription> vertexBindingDescriptions(3);
        vertexBindingDescriptions[0] = Initializer::vertexInputBindingDescription(0, sizeof(glm::vec3));
        vertexBindingDescriptions[1] = Initializer::vertexInputBindingDescription(1, sizeof(glm::vec3));
        vertexBindingDescriptions[2] = Initializer::vertexInputBindingDescription(2, sizeof(glm::vec2));
        std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions(3);
        vertexAttributeDescriptions[0] = Initializer::vertexInputAttributeDescription(0, vertexBindingDescriptions[0].binding, VK_FORMAT_R32G32B32_SFLOAT, 0);
        vertexAttributeDescriptions[1] = Initializer::vertexInputAttributeDescription(1, vertexBindingDescriptions[1].binding, VK_FORMAT_R32G32B32_SFLOAT, 0);
        vertexAttributeDescriptions[2] = Initializer::vertexInputAttributeDescription(2, vertexBindingDescriptions[2].binding, VK_FORMAT_R32G32_SFLOAT, 0);
        auto vertexInputState = Initializer::pipelineVertexInputStateCreateInfo(vertexBindingDescriptions, vertexAttributeDescriptions);

        auto inputAssemblyState = Initializer::pipelineInputAssemblyStateCreateInfo();
        auto tessellationState = Initializer::pipelineTessellationStateCreateInfo();

        std::vector<VkViewport> viewports(1);
        viewports[0] = Initializer::viewport(globals.swapchain.extent.width, globals.swapchain.extent.height);
        std::vector<VkRect2D> scissors(1);
        scissors[0] = Initializer::scissor(globals.swapchain.extent.width, globals.swapchain.extent.height);
        auto viewportState = Initializer::pipelineViewportStateCreateInfo(viewports, scissors);

        auto rasterizationState = Initializer::pipelineRasterizationStateCreateInfo();
        auto multisampleState = Initializer::pipelineMultisampleStateCreateInfo();
        auto depthStencilState = Initializer::pipelineDepthStencilStateCreateInfo();

        std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentStates(1);
        colorBlendAttachmentStates[0] = Initializer::pipelineColorBlendAttachmentState();
        auto colorBlendState = Initializer::pipelineColorBlendStateCreateInfo(colorBlendAttachmentStates);

        std::vector<VkDynamicState> dynamicStates(2);
        dynamicStates[0] = VK_DYNAMIC_STATE_VIEWPORT;
        dynamicStates[1] = VK_DYNAMIC_STATE_SCISSOR;
        auto dynamicState = Initializer::pipelineDynamicStateCreateInfo(dynamicStates);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts(3);
        descriptorSetLayouts[0] = resourceDescriptors[0].setLayout;
        descriptorSetLayouts[1] = gltfModel.resourceDescriptors[0].setLayout;
        descriptorSetLayouts[2] = gltfModel.resourceDescriptors[1].setLayout;
        auto pipelineLayoutCreateInfo = Initializer::pipelineLayoutCreateInfo(descriptorSetLayouts, pushConstantRanges);
        THROW_IF_FAILED(
            vkCreatePipelineLayout(globals.device.handle, &pipelineLayoutCreateInfo, globals.allocator, &pipelineLayouts[0]),
            __FILE__, __LINE__,
            "Failed to create pipeline layout");

        auto createInfo = Initializer::graphicsPipelineCreateInfo(
            stages,
            &vertexInputState,
            &inputAssemblyState,
            &tessellationState,
            &viewportState,
            &rasterizationState,
            &multisampleState,
            &depthStencilState,
            &colorBlendState,
            &dynamicState,
            pipelineLayouts[0],
            globals.renderPass);
        THROW_IF_FAILED(
            vkCreateGraphicsPipelines(globals.device.handle, VK_NULL_HANDLE, 1, &createInfo, globals.allocator, &pipelines[0]),
            __FILE__, __LINE__,
            "Failed to create graphics pipeline");

        vkDestroyShaderModule(globals.device.handle, shaderModule[0], globals.allocator);
        vkDestroyShaderModule(globals.device.handle, shaderModule[1], globals.allocator);
    }
}

void GltfTest::recordCommandBuffer(VkCommandBuffer commandBuffer, u32 imageIndex, u32 frameIndex, ImDrawData* draw_data)
{
    auto commandBufferBeginInfo = Initializer::commandBufferBeginInfo();
    THROW_IF_FAILED(
        vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo),
        __FILE__, __LINE__,
        "Failed to begin command buffer");

    VkClearValue clearColor = {};
    clearColor.color = {{ 0.f, 0.f, 0.f, 1.f }};
    VkClearValue clearDepth = {};
    clearDepth.depthStencil = { 1.f, 0 };
    std::vector<VkClearValue> clearValues(2);
    clearValues[0] = clearColor;
    clearValues[1] = clearDepth;

    VkRect2D renderArea = { { 0, 0 }, globals.swapchain.extent };
    auto renderPassBeginInfo = Initializer::renderPassBeginInfo(
        globals.renderPass,
        globals.swapchain.framebuffers[imageIndex],
        renderArea,
        clearValues);
    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    auto viewport = Initializer::viewport(globals.swapchain.extent.width, globals.swapchain.extent.height);
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    auto scissor = Initializer::scissor(globals.swapchain.extent.width, globals.swapchain.extent.height);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    std::vector<VkBuffer> vertexBuffers(3);
    vertexBuffers[0] = gltfModel.vertexBuffers.positions.handle;
    vertexBuffers[1] = gltfModel.vertexBuffers.normals.handle;
    vertexBuffers[2] = gltfModel.vertexBuffers.texCoords.handle;
    std::vector<VkDeviceSize> offsets(3, 0);
    //offsets[0] = 0;
    //offsets[1] = gltfModel.positions.size() * sizeof(gltfModel.positions[0]);
    //offsets[2] = gltfModel.positions.size() * sizeof(gltfModel.positions[0]) + gltfModel.normals.size() * sizeof(gltfModel.normals[0]);
    vkCmdBindVertexBuffers(commandBuffer, 0, vertexBuffers.size(), vertexBuffers.data(), offsets.data());
    vkCmdBindIndexBuffer(commandBuffer, gltfModel.indexBuffer.handle, 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayouts[0],
        0, 1, &resourceDescriptors[0].handles[frameIndex],
        0, nullptr);

    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayouts[0],
        1, 1, &gltfModel.resourceDescriptors[0].handles[frameIndex],
        0, nullptr);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines[0]);

    for (u32 i = 0; i < gltfModel.model.nodes.size(); ++i) {
        u32 dynamicOffset = i * gltfModel.frameResources[frameIndex].renderObjectBuffer.alignment;

        vkCmdBindDescriptorSets(
            commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayouts[0],
            2, 1, &gltfModel.resourceDescriptors[1].handles[frameIndex],
            1, &dynamicOffset);

        auto& mesh = gltfModel.model.meshes[gltfModel.model.nodes[i].mesh];
        for (u32 j = 0; j < mesh.primitives.size(); ++j) {
            u32 materialIndex = gltfModel.meshes[gltfModel.model.nodes[i].mesh].primitives[j].materialIndex;
            vkCmdPushConstants(
                commandBuffer,
                pipelineLayouts[0],
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0, sizeof(materialIndex), &materialIndex);

            vkCmdDrawIndexed(
            commandBuffer,
            gltfModel.meshes[gltfModel.model.nodes[i].mesh].primitives[j].indexCount, 1,
            gltfModel.meshes[gltfModel.model.nodes[i].mesh].primitives[j].firstIndex,
            gltfModel.meshes[gltfModel.model.nodes[i].mesh].primitives[j].vertexOffset, 0);
        }
    }

    vkCmdEndRenderPass(commandBuffer);
    THROW_IF_FAILED(
        vkEndCommandBuffer(commandBuffer),
        __FILE__, __LINE__,
        "Failed to end command buffer");
}

void GltfTest::updateFrameResources(u32 frameIndex)
{
    {
        Pass pass = {};
        pass.view = camera.matrices.view;
        pass.proj = camera.matrices.proj;
        pass.viewPos = camera.pos;
        memcpy(frameResources[frameIndex].passBuffer.mapped, &pass, sizeof(pass));
    }
    {
        // materials
    }
    {
        // render objects
    }
}

void GltfTest::destroyPipelines()
{
    for (u32 i = 0; i < pipelineLayouts.size(); ++i) {
        vkDestroyPipelineLayout(globals.device.handle, pipelineLayouts[i], globals.allocator);
    }

    for (u32 i = 0; i < pipelines.size(); ++i) {
        vkDestroyPipeline(globals.device.handle, pipelines[i], globals.allocator);
    }
}

void GltfTest::destroyPushConstantRanges()
{
}

void GltfTest::destroyResourceDescriptors()
{
    for (u32 i = 0; i < resourceDescriptors.size(); ++i) {
        vkDestroyDescriptorSetLayout(globals.device.handle, resourceDescriptors[i].setLayout, globals.allocator);
    }

    for (u32 i = 0; i < resourceDescriptors.size(); ++i) {
        vkDestroyDescriptorPool(globals.device.handle, resourceDescriptors[i].pool, globals.allocator);
    }
}

void GltfTest::destroyFrameResources()
{
    for (u32 i = 0; i < frameResources.size(); ++i) {
        vkUnmapMemory(globals.device.handle, frameResources[i].passBuffer.memory);
        destroyBuffer(globals, frameResources[i].passBuffer);
    }
}

void GltfTest::destroyTextures()
{
    for (u32 i = 0; i < textures.size(); ++i) {
        vkDestroySampler(globals.device.handle, textures[i].sampler.handle, globals.allocator);
        vkDestroyImageView(globals.device.handle, textures[i].image.view, globals.allocator);
        destroyImage(globals, textures[i].image);
    }
}

void GltfTest::destroyMeshes()
{
    for (u32 i = 0; i < meshes.size(); ++i) {
        destroyBuffer(globals, meshes[i].indexBuffer);
        destroyBuffer(globals, meshes[i].vertexBuffer);
    }
}

std::unique_ptr<SampleBase> createSample()
{
    return std::make_unique<GltfTest>(1600, 900, "GltfTest");
}
