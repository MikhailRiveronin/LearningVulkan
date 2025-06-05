#pragma once

#include "Defines.h"
#include "Structures.h"

#include <glm/glm.hpp>
#include <string>
#include <vector>

#include <tiny_gltf.h>

class GltfModel {
public:
    void load(std::string filename);

    void loadNodes();
    void calculateGlobalTransform(u32 nodeIndex, glm::mat4 const& parentGlobalTransform);
    void loadMeshes(Globals const& globals);
    void loadImages(Globals const& globals);
    void loadSamplers(Globals const& globals);
    void loadMaterials();

    void createFrameResources(Globals const& globals);
    void createDescriptors(Globals const& globals);

    struct Node {
        glm::mat4 localTransform;
        glm::mat4 globalTransform;
    };

    struct Mesh {
        struct Primitive {
            u32 indexCount = 0;
            u32 firstIndex = 0;
            i32 vertexOffset = 0;
            u32 materialIndex = 0;
        };
        std::vector<Primitive> primitives;
    };

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec4> tangents;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec4> colors;
    std::vector<glm::u16vec4> joints;
    std::vector<glm::vec4> weights;
    std::vector<u32> indices;

    struct {
        Buffer positions;
        Buffer normals;
        Buffer tangents;
        Buffer texCoords;
        Buffer colors;
        Buffer joints;
        Buffer weights;
    } vertexBuffers;
    Buffer indexBuffer;

    struct Scene{
        std::vector<Node> rootNodes;
    };

    tinygltf::Model model;
    std::vector<Scene> scenes;

    std::vector<Node> nodes;
    std::vector<Mesh> meshes;
    std::vector<Image> images;
    std::vector<Sampler> samplers;
    std::vector<Material> materials;

    std::vector<FrameResource> frameResources;
    std::vector<DescriptorSets> resourceDescriptors;
};
