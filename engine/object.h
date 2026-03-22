#pragma once

#include "defines.h"

#include "scene_node.h"
#include "third_party/glm/glm.hpp

class Object
{
public:
    Scene_Node* parent_node;

    // From parent node
    glm:mat4 get_derived_transformation() const;


};

class Dynamic_Object : public Object
{

};

class Static_Object : public Object
{

};
