#pragma once

#include "defines.h"

#include "resource.h"

class Render_Subpass
{
public:
    VkPipeline pipeline;

};

class Material : public Resource
{
public:
    std::vector<Render_Subpass> subpasses;

};
