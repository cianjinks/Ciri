#include "GLTFImporter.h"

#include "tiny_gltf.h"

namespace Ciri
{
    bool GLTFImporter::Import(Scene* scene, S<SceneNode> container, std::string filepath)
    {
        tinygltf::TinyGLTF loader;
        tinygltf::Model model;
        std::string error;
        std::string warning;

        bool ret = loader.LoadASCIIFromFile(&model, &error, &warning, filepath.c_str());

        if (!warning.empty())
        {
            CIRI_WARN("[GLTF] {}", warning);
        }

        if (!error.empty())
        {
            CIRI_ERROR("[GLTF] Error importing model: {}", filepath);
            CIRI_ERROR("[GLTF] {}", error);
            return false;
        }

        if (!ret)
        {
            return false;
        }

        const tinygltf::Scene &default_scene = model.scenes[model.defaultScene];
        std::stack<const tinygltf::Node&> node_stack;

        for (size_t i = 0; i < default_scene.nodes.size(); ++i) {
            const tinygltf::Node& node = model.nodes[default_scene.nodes[i]];
            node_stack.push(node);
        }

        S<SceneNode> current_node = container;
        while (!node_stack.empty())
        {
            const tinygltf::Node& node = node_stack.top();
            node_stack.pop();

            S<SceneNode> scene_node = CreateS<SceneNode>();

            const tinygltf::Mesh& mesh = model.meshes[node.mesh];
            for (size_t i = 0; i < model.bufferViews.size(); i++)
            {
                for (size_t j = 0; j < mesh.primitives.size(); j++) {
                    const tinygltf::Primitive& primitive = mesh.primitives[i];
                    const tinygltf::Accessor& accessor = model.accessors[primitive.indices];
                    const tinygltf::BufferView& buffer_view = model.bufferViews[accessor.bufferView];
                    if (buffer_view.target == 0)
                    {
                        CIRI_WARN("[GLTF] Target buffer view is 0!");
                        continue;
                    }

                    const tinygltf::Buffer& buffer = model.buffers[buffer_view.buffer];
                    

                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos.at(indexAccessor.bufferView));

                    glDrawElements(primitive.mode, indexAccessor.count,
                                indexAccessor.componentType,
                                BUFFER_OFFSET(indexAccessor.byteOffset));
                }
            }
    
            for (size_t c = 0; c < node.children.size(); c++)
            {
                const tinygltf::Node& child = model.nodes[node.children[c]];
                node_stack.push(child);
            }
        }

        return true;
    }
}