#include "scene_node.h"

Scene_Node::Scene_Node(std::string const& name, glm::mat4 const& local_transform, Scene_Node* parent) : name(name), local_transform(local_transform), parent(parent)
{
}

void Scene_Node::create_child(std::string const& name, glm::mat4 const& local_transform)
{
    children.push_back(std::make_unique<Scene_Node>(name, local_transform, this));
}

void Scene_Node::attach_movable(std::string const& name, Movable* movable)
{
    movables.emplace(name, movable);
}
