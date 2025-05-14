#include "GltfModel.h"
#include "Initializer.h"
#include "Logger.h"
#include "Utils.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

void GltfModel::load(std::string filename)
{
    tinygltf::TinyGLTF loader;
    std::string errors;
    std::string warnings;
    if (!loader.LoadASCIIFromFile(&model, &errors, &warnings, filename)) {
        LOG_ERROR("%s (%u): %s", __FILE__, __LINE__, errors.c_str());
        throw std::runtime_error(errors);
    }

    scenes.resize(model.scenes.size());
    for (u32 i = 0; i < scenes.size(); ++i) {

    }
}

void GltfModel::loadNodes()
{
    nodes.resize(model.nodes.size());
    for (u32 i = 0; i < model.nodes.size(); ++i) {
        if (!model.nodes[i].matrix.empty()) {
            std::vector<float> matrix(model.nodes[i].matrix.size());
            std::transform(model.nodes[i].matrix.begin(), model.nodes[i].matrix.end(), matrix.begin(), [](auto d) {
                return static_cast<float>(d);
            });
            nodes[i].local = glm::make_mat4(matrix.data());
        } else {
            std::vector<float> scale(model.nodes[i].scale.size());
            std::transform(model.nodes[i].scale.begin(), model.nodes[i].scale.end(), scale.begin(), [](auto d) {
                return static_cast<float>(d);
            });

            std::vector<float> rotation(model.nodes[i].rotation.size());
            std::transform(model.nodes[i].rotation.begin(), model.nodes[i].rotation.end(), rotation.begin(), [](auto d) {
                return static_cast<float>(d);
            });

            std::vector<float> translation(model.nodes[i].translation.size());
            std::transform(model.nodes[i].translation.begin(), model.nodes[i].translation.end(), translation.begin(), [](auto d) {
                return static_cast<float>(d);
            });

            nodes[i].local =
                glm::translate(glm::mat4(1.0), glm::make_vec3(translation.data())) *
                glm::mat4_cast(glm::make_quat(rotation.data())) *
                glm::scale(glm::mat4(1.0), glm::make_vec3(scale.data()));
        }
    }
}

void GltfModel::loadMeshes()
{
    meshes.resize(model.meshes.size());
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec4> tangents;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec4> colors;
    std::vector<glm::u16vec4> joints;
    std::vector<glm::vec4> weights;
    std::vector<u32> indices;
    for (u32 i = 0; i < model.meshes.size(); ++i) {
        meshes[i].primitives.resize(model.meshes[i].primitives.size());
        for (u32 j = 0; j < model.meshes[i].primitives.size(); ++j) {
            meshes[i].primitives[j].firstIndex = indices.size();
            meshes[i].primitives[j].vertexOffset = positions.size();

            if (model.meshes[i].primitives[j].attributes.count("POSITION") != 0) {
                auto& accessor = model.accessors[model.meshes[i].primitives[j].attributes["POSITION"]];
                auto& bufferView = model.bufferViews[accessor.bufferView];
                auto& buffer = model.buffers[bufferView.buffer];
                std::vector<glm::vec3> data(accessor.count);
                memcpy(data.data(), buffer.data.data() + accessor.byteOffset + bufferView.byteOffset, bufferView.byteLength);
                positions.insert(positions.end(), data.begin(), data.end());
            };

            if (model.meshes[i].primitives[j].attributes.count("NORMAL") != 0) {
                auto& accessor = model.accessors[model.meshes[i].primitives[j].attributes["NORMAL"]];
                auto& bufferView = model.bufferViews[accessor.bufferView];
                auto& buffer = model.buffers[bufferView.buffer];
                std::vector<glm::vec3> data(accessor.count);
                memcpy(data.data(), buffer.data.data() + accessor.byteOffset + bufferView.byteOffset, bufferView.byteLength);
                normals.insert(normals.end(), data.begin(), data.end());
            }

            if (model.meshes[i].primitives[j].attributes.count("TANGENT") != 0) {
                auto& accessor = model.accessors[model.meshes[i].primitives[j].attributes["TANGENT"]];
                auto& bufferView = model.bufferViews[accessor.bufferView];
                auto& buffer = model.buffers[bufferView.buffer];
                std::vector<glm::vec4> data(accessor.count);
                memcpy(data.data(), buffer.data.data() + accessor.byteOffset + bufferView.byteOffset, bufferView.byteLength);
                tangents.insert(tangents.end(), data.begin(), data.end());
            }

            if (model.meshes[i].primitives[j].attributes.count("TEXCOORD_0") != 0) {
                auto& accessor = model.accessors[model.meshes[i].primitives[j].attributes["TEXCOORD_0"]];
                auto& bufferView = model.bufferViews[accessor.bufferView];
                auto& buffer = model.buffers[bufferView.buffer];

                switch (accessor.componentType) {
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
                    std::vector<glm::u8vec2> data(accessor.count);
                    memcpy(data.data(), buffer.data.data() + accessor.byteOffset + bufferView.byteOffset, bufferView.byteLength);
                    texCoords.reserve(texCoords.size() + accessor.count);
                    for (u32 i = 0; i < accessor.count; ++i) {
                        texCoords.push_back(glm::vec2(static_cast<float>(data[i][0]) / 255.f, static_cast<float>(data[i][1]) / 255.f));
                    }
                    break;
                }

                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
                    std::vector<glm::u16vec2> data(accessor.count);
                    memcpy(data.data(), buffer.data.data() + accessor.byteOffset + bufferView.byteOffset, bufferView.byteLength);
                    texCoords.reserve(texCoords.size() + accessor.count);
                    for (u32 i = 0; i < accessor.count; ++i) {
                        texCoords.push_back(glm::vec2(static_cast<float>(data[i][0]) / 65535.f, static_cast<float>(data[i][1]) / 65535.f));
                    }
                    break;
                }

                case TINYGLTF_COMPONENT_TYPE_FLOAT: {
                    std::vector<glm::vec2> data(accessor.count);
                    memcpy(data.data(), buffer.data.data() + accessor.byteOffset + bufferView.byteOffset, bufferView.byteLength);
                    texCoords.insert(texCoords.end(), data.begin(), data.end());
                    break;
                }

                default:
                    break;
                }
            }

            if (model.meshes[i].primitives[j].attributes.count("COLOR_0") != 0) {
                auto& accessor = model.accessors[model.meshes[i].primitives[j].attributes["COLOR_0"]];
                auto& bufferView = model.bufferViews[accessor.bufferView];
                auto& buffer = model.buffers[bufferView.buffer];

                switch (accessor.type) {
                case TINYGLTF_TYPE_VEC3: {
                    switch (accessor.componentType) {
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
                        std::vector<glm::u8vec3> data(accessor.count);
                        memcpy(data.data(), buffer.data.data() + accessor.byteOffset + bufferView.byteOffset, bufferView.byteLength);
                        colors.reserve(colors.size() + accessor.count);
                        for (u32 i = 0; i < accessor.count; ++i) {
                            colors.push_back(glm::vec4(static_cast<float>(data[i][0]) / 255.f, static_cast<float>(data[i][1]) / 255.f, static_cast<float>(data[i][2]) / 255.f, 1.f));
                        }
                        break;
                    }

                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
                        std::vector<glm::u16vec3> data(accessor.count);
                        memcpy(data.data(), buffer.data.data() + accessor.byteOffset + bufferView.byteOffset, bufferView.byteLength);
                        colors.reserve(colors.size() + accessor.count);
                        for (u32 i = 0; i < accessor.count; ++i) {
                            colors.push_back(glm::vec4(static_cast<float>(data[i][0]) / 65535.f, static_cast<float>(data[i][1]) / 65535.f, static_cast<float>(data[i][2]) / 65535.f, 1.f));
                        }
                        break;
                    }

                    case TINYGLTF_COMPONENT_TYPE_FLOAT: {
                        std::vector<glm::vec3> data(accessor.count);
                        memcpy(data.data(), buffer.data.data() + accessor.byteOffset + bufferView.byteOffset, bufferView.byteLength);
                        colors.reserve(colors.size() + accessor.count);
                        for (u32 i = 0; i < accessor.count; ++i) {
                            colors.push_back(glm::vec4(data[i], 1.f));
                        }
                        break;
                    }

                    default:
                        break;
                    }
                    break;
                }

                case TINYGLTF_TYPE_VEC4: {
                    switch (accessor.componentType) {
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
                        std::vector<glm::u8vec4> data(accessor.count);
                        memcpy(data.data(), buffer.data.data() + accessor.byteOffset + bufferView.byteOffset, bufferView.byteLength);
                        colors.reserve(colors.size() + accessor.count);
                        for (u32 i = 0; i < accessor.count; ++i) {
                            colors.push_back(glm::vec4(static_cast<float>(data[i][0]) / 255.f, static_cast<float>(data[i][1]) / 255.f, static_cast<float>(data[i][2]) / 255.f, static_cast<float>(data[i][3]) / 255.f));
                        }
                        break;
                    }

                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
                        std::vector<glm::u16vec4> data(accessor.count);
                        memcpy(data.data(), buffer.data.data() + accessor.byteOffset + bufferView.byteOffset, bufferView.byteLength);
                        colors.reserve(colors.size() + accessor.count);
                        for (u32 i = 0; i < accessor.count; ++i) {
                            colors.push_back(glm::vec4(static_cast<float>(data[i][0]) / 65535.f, static_cast<float>(data[i][1]) / 65535.f, static_cast<float>(data[i][2]) / 65535.f, static_cast<float>(data[i][3]) / 65535.f));
                        }
                        break;
                    }

                    case TINYGLTF_COMPONENT_TYPE_FLOAT: {
                        std::vector<glm::vec3> data(accessor.count);
                        memcpy(data.data(), buffer.data.data() + accessor.byteOffset + bufferView.byteOffset, bufferView.byteLength);
                        colors.insert(colors.end(), data.begin(), data.end());
                        break;
                    }

                    default:
                        break;
                    }
                    break;
                }

                default:
                    break;
                }
            }

            if (model.meshes[i].primitives[j].attributes.count("JOINTS_0") != 0) {
                auto& accessor = model.accessors[model.meshes[i].primitives[j].attributes["JOINTS_0"]];
                auto& bufferView = model.bufferViews[accessor.bufferView];
                auto& buffer = model.buffers[bufferView.buffer];

                switch (accessor.componentType) {
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
                    std::vector<glm::u8vec4> data(accessor.count);
                    memcpy(data.data(), buffer.data.data() + accessor.byteOffset + bufferView.byteOffset, bufferView.byteLength);
                    joints.reserve(joints.size() + accessor.count);
                    for (u32 i = 0; i < accessor.count; ++i) {
                        joints.push_back(glm::u16vec4(data[i]));
                    }
                    break;
                }

                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
                    std::vector<glm::u16vec4> data(accessor.count);
                    memcpy(data.data(), buffer.data.data() + accessor.byteOffset + bufferView.byteOffset, bufferView.byteLength);
                    joints.insert(joints.begin(), data.begin(), data.end());
                    break;
                }

                default:
                    break;
                }
            }

            if (model.meshes[i].primitives[j].attributes.count("WEIGHTS_0") != 0) {
                auto& accessor = model.accessors[model.meshes[i].primitives[j].attributes["WEIGHTS_0"]];
                auto& bufferView = model.bufferViews[accessor.bufferView];
                auto& buffer = model.buffers[bufferView.buffer];

                switch  (accessor.componentType) {
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
                    std::vector<glm::u8vec2> data(accessor.count);
                    memcpy(data.data(), buffer.data.data() + accessor.byteOffset + bufferView.byteOffset, bufferView.byteLength);
                    weights.reserve(weights.size() + accessor.count);
                    for (u32 i = 0; i < accessor.count; ++i) {
                        weights.push_back(glm::vec4(static_cast<float>(data[i][0]) / 255.f, static_cast<float>(data[i][1]) / 255.f, static_cast<float>(data[i][2]) / 255.f, static_cast<float>(data[i][3]) / 255.f));
                    }
                    break;
                }

                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
                    std::vector<glm::u16vec2> data(accessor.count);
                    memcpy(data.data(), buffer.data.data() + accessor.byteOffset + bufferView.byteOffset, bufferView.byteLength);
                    weights.reserve(weights.size() + accessor.count);
                    for (u32 i = 0; i < accessor.count; ++i) {
                        weights.push_back(glm::vec4(static_cast<float>(data[i][0]) / 65535.f, static_cast<float>(data[i][1]) / 65535.f, static_cast<float>(data[i][2]) / 65535.f, static_cast<float>(data[i][3]) / 65535.f));
                    }
                    break;
                }

                case TINYGLTF_COMPONENT_TYPE_FLOAT: {
                    std::vector<glm::vec4> data(accessor.count);
                    memcpy(data.data(), buffer.data.data() + accessor.byteOffset + bufferView.byteOffset, bufferView.byteLength);
                    weights.insert(weights.begin(), data.begin(), data.end());
                    break;
                }

                default:
                    break;
                }
            }

            if (model.meshes[i].primitives[j].indices != -1) {
                auto& accessor = model.accessors[model.meshes[i].primitives[j].indices];
                auto& bufferView = model.bufferViews[accessor.bufferView];
                auto& buffer = model.buffers[bufferView.buffer];
                meshes[i].primitives[j].indexCount = accessor.count;

                switch (accessor.componentType) {
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
                    std::vector<u8> data(accessor.count);
                    memcpy(data.data(), buffer.data.data() + accessor.byteOffset + bufferView.byteOffset, bufferView.byteLength);
                    indices.reserve(indices.size() + accessor.count);
                    for (u32 i = 0; i < accessor.count; ++i) {
                        indices.push_back(data[i]);
                    }
                    break;
                }

                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
                    std::vector<u16> data(accessor.count);
                    memcpy(data.data(), buffer.data.data() + accessor.byteOffset + bufferView.byteOffset, bufferView.byteLength);
                    indices.reserve(indices.size() + accessor.count);
                    for (u32 i = 0; i < accessor.count; ++i) {
                        indices.push_back(data[i]);
                    }
                    break;
                }

                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: {
                    std::vector<u32> data(accessor.count);
                    memcpy(data.data(), buffer.data.data() + accessor.byteOffset + bufferView.byteOffset, bufferView.byteLength);
                    indices.insert(indices.begin(), data.begin(), data.end());
                    break;
                }

                default:
                    // Debugging
                    assert(false);
                }
            }
        }
    }

    
}

void GltfModel::loadImages(Globals const& globals)
{
    images.resize(model.images.size());
    for (u32 i = 0; i < model.images.size(); ++i) {
        Buffer stagingBuffer;
        stagingBuffer.size = model.images[i].width * model.images[i].height * 4;
        stagingBuffer.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        stagingBuffer.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        createBuffer(globals, stagingBuffer);
        vkMapMemory(globals.device.handle, stagingBuffer.memory, 0, stagingBuffer.size, 0, &stagingBuffer.mapped);
        memcpy(stagingBuffer.mapped, model.images[i].image.data(), stagingBuffer.size);
        vkUnmapMemory(globals.device.handle, stagingBuffer.memory);

        images[i].width = model.images[i].width;
        images[i].height = model.images[i].height;
        images[i].usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        images[i].memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        createImage(globals, images[i]);
        transitionImageLayout(globals, images[i], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        copyBufferToImage(globals, stagingBuffer, images[i]);
        destroyBuffer(globals, stagingBuffer);
        transitionImageLayout(globals, images[i], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        createImageView(globals, images[i]);
    }
}

void GltfModel::loadSamplers(Globals const& globals)
{
    samplers.resize(model.images.size());
    for (u32 i = 0; i < model.images.size(); ++i) {
        createSampler(globals, samplers[i]);
    }
}

void GltfModel::loadMaterials()
{
    materials.resize(model.materials.size());
    for (u32 i = 0; i < model.materials.size(); ++i) {
        materials[i].shininess = 32.f;
        materials[i].diffuseTexIndex = model.materials[i].pbrMetallicRoughness.baseColorTexture.index;
    }
}

void GltfModel::createFrameResources(Globals const& globals)
{
    frameResources.resize(framesInFlight);
    for (u32 i = 0; i < framesInFlight; ++i) {
        {
            Buffer stagingBuffer;
            stagingBuffer.size = materials.size() * sizeof(materials[0]);
            stagingBuffer.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            stagingBuffer.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            createBuffer(globals, stagingBuffer);
            THROW_IF_FAILED(
                vkMapMemory(globals.device.handle, stagingBuffer.memory, 0, stagingBuffer.size, 0, &stagingBuffer.mapped),
                __FILE__, __LINE__,
                "Failed to map memory");
            memcpy(stagingBuffer.mapped, materials.data(), stagingBuffer.size);
            vkUnmapMemory(globals.device.handle, stagingBuffer.memory);

            frameResources[i].materialBuffer.size = stagingBuffer.size;
            frameResources[i].materialBuffer.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
            frameResources[i].materialBuffer.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            createBuffer(globals, frameResources[i].materialBuffer);
            copyBuffer(globals, stagingBuffer, frameResources[i].materialBuffer);
            destroyBuffer(globals, stagingBuffer);
        }
    }


}

void GltfModel::createDescriptors(Globals const& globals)
{
    resourceDescriptors.resize(2);
    {
        std::vector<VkDescriptorPoolSize> poolSizes(2);
        poolSizes[0] = Initializer::descriptorPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, framesInFlight);
        poolSizes[1] = Initializer::descriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, framesInFlight * images.size());
        auto descriptorPoolCreateInfo = Initializer::descriptorPoolCreateInfo(framesInFlight, poolSizes);
        THROW_IF_FAILED(
            vkCreateDescriptorPool(globals.device.handle, &descriptorPoolCreateInfo, globals.allocator, &resourceDescriptors[0].pool),
            __FILE__, __LINE__,
            "Failed to create descriptor pool");

        std::vector<VkDescriptorSetLayoutBinding> bindings(2);
        bindings[0] = Initializer::descriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        bindings[1] = Initializer::descriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, images.size(), VK_SHADER_STAGE_FRAGMENT_BIT);
        std::vector<VkDescriptorBindingFlags> descriptorBindingFlags(2);
        descriptorBindingFlags[0] = 0;
        descriptorBindingFlags[1] = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT;
        auto descriptorSetLayoutBindingFlagsCreateInfo = Initializer::descriptorSetLayoutBindingFlagsCreateInfo(descriptorBindingFlags);
        auto descriptorSetLayoutCreateInfo = Initializer::descriptorSetLayoutCreateInfo(bindings, &descriptorSetLayoutBindingFlagsCreateInfo);
        THROW_IF_FAILED(
            vkCreateDescriptorSetLayout(globals.device.handle, &descriptorSetLayoutCreateInfo, globals.allocator, &resourceDescriptors[0].setLayout),
            __FILE__, __LINE__,
            "Failed to create descriptor set layout");

        std::vector<u32> descriptorCounts(framesInFlight, images.size());
        auto descriptorSetVariableDescriptorCountAllocateInfo = Initializer::descriptorSetVariableDescriptorCountAllocateInfo(descriptorCounts);
        std::vector<VkDescriptorSetLayout> setLayouts(framesInFlight, resourceDescriptors[0].setLayout);
        auto descriptorSetAllocateInfo = Initializer::descriptorSetAllocateInfo(
            resourceDescriptors[0].pool,
            framesInFlight,
            setLayouts,
            &descriptorSetVariableDescriptorCountAllocateInfo);
        resourceDescriptors[0].sets.resize(framesInFlight);
        THROW_IF_FAILED(
            vkAllocateDescriptorSets(globals.device.handle, &descriptorSetAllocateInfo, resourceDescriptors[0].sets.data()),
            __FILE__, __LINE__,
            "Failed to allocate descriptor sets");

        for (u32 i = 0; i < framesInFlight; ++i) {
            std::vector<VkDescriptorBufferInfo> bufferDescriptors(1);
            bufferDescriptors[0] = Initializer::descriptorBufferInfo(frameResources[i].materialBuffer.handle, 0);
            std::vector<VkDescriptorImageInfo> imageDescriptors(images.size());
            for (u32 j = 0; j < imageDescriptors.size(); ++j) {
                imageDescriptors[j].sampler = samplers[j].handle;
                imageDescriptors[j].imageView = images[j].view;
                imageDescriptors[j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            }
            std::vector<VkWriteDescriptorSet> descriptorWrites(2);
            descriptorWrites[0] = Initializer::writeDescriptorSet(
                resourceDescriptors[1].sets[i],
                0, 0, bufferDescriptors.size(),
                VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                nullptr, bufferDescriptors.data(), nullptr);
            descriptorWrites[1] = Initializer::writeDescriptorSet(
                resourceDescriptors[1].sets[i],
                1, 0, imageDescriptors.size(),
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                imageDescriptors.data(), nullptr, nullptr);
            vkUpdateDescriptorSets(globals.device.handle, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
        }
    }
    {
        std::vector<VkDescriptorPoolSize> poolSizes(1);
        poolSizes[0] = Initializer::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, framesInFlight);
        auto descriptorPoolCreateInfo = Initializer::descriptorPoolCreateInfo(framesInFlight, poolSizes);
        THROW_IF_FAILED(
            vkCreateDescriptorPool(globals.device.handle, &descriptorPoolCreateInfo, globals.allocator, &resourceDescriptors[2].pool),
            __FILE__, __LINE__,
            "Failed to create descriptor pool");

        std::vector<VkDescriptorSetLayoutBinding> bindings(1);
        bindings[0] = Initializer::descriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
        auto descriptorSetLayoutCreateInfo = Initializer::descriptorSetLayoutCreateInfo(bindings);
        THROW_IF_FAILED(
            vkCreateDescriptorSetLayout(globals.device.handle, &descriptorSetLayoutCreateInfo, globals.allocator, &resourceDescriptors[2].setLayout),
            __FILE__, __LINE__,
            "Failed to create descriptor set layout");

        std::vector<VkDescriptorSetLayout> setLayouts(framesInFlight, resourceDescriptors[2].setLayout);
        auto descriptorSetAllocateInfo = Initializer::descriptorSetAllocateInfo(resourceDescriptors[2].pool, framesInFlight, setLayouts);
        resourceDescriptors[2].sets.resize(framesInFlight);
        THROW_IF_FAILED(
            vkAllocateDescriptorSets(globals.device.handle, &descriptorSetAllocateInfo, resourceDescriptors[2].sets.data()),
            __FILE__, __LINE__,
            "Failed to allocate descriptor sets");

        for (u32 i = 0; i < framesInFlight; ++i) {
            std::vector<VkDescriptorBufferInfo> bufferDescriptors(1);
            bufferDescriptors[0] = Initializer::descriptorBufferInfo(frameResources[i].renderObjectBuffer.handle, 0, sizeof(renderObjects[0]));
            std::vector<VkWriteDescriptorSet> descriptorWrites(1);
            descriptorWrites[0] = Initializer::writeDescriptorSet(
                resourceDescriptors[2].sets[i],
                0, 0, bufferDescriptors.size(),
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
                nullptr, bufferDescriptors.data(), nullptr);
            vkUpdateDescriptorSets(globals.device.handle, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
        }
    }
}
