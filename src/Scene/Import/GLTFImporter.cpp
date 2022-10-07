#include "GLTFImporter.h"

#include "tiny_gltf.h"

namespace Ciri
{
    S<SceneNode> GLTFImporter::Import(Scene *scene, const char *filepath)
    {
        S<SceneNode> container = scene->AddContainer();

        tinygltf::TinyGLTF loader;
        std::string err;
        std::string warn;

        tinygltf::Model model;
        bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, filepath);
        if (!ret)
        {
            CIRI_ERROR(err);
            CIRI_ASSERT(false, "Failed to load GLTF ascii file!");
        }
        CIRI_LOG("Successfully loaded {}", filepath);

        if (!warn.empty())
        {
            CIRI_WARN(warn);
        }

        if (model.defaultScene == -1)
        {
            CIRI_ASSERT(false, "Failed to load GLTF ascii file - Missing default scene?");
        }

        const tinygltf::Scene &default_scene = model.scenes[model.defaultScene];
        std::stack<int> node_stack;
        for (int n : default_scene.nodes)
        {
            if ((n >= 0) && (n < model.nodes.size()))
            {
                node_stack.push(n);
            }
        }

        /* Traverse node tree of the default scene. */
        while (!node_stack.empty())
        {
            int n = node_stack.top();
            node_stack.pop();

            /* Retrieve Node. */
            const tinygltf::Node &node = model.nodes[n];
            CIRI_LOG("Node - {}", node.name);

            /* Retrieve Mesh if it exists. */
            if ((node.mesh >= 0) && (node.mesh < model.meshes.size()))
            {
                const tinygltf::Mesh &mesh = model.meshes[node.mesh];
                CIRI_LOG("Mesh - {}", mesh.name);
                CIRI_LOG("Primitives Count - {}", mesh.primitives.size());

                /* Loop over mesh primitives. */
                // int i = 0;
                for (const tinygltf::Primitive &primitive : mesh.primitives)
                {
                    // i++;
                    // if (i != 3)
                    // {
                    //     continue;
                    // }
                    std::vector<glm::vec3> positionData;
                    std::vector<glm::vec3> normalData;
                    std::vector<glm::vec2> texCoordData;

                    /* Loop over primitive attributes. */
                    for (auto &attrib : primitive.attributes)
                    {
                        CIRI_LOG("Attribute: {}", attrib.first);
                        const tinygltf::Accessor &accessor = model.accessors[attrib.second];

                        const tinygltf::BufferView &buffer_view = model.bufferViews[accessor.bufferView];
                        const tinygltf::Buffer &buffer = model.buffers[buffer_view.buffer];
                        CIRI_LOG("Accessor Count: {}", accessor.count);
                        CIRI_LOG("Accessor Component Type: {}", accessor.componentType);
                        CIRI_LOG("Accessor Type: {}", accessor.type);
                        CIRI_LOG("Accessor Byte Offset: {}", accessor.byteOffset);
                        CIRI_LOG("Accessor Byte Stride: {}", accessor.ByteStride(buffer_view));
                        CIRI_LOG("BufferView Target: {}", buffer_view.target);
                        CIRI_LOG("BufferView Buffer Byte Count: {}", buffer.data.size());
                        CIRI_LOG("BufferView Byte Offset: {}", buffer_view.byteOffset);
                        CIRI_LOG("BufferView Byte Length: {}", buffer_view.byteLength);

                        if (attrib.first.compare("POSITION") == 0)
                        {
                            if (accessor.componentType == GL_FLOAT && accessor.type == TINYGLTF_TYPE_VEC3)
                            {
                                for (size_t i = buffer_view.byteOffset; i < buffer_view.byteOffset + buffer_view.byteLength; i += accessor.ByteStride(buffer_view))
                                {
                                    float p1 = *(float *)&buffer.data[i];
                                    float p2 = *(float *)&buffer.data[i + 4];
                                    float p3 = *(float *)&buffer.data[i + 8];

                                    positionData.push_back({p1, p2, p3});
                                }
                            }
                            else
                            {
                                CIRI_ASSERT(false, "GLTF primitive has wrong position data type for Ciri mesh");
                            }
                        }

                        if (attrib.first.compare("NORMAL") == 0)
                        {
                            if (accessor.componentType == GL_FLOAT && accessor.type == TINYGLTF_TYPE_VEC3)
                            {
                                for (size_t i = buffer_view.byteOffset; i < buffer_view.byteOffset + buffer_view.byteLength; i += accessor.ByteStride(buffer_view))
                                {
                                    float n1 = *(float *)&buffer.data[i];
                                    float n2 = *(float *)&buffer.data[i + 4];
                                    float n3 = *(float *)&buffer.data[i + 8];

                                    normalData.push_back({n1, n2, n3});
                                }
                            }
                            else
                            {
                                CIRI_ASSERT(false, "GLTF primitive has wrong normal data type for Ciri mesh");
                            }
                        }

                        if (attrib.first.compare("TEXCOORD_0") == 0)
                        {
                            if (accessor.componentType == GL_FLOAT && accessor.type == TINYGLTF_TYPE_VEC2)
                            {
                                for (size_t i = buffer_view.byteOffset; i < buffer_view.byteOffset + buffer_view.byteLength; i += accessor.ByteStride(buffer_view))
                                {
                                    float t1 = *(float *)&buffer.data[i];
                                    float t2 = *(float *)&buffer.data[i + 4];

                                    texCoordData.push_back({t1, t2});
                                }
                            }
                            else
                            {
                                CIRI_ASSERT(false, "GLTF primitive has wrong texcoord data type for Ciri mesh");
                            }
                        }
                    }

                    /* Retrieve primitive indices. */
                    std::vector<uint16_t> indexData;
                    tinygltf::Accessor index_accessor = model.accessors[primitive.indices];
                    const tinygltf::BufferView &buffer_view = model.bufferViews[index_accessor.bufferView];
                    const tinygltf::Buffer &buffer = model.buffers[buffer_view.buffer];
                    CIRI_LOG("Primitive Mode: {}", primitive.mode);
                    CIRI_LOG("Index Accessor Count: {}", index_accessor.count);
                    CIRI_LOG("Index Accessor Component Type: {}", index_accessor.componentType);
                    CIRI_LOG("Index Accessor Type: {}", index_accessor.type);
                    CIRI_LOG("Index Accessor Byte Offset: {}", index_accessor.byteOffset);
                    CIRI_LOG("Index Accessor Byte Stride: {}", index_accessor.ByteStride(buffer_view));
                    CIRI_LOG("BufferView Target: {}", buffer_view.target);
                    CIRI_LOG("BufferView Buffer Byte Count: {}", buffer.data.size());
                    CIRI_LOG("BufferView Byte Offset: {}", buffer_view.byteOffset);
                    CIRI_LOG("BufferView Byte Length: {}", buffer_view.byteLength);

                    if (index_accessor.componentType == GL_UNSIGNED_SHORT && index_accessor.type == TINYGLTF_TYPE_SCALAR)
                    {
                        for (size_t i = index_accessor.byteOffset; i < index_accessor.byteOffset + (index_accessor.count * index_accessor.ByteStride(buffer_view)); i += index_accessor.ByteStride(buffer_view))
                        {
                            uint16_t t1 = *(uint16_t *)&buffer.data[i];
                            indexData.push_back(t1);
                        }
                    }
                    else
                    {
                        CIRI_ASSERT(false, "GLTF primitive has wrong index data type for Ciri mesh");
                    }

                    S<Mesh> mesh = CreateS<Mesh>(positionData, normalData, texCoordData, indexData);
                    mesh->Construct();

                    S<SceneNode> node = CreateS<SceneNode>();
                    node->NodeMesh = mesh;
                    node->NodeMaterial = scene->GetDefaultMaterial();
                    container->AddChild(node);
                    // break;
                }
            }

            /* Process child nodes of current node. */
            for (int c : node.children)
            {
                if ((c >= 0) && (c < model.nodes.size()))
                {
                    node_stack.push(c);
                }
            }
        }

        return container;
    }
}