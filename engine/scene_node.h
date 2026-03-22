#pragma once

#include "defines.h"

#include "camera.h"
#include "object.h"
#include "third_party/glm/glm.hpp"

class Scene_Manager;

class Scene_Node
{
public:
    std::string name;
    Scene_Node* parent;
    glm::mat4 local_transform;
    std::vector<Scene_Node> children;

    std::unordered_map<std::string, Movable*> movables;

    void attach_object(std::shared_ptr<Object> object);





    Scene_Node(std::string const& name, glm::mat4 const& local_transform, Scene_Node* parent = nullptr);

    void create_child(std::string const& name, glm::mat4 const& local_transform);

    Scene_Manager* manager;

    




    // Camera camera;

    void attach_camera(Camera const& camera);


private:

};
