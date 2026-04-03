#pragma once

#include "defines.h"

constexpr const u32 MAX_NODE_LEVEL = 16;

struct Hierarchy
{
    i32 parent = -1;
    i32 first_child = -1;
    i32 next_sibling = -1;
    i32 last_sibling = -1;
    i32 level = 0;
};

// Left-child right-sibling binary tree
class Scene
{
public:
    std::vector<Hierarchy> hierarchies;
    std::vector<glm::mat4> local_transforms;
    std::vector<glm::mat4> global_transforms;

    std::unordered_map<u32, u32> node_to_mesh;
    std::unordered_map<u32, u32> node_to_material;
    std::unordered_map<u32, u32> node_to_name;

    std::vector<std::string> node_names;
    std::vector<std::string> material_names;

    std::vector<int> changed_at_this_frame[MAX_NODE_LEVEL];

    i32 add_node(i32 parent, i32 level);
    void save_to_file(char const* filename) const;
    void load_from_file(char const* filename);
    void mark_as_changed(i32 node);
    void recalculate_global_transforms();
};


















#include "render_queue.h"
#include "renderer.h"
#include "scene_node.h"
#include "mesh_instance.h"
#include "third_party/glm/glm.hpp"

class Scene_Manager
{
public:

    static Scene_Manager* get_instance();

    std::string name;
    std::vector<Scene_Node> root_nodes;
    std::vector<Scene_Node*> camera_nodes;



    Render_Queue render_queue;
    Renderer renderer;






    std::vector<std::unique_ptr<Scene_Node>> scene_nodes;
    std::unordered_map<std::string, std::unique_ptr<Camera>> cameras;

    void add_root_node(Scene_Node const& node, glm::mat4 const& transform = glm::mat4(1.f));
    void add_root_nodes_from_gltf(std::string const& filename, glm::mat4 const& transform = glm::mat4(1.f));
    void add_root_nodes_from_gltf(std::string const& filename, u32 scene_index = 0, glm::mat4 const& transform = glm::mat4(1.f));
    void add_node(Scene_Node const* parent);





    void load_from_gltf(std::string const& filename);

    void create_scene_node();
    void create_camera(std::string const& name, glm::vec3 const& position, float aspect_ratio, float y_fov, float z_near, float z_far);
    std::shared_ptr<Mesh_Instance> create_mesh_instance(std::string const& name, std::string const& mesh_name);

    void render_scene(u32 scene_index = 0, u32 camera_index = 0);



    class Scene_Manager_Renderable_Visitor : public Renderable_Visitor
    {
    public:
        void visit(Opaque_Renderable_Collection* collection) override;
        void visit(Transparent_Renderable_Collection* collection) override;
    }


private:
    void undate_descriptor_sets();


};
