#include "scene_manager.h"

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
