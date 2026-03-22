#pragma once

#include "defines.h"

#include "resource_manager.h"

class Shader_Manager : public Resource_Manager
{
public:
    std::vector<u32> code;

    static Shader_Manager* get_instance();

private:

};
