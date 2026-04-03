#include "scene.h"

#include "utils.h"

static void save_map(FILE* file, std::unordered_map<u32, u32> const& map);
static void load_map(FILE* file, std::unordered_map<u32, u32>& map);

i32 Scene::add_node(i32 parent, i32 level)
{
    i32 node = static_cast<i32>(hierarchies.size());
    hierarchies.push_back({ .parent = parent, .last_sibling = -1 });
    if (parent != -1)
    {
        if (hierarchies[parent].first_child == -1)
        {
            hierarchies[parent].first_child = node;
            hierarchies[node].last_sibling = node;
        }
        else
        {
            i32 dest = hierarchies[hierarchies[parent].first_child].last_sibling;
            if (dest == -1)
            {
                for (dest = hierarchies[parent].first_child; hierarchies[dest].next_sibling != -1; dest = hierarchies[dest].next_sibling)
                {
                }
            }

            hierarchies[dest].next_sibling = node;
            hierarchies[hierarchies[parent].first_child].last_sibling = node;
        }
    }

    hierarchies[node].first_child = -1;
    hierarchies[node].next_sibling = -1;
    hierarchies[node].level = level;

    local_transforms.push_back(glm::mat4(1.0f));
    global_transforms.push_back(glm::mat4(1.0f));

    return node;
}

void Scene::save_to_file(char const* filename) const
{
    FILE* file = fopen(filename, "wb");
    if (!file)
    {
        // Handle error
        return;
    }

    u32 size = static_cast<u32>(hierarchies.size());
    fwrite(&size, sizeof(size), 1, file);

    fwrite(hierarchies.data(), sizeof(Hierarchy), size, file);
    fwrite(local_transforms.data(), sizeof(glm::mat4), size, file);
    fwrite(global_transforms.data(), sizeof(glm::mat4), size, file);

    save_map(file, node_to_material);
    save_map(file, node_to_mesh);

    if (!node_names.empty() && !node_to_name.empty())
    {
        save_map(file, node_to_name);
        save_strings(file, node_names);
        save_strings(file, material_names);
    }

    fclose(file);
}

void Scene::load_from_file(char const* filename)
{
    FILE* file = fopen(filename, "rb");
    if (!file)
    {
        // Handle error
        return;
    }

    u32 size;
    fread(&size, sizeof(size), 1, file);
    hierarchies.resize(size);
    local_transforms.resize(size);
    global_transforms.resize(size);

    fread(hierarchies.data(), sizeof(Hierarchy), size, file);
    fread(local_transforms.data(), sizeof(glm::mat4), size, file);
    fread(global_transforms.data(), sizeof(glm::mat4), size, file);

    load_map(file, node_to_mesh);
    load_map(file, node_to_material);

    if (!feof(file))
    {
        load_map(file, node_to_name);
        load_strings(file, node_names);
        load_strings(file, material_names);
    }

    fclose(f);
}

void Scene::mark_as_changed(i32 node)
{
    changed_at_this_frame[hierarchies[node].level].push_back(node);

    for (i32 first_child = hierarchies[node].first_child; first_child != -1; first_child = hierarchies[node].next_sibling)
    {
        mark_as_changed(first_child);
    }
}

void Scene::recalculate_global_transforms()
{
    if (!changed_at_this_frame[0].empty())
    {
        global_transforms[changed_at_this_frame[0][0]] = local_transforms[changed_at_this_frame[0][0]];
        changed_at_this_frame[0].clear();
    }

    for (u32 level = 1; level < MAX_NODE_LEVEL; ++level)
    {
        if (!changed_at_this_frame[level].empty())
        {
            for (int changed : changed_at_this_frame[level])
            {
                global_transforms[changed] = global_transforms[hierarchies[changed].parent] * local_transforms[changed];
            }

            changed_at_this_frame[level].clear();
        }
    }
}

void save_map(FILE* file, std::unordered_map<u32, u32> const& map)
{
    std::vector<u32> data;
    data.reserve(map.size() * 2);
    for (auto& pair : map)
    {
        data.push_back(pair.first);
        data.push_back(pair.second);
    }

    u32 size = static_cast<u32>(data.size());
    fwrite(&size, sizeof(size), 1, file);
    fwrite(data.data(), sizeof(u32), data.size(), file);
}

void load_map(FILE* file, std::unordered_map<u32, u32>& map)
{
    u32 size = 0;
    fread(&size, 1, sizeof(size), file);

    std::vector<u32> data(size);
    fread(data.data(), sizeof(u32), size, file);
    for (u32 i = 0; i < (size / 2); ++i)
    {
        map[data[i * 2]] = data[i * 2 + 1];
    }
}


















#include "mesh_manager.h"
#include "third_party/tiny_gltf.h"

void add_all_scenes_from_gltf(std::string const& filename)
{
    tinygltf::TinyGLTF loader;
    tinygltf::Model model;
    std::string errors;
    std::string warnings;
    if (!loader.LoadASCIIFromFile(&model, &errors, &warnings, filename))
    {
        // TODO: handle error
    }

    for (u32 scene_index = 0; scene_index < model.scenes.size(); ++scene_index)
    {
        Scene scene;
        auto set_node = [=, &model](Scene_Node& node, tinygltf::Node const& tinygltf_node, Scene_Node* parent)
        {
            node.name = tinygltf_node.name;
            node.parent = parent;

            node.local_transform = glm::mat4(1.0);
            if (!tinygltf_node.matrix.empty())
            {
                node.local_transform = glm::make_mat4(tinygltf_node.matrix.data());
            }
            else
            {
                if (!tinygltf_node.translation.empty())
                {
                    node.local_transform *= glm::translate(glm::mat4(1.0), glm::make_vec3(tinygltf_node.translation.data()));
                }

                if (!tinygltf_node.rotation.empty())
                {
                    node.local_transform *= glm::mat4_cast(glm::make_quat(tinygltf_node.rotation.data()));
                }

                if (tinygltf_node.scale.empty())
                {
                    node.local_transform *= glm::scale(glm::mat4(1.0), glm::make_vec3(tinygltf_node.scale.data()));
                }
            }

            if (tinygltf_node.camera != -1 && model.cameras[tinygltf_node.camera].type == "perspective")
            {
                auto camera = std::make_shared<Camera>();
                camera->name = model.cameras[tinygltf_node.camera].name;
                camera->aspect_ratio = model.cameras[tinygltf_node.camera].perspective.aspectRatio;
                camera->y_fov = model.cameras[tinygltf_node.camera].perspective.yfov;
                camera->z_near = model.cameras[tinygltf_node.camera].perspective.znear;
                camera->z_far = model.cameras[tinygltf_node.camera].perspective.zfar;

                node.attach_object(camera);

                // scenes[scene_index].cameras.push_back(std::move(camera));
            }

            if (tinygltf_node.mesh != -1)
            {
                auto mesh_instance = create_mesh_instance(model.meshes[tinygltf_node.mesh].name + "_instance_0", model.meshes[tinygltf_node.mesh].name);
                node.attach_object(mesh_instance);
            }

            node.children.resize(tinygltf_node.children.size());
            for (u32 child_index = 0; child_index < node.children.size(); ++child_index)
            {
                set_node(node.children[child_index], model.nodes[tinygltf_node.children[child_index]], &node);
            }
        }

        scene.root_nodes.resize(model.scenes[scene_index].nodes.size());
        for (u32 root_node_index = 0; root_node_index < scenes[scene_index].root_nodes.size(); ++root_node_index)
        {
            set_node(scene.root_nodes[root_node_index], model.nodes[model.scenes[scene_index].nodes[root_node_index]], nullptr);
        }

        scenes.push_back(std::move(scene));
    }
}

void Scene_Manager::load_from_gltf(std::string const& filename)
{
    tinygltf::TinyGLTF loader;
    tinygltf::Model model;
    std::string errors;
    std::string warnings;
    if (!loader.LoadASCIIFromFile(&model, &errors, &warnings, filename))
    {
        // TODO: handle error
    }

    scenes.resize(model.scenes.size());
    for (u32 scene_index = 0; scene_index < scenes.size(); ++scene_index)
    {
        auto set_node = [=, &model](Scene_Node& node, tinygltf::Node const& tinygltf_node, Scene_Node* parent)
        {
            node.name = tinygltf_node.name;
            node.parent = parent;

            node.local_transform = glm::mat4(1.0);
            if (!tinygltf_node.matrix.empty())
            {
                node.local_transform = glm::make_mat4(tinygltf_node.matrix.data());
            }
            else
            {
                if (!tinygltf_node.translation.empty())
                {
                    node.local_transform *= glm::translate(glm::mat4(1.0), glm::make_vec3(tinygltf_node.translation.data()));
                }

                if (!tinygltf_node.rotation.empty())
                {
                    node.local_transform *= glm::mat4_cast(glm::make_quat(tinygltf_node.rotation.data()));
                }

                if (tinygltf_node.scale.empty())
                {
                    node.local_transform *= glm::scale(glm::mat4(1.0), glm::make_vec3(tinygltf_node.scale.data()));
                }
            }

            if (tinygltf_node.camera != -1 && model.cameras[tinygltf_node.camera].type == "perspective")
            {
                Camera camera;
                camera.name = model.cameras[tinygltf_node.camera].name;
                camera.aspect_ratio = model.cameras[tinygltf_node.camera].perspective.aspectRatio;
                camera.y_fov = model.cameras[tinygltf_node.camera].perspective.yfov;
                camera.z_near = model.cameras[tinygltf_node.camera].perspective.znear;
                camera.z_far = model.cameras[tinygltf_node.camera].perspective.zfar;
                scenes[scene_index].cameras.push_back(std::move(camera));
            }

            if (tinygltf_node.mesh != -1)
            {
                auto mesh_instance = create_mesh_instance(model.meshes[tinygltf_node.mesh].name + "_instance_0", model.meshes[tinygltf_node.mesh].name);
                node.attach_object(mesh_instance);
            }

            node.children.resize(tinygltf_node.children.size());
            for (u32 child_index = 0; child_index < node.children.size(); ++child_index)
            {
                set_node(node.children[child_index], model.nodes[tinygltf_node.children[child_index]], &node);
            }
        }

        scenes[scene_index].root_nodes.resize(model.scenes[scene_index].nodes.size());
        for (u32 root_node_index = 0; root_node_index < scenes[scene_index].root_nodes.size(); ++root_node_index)
        {
            set_node(scenes[scene_index].root_nodes[root_node_index], model.nodes[model.scenes[scene_index].nodes[root_node_index]], nullptr);
        }
    }
}

void Scene_Manager::create_scene_node()
{
    scene_nodes.push_back(std::make_unique<Scene_Node>());
}

void Scene_Manager::create_camera(std::string const& name, glm::vec3 const& position, float aspect_ratio, float y_fov, float z_near, float z_far)
{
    cameras.emplace(name, std::make_unique<Camera>(glm::vec3 const& position, float aspect_ratio, float y_fov, float z_near, float z_far));
}

std::shared_ptr<Mesh_Instance> Scene_Manager::create_mesh_instance(std::string const& name, std::string const& mesh_name)
{
    return std::make_shared<Mesh_Instance>(std::string const& name, Mesh_Manager::get_instance()->load(mesh_name));
}

void Scene_Manager::render_scene(u32 camera_index = 0, u32 scene_index = 0)
{
    // apply animation


    // set viewport


    // setCurrentRenderTarget


    // prepareRenderQueue

    renderer.begin_frame();


    // Render visible objects
    for (u32 i = 0; i < static_cast<u32>(Render_Queue_Group_Type::ENUM_COUNT); ++i)
    {

    }


    renderer.end_frame();

}


void Scene_Manager::undate_descriptor_sets()
{

}

void Scene_Manager::Scene_Manager_Renderable_Visitor::visit(Opaque_Renderable_Collection* collection)
{
    // Draw call
}

void Scene_Manager::Scene_Manager_Renderable_Visitor::visit(Transparent_Renderable_Collection* collection)
{
    // Draw call
}
