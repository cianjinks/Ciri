#include "Scene.h"

#include "tiny_obj_loader.h"
#include <cstring>
#include <algorithm>

namespace Ciri
{
    void SceneNode::AddChild(SceneNode *child)
    {
        m_Children.push_back(child);
    }

    Scene::Scene(const char *name)
        : Name(name)
    {
        m_Root = new SceneNode();
        m_Root->Name = std::string(name);

        // Default Material
        m_DefaultMaterial = MatLib.CreateMaterial("default", glm::vec3(1.0f, 0.0f, 1.0f), {false});
    }

    Scene::~Scene()
    {
        // Destroy scene node tree
        DestroyTree(m_Root);
        delete m_Root;
    }

    void Scene::DestroyTree(SceneNode *root)
    {
        for (SceneNode *node : root->m_Children)
        {
            DestroyTree(node);
        }
        root->m_Children.clear(); // Destroys all elements in vector?
    }

    SceneNode *Scene::AddMesh(const char *name, Mesh *mesh, Material *material)
    {
        SceneNode *node = new SceneNode();

        node->Name = name;
        node->NodeMesh = mesh;
        node->NodeMaterial = material ? material : m_DefaultMaterial;
        m_Root->AddChild(node);

        m_TotalTriCount += mesh->TriCount;
        m_MeshCount++;

        return node;
    }

    SceneNode *Scene::AddContainer(const char *name)
    {
        SceneNode *node = new SceneNode();

        node->Name = name;
        m_Root->AddChild(node);

        return node;
    }

    SceneNode *Scene::LoadModel(const char *name, const char *filepath, Material *custom_material)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;

        std::string warn;
        std::string err;

        // materialpath = folder containing .obj
        int i = std::strlen(filepath) - 1;
        while (filepath[i] != '/' && i >= 0)
        {
            i--;
        }
        char materialpath[i + 2];
        for (int j = 0; j <= i; j++)
        {
            materialpath[j] = filepath[j];
        }
        materialpath[i + 1] = '\0';

        // ^TODO: Remove, find better solution (`find_last_of` ?)

        // Load OBJ
        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath, materialpath);
        if (!ret)
        {
            std::cout << "Failed to load object!" << std::endl;
            std::cout << err << std::endl;
        }
        printf("Successfully loaded %s - Shapes: %d | Materials: %d\n", filepath, shapes.size(), materials.size());

        // Warnings
        if (!warn.empty())
        {
            std::cout << warn << std::endl;
        }

        // List shapes
        for (size_t i = 0; i < shapes.size(); i++)
        {
            printf("Shape %d: %s\n", i, shapes[i].name.c_str());
        }
        printf("\n");

        // List materials
        materials.push_back(tinyobj::material_t());
        for (size_t i = 0; i < materials.size(); i++)
        {
            printf("Material %d: %s\n", i, materials[i].name.c_str());
            if (materials[i].diffuse_texname.length() > 0)
            {
                printf("    Diffuse Texture: %s\n", materials[i].diffuse_texname.c_str());
            }
        }

        // Add materials to material library
        for (size_t m = 0; m < materials.size(); m++)
        {
            if (materials[m].diffuse_texname.length() > 0)
            {
                std::string texture_name = materials[m].diffuse_texname;
                std::replace(texture_name.begin(), texture_name.end(), '\\', '/');
                std::string full_material_path = std::string(materialpath) + texture_name;
                // TODO: The c_str value is inserted into a map so becomes invalid memory when string goes out of scope :(
                MatLib.CreateMaterial(materials[m].name.c_str(), glm::vec3(1.0f), {true}, full_material_path.c_str());
            }
        }

        SceneNode *container = AddContainer(name);

        std::vector<glm::vec3> positionData;
        std::vector<glm::vec3> normalData;
        std::vector<glm::vec2> texCoordData;

        for (size_t s = 0; s < shapes.size(); s++)
        {
            int offset = 0;
            for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
            {
                for (int v = 0; v < 3; v++)
                {
                    tinyobj::index_t index = shapes[s].mesh.indices[offset + v];

                    // Vertices
                    glm::vec3 position;
                    position.x = attrib.vertices[3 * index.vertex_index + 0];
                    position.y = attrib.vertices[3 * index.vertex_index + 1];
                    position.z = attrib.vertices[3 * index.vertex_index + 2];

                    // Normals
                    glm::vec3 normal;
                    if (attrib.normals.size() > 0)
                    {
                        normal.x = attrib.normals[3 * index.normal_index + 0];
                        normal.y = attrib.normals[3 * index.normal_index + 1];
                        normal.z = attrib.normals[3 * index.normal_index + 2];
                    }
                    else
                    {
                        normal.x = 0.0f;
                        normal.y = 0.0f;
                        normal.z = 0.0f;
                    }

                    // Texture Coordinates
                    glm::vec2 texCoord;
                    size_t tindex_x = 2 * index.texcoord_index + 0;
                    size_t tindex_y = 2 * index.texcoord_index + 1;
                    if (attrib.texcoords.size() > 0 && tindex_x < attrib.texcoords.size() && tindex_y < attrib.texcoords.size())
                    {
                        texCoord.x = attrib.texcoords[tindex_x];
                        texCoord.y = attrib.texcoords[tindex_y];
                    }
                    else
                    {
                        texCoord.x = 0.0f;
                        texCoord.y = 0.0f;
                    }

                    positionData.push_back(position);
                    normalData.push_back(normal);
                    texCoordData.push_back(texCoord);
                }

                offset += 3;
            }

            // TODO: This is a memory leak, mesh pointer is lost
            Mesh *mesh = new Mesh(positionData, normalData, texCoordData);
            mesh->Construct();

            SceneNode *node = new SceneNode();
            node->Name = shapes[s].name;
            node->NodeMesh = mesh;

            // Set material
            if (custom_material)
            {
                node->NodeMaterial = custom_material;
            }
            else if (!shapes[s].mesh.material_ids.empty())
            {
                int materialID = shapes[s].mesh.material_ids[0];
                if ((materialID < 0) || (materialID >= static_cast<int>(materials.size())))
                {
                    materialID = materials.size() - 1;
                    std::cout << "Invalid material id for mesh: " << shapes[s].name << std::endl;
                }

                Material *material = MatLib.GetMaterial(materials[materialID].name.c_str());
                if (material)
                {
                    node->NodeMaterial = material;
                }
            }

            if (!node->NodeMaterial)
            {
                node->NodeMaterial = m_DefaultMaterial;
            }
            //

            container->AddChild(node);

            m_TotalTriCount += mesh->TriCount;
            m_MeshCount++;

            positionData.clear();
            normalData.clear();
            texCoordData.clear();
        }

        return container;
    }
}