#pragma once

#include "Boilerplate/Structures.h"

#include <string>
#include <vector>
#include <unordered_map>

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

struct FrameGraphResourceCache {
    std::vector<FrameGraphResource> resources;
    std::unordered_map<u32, u32> resource_map;
};

struct Framebuffer {

};

struct FrameGraphNode {
    u32 refCount;
    u32 renderPass;
    u32 framebuffer;
    FrameGraphRenderPass* graphRenderPass;

    std::vector<FrameGraphResource> inputs;
    std::vector<FrameGraphResource> outputs;
    std::vector<u32> edges;

    Framebuffer framebuffer;

    std::string name;

    u32 renderPassIndex;
};

struct FrameGraphNodeCache {
    std::vector<FrameGraphNode> nodes;
    std::unordered_map<u32, u32> node_map;
};

struct FrameGraphBuilder {
    FrameGraphResourceCache resourceCache;
};

struct FrameGraphRenderPass {

};



struct FrameGraph {

    std::vector<FrameGraphNode> nodes;
    std::unordered_map<std::string, u32> node_cache;

    std::vector<Framebuffer> framebuffers;

    void parse(std::string const& filename);
    void compile();

private:
    void createRenderPasses();
    void createFramebuffers();
};
