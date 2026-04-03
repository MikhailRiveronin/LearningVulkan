#pragma once

#include "defines.h"

#include "vulkan_classes.h"

struct GLTF_Global_Samplers
{
    VkSampler clamp;
    VkSampler wrap;
    VkSampler mirror;
};

struct GLTF_Material_Texture
{
    Texture base_color_texture;

};

struct Environment_Map_Textures
{
    Texture brdf_lut;
    Texture environment_map;
};
