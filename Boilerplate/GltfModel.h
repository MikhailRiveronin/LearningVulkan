#pragma once

#include "Defines.h"
#include "Structures.h"

#include <glm/glm.hpp>
#include <tiny_gltf.h>
#include <string>
#include <vector>

class GltfModel {
public:
    void load(std::string filename);

private:
    struct Node {
        glm::mat4 local;
    };

    struct Mesh {
        struct Primitive {
            // std::vector<glm::vec3> positions;
            // std::vector<glm::vec3> normals;
            // std::vector<glm::vec3> tangents;
            // std::vector<glm::vec2> texCoord;
            // std::vector<glm::vec4> colors;
            // std::vector<glm::u16vec4> joints;
            // std::vector<glm::vec4> weights;
            // std::vector<u16> indices;

            u32 indexCount = 0;
            u32 firstIndex = 0;
            i32 vertexOffset = 0;
        };
        std::vector<Primitive> primitives;
    };

    struct {
        Buffer pos;
        Buffer normal;
        Buffer texCoord;
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
    std::vector<ResourceDescriptor> resourceDescriptors;

    void loadNodes();
    void loadMeshes();
    void loadImages(Globals const& globals);
    void loadSamplers(Globals const& globals);
    void loadMaterials();

    void createFrameResources(Globals const& globals);
    void createDescriptors(Globals const& globals);
};
