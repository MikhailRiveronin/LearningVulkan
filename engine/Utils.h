#pragma once

#include "defines.h"

#include "Logger.h"
#include "vulkan_classes.h"

#include <vulkan/vulkan.h>

#include <string>
#include <vector>
#include <unordered_map>

#include "mesh.h"

namespace engine
{

// #define VK_CHECK(result)                                       \
//     do {                                                              \
//         printf("Vulkan command failed: %s: %d\n", __FILE__, __LINE__);\
//         if (result != VK_SUCCESS) {                                   \
//             throw std::runtime_error("Vulkan command failed");        \
//         }                                                             \
//     } while (0)






bool has_extension(std::string const& filename, std::string const& extension);
std::string read_GLSL_from_file(std::string const& filename);
std::vector<u32 const> compile_GLSL_to_SPIRV(VkShaderStageFlagBits shader_stage, std::string const& source_code, glslang_resource_t const* glslang_resource);




Shader_Stage shader_stage_from_filename(const char* filename);

bool compile_shader_from_source(Shader_Stage stage, char const* code, );





RAII_Wrapper<Shander_Module_Handle> create_shader_module(Context const& context, std::string const& filename);






void create_buffer(Context const& context, VkDeviceSize size, VkBufferUsageFlags usage, VmaAllocationCreateFlags allocation_create_flags, Buffer& buffer);
void create_staging_buffer(Context const& context, VkDeviceSize size, Buffer& buffer);
void destroy_buffer(Context const& context, Buffer& buffer);

void create_image(Context const& context, VkFormat format, u32 width, u32 height, VkImageUsageFlags usage, VmaAllocationCreateFlags allocation_create_flags, Image& image);
void create_texture(Context const& context, VkFormat format, u32 width, u32 height, void const* data, Texture& texture);
void create_attachment(Context const& context, Attachment& attachment);
void destroy_image(Context const& context, Image& image);
void destroy_attachment(Context const& context, Attachment& attachment);

void copyBuffer(Context const& globals, Buffer& srcBuffer, Buffer& dstBuffer);
void copy_buffer_to_image(Context const& context, Buffer const& buffer, Image& image);
VkDeviceSize calculateUniformBufferAlignment(Context const& globals, VkDeviceSize size);

std::vector<char> load_shader_code(std::string const& filename);

void createImageView(Context const& globals, Image& image);
void transition_image_layout(Context const& context, VkImageLayout old_layout, VkImageLayout new_layout, Image& image);


u32 findMemoryTypeIndex(Context const& context, VkMemoryRequirements const& requirements, VkMemoryPropertyFlags properties);

VkCommandBuffer begin_command_buffer_one_time_submit(Context const& globals);
void end_command_buffer_one_time_submit(Context const& globals, VkCommandBuffer commandBuffer);

void createDescriptorSets(Context const& globals, std::vector<DescriptorSetBinding> const& descriptorSetBindings, DescriptorSets& descriptorSets);
void destroyDescriptorSets(Context const& globals, DescriptorSets& descriptorSets);

void create_pipeline(Context const& context,
    std::unordered_map<VkShaderStageFlagBits, std::string> shaderStages,
    VertexInputState const& vertexInputState,
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts,
    std::vector<VkPushConstantRange> pushConstantRanges,
    std::vector<PipelineCreateInfo> const& pipelineCreateInfos);
void destroyPipeline(Context const& globals, Pipeline& pipeline);

void create_pipeline_shader_stage(Context const& globals, ShaderStage shaderStage);

void createCubeTexture(
    Context const& context,
    char const* front, char const* back,
    char const* left, char const* right,
    char const* up, char const* down,
    Image& image);


void load_gltf(Context const& context, std::string const& filename, std::vector<Texture>& textures, Mesh_Data& mesh_data);

void save_strings(FILE* file, std::vector<std::string> const& strings);
void load_strings(FILE* file, std::vector<std::string>& strings);

} // namespace engine
