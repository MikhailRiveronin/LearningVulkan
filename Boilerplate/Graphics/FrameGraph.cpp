#include "FrameGraph.h"

#include <nlohmann/json.hpp>
#include <fstream>

static FrameGraphResourceType stringToResourceType(std::string const& type)
{
    if (type == "buffer") {
        return FrameGraphResourceType::BUFFER;
    }

    if (type == "texture") {
        return FrameGraphResourceType::TEXTURE;
    }

    if (type == "attachment") {
        return FrameGraphResourceType::ATTACHMENT;
    }

    if (type == "reference") {
        return FrameGraphResourceType::REFERENCE;
    }

    assert(false);
}

static VkFormat stringToFormat(std::string const& format)
{
    if (format == "VK_FORMAT_B8G8R8A8_UNORM") {
        return VK_FORMAT_B8G8R8A8_UNORM;
    }

    if (format == "VK_FORMAT_R16G16B16A16_SFLOAT") {
        return VK_FORMAT_R16G16B16A16_SFLOAT;
    }

    if (format == "VK_FORMAT_D32_SFLOAT") {
        return VK_FORMAT_D32_SFLOAT;
    }

    if (format == "VK_FORMAT_R8G8B8A8_SRGB") {
        return VK_FORMAT_R8G8B8A8_SRGB;
    }

    assert(false);
}

static VkAttachmentLoadOp stringToLoadOp(std::string const& loadOp)
{
    if (loadOp == "VK_ATTACHMENT_LOAD_OP_CLEAR") {
        return VK_ATTACHMENT_LOAD_OP_CLEAR;
    }

    if (loadOp == "VK_ATTACHMENT_LOAD_OP_LOAD") {
        return VK_ATTACHMENT_LOAD_OP_LOAD;
    }

    assert(false);
}

void FrameGraph::parse(std::string const& filename)
{
    using json = nlohmann::json;

    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file");
    }

    auto graph = json::parse(file);
    auto graphName = graph.value("name", "");

    auto passes = graph["passes"];
    nodes.resize(passes.size());
    for (u32 i = 0; i < passes.size(); ++i) {
        auto& pass = passes[i];
        nodes[i].name = pass.value("name", "");
        auto& inputs = pass["inputs"];
        nodes[i].inputs.resize(inputs.size());
        for (u32 j = 0; j < inputs.size(); ++j) {
            nodes[i].inputs[j].type = stringToResourceType(inputs[j]["type"]);
            nodes[i].inputs[j].name = inputs[j].value("name", "");
        }

        auto& outputs = pass["outputs"];
        nodes[i].outputs.resize(outputs.size());
        for (u32 j = 0; j < outputs.size(); ++j) {
            nodes[i].outputs[j].type = stringToResourceType(outputs[j]["type"]);
            nodes[i].outputs[j].name = outputs[j].value("name", "");

            switch (nodes[i].outputs[j].type) {
            case FrameGraphResourceType::TEXTURE:
            case FrameGraphResourceType::ATTACHMENT: {
                nodes[i].outputs[j].image.format = stringToFormat(outputs[j]["format"]);
                nodes[i].outputs[j].image.loadOp = stringToLoadOp(outputs[j]["op"]);

                auto resolution = outputs[j]["resolution"];
                nodes[i].outputs[j].image.width = resolution[0];
                nodes[i].outputs[j].image.height = resolution[1];
            } break;

            case FrameGraphResourceType::BUFFER:
            case FrameGraphResourceType::REFERENCE:
                assert(false);
            default:
                assert(false);
            }
        }
    }


}
