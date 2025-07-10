#pragma once

#include "Boilerplate/Structures.h"

#include <string>
#include <vector>

enum class FrameGraphResourceType {
    BUFFER,
    TEXTURE,
    ATTACHMENT,
    REFERENCE
};

struct FrameGraphResource {
    FrameGraphResourceType type;

    union {
        Buffer buffer;
        Image image;
    };

    u32 producerNode;
    u32 outputResource;

    u32 refCount = 0;
    std::string name;
};

struct FrameGraphRenderPass {

};

struct FrameGraphNode {
    u32 refCount;
    u32 renderPass;
    u32 framebuffer;
    FrameGraphRenderPass* graphRenderPass;

    std::vector<FrameGraphResource> inputs;
    std::vector<FrameGraphResource> outputs;
    std::vector<u32> edges;

    std::string name;
};

struct FrameGraph {

    std::vector<FrameGraphNode> nodes;

    void parse(std::string const& filename);
};
