#include "Scene.h"

#include "tiny_obj_loader.h"
#include <cstring>

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

    SceneNode *Scene::AddMesh(const char *name, Mesh *mesh)
    {
        SceneNode *node = new SceneNode();

        // Check for duplicates
        if (m_NameLookup.find(name) != m_NameLookup.end())
        {
            std::cout << "Failed to add mesh to scene with duplicate name: " << name << std::endl;
            return node;
        }
        m_NameLookup.insert(name);

        node->Name = name;
        node->NodeMesh = mesh;
        m_Root->AddChild(node);

        m_TotalTriCount += mesh->TriCount;
        m_MeshCount++;

        return node;
    }

    SceneNode *Scene::AddContainer(const char *name)
    {
        SceneNode *node = new SceneNode();
        // Check for duplicates
        if (m_NameLookup.find(name) != m_NameLookup.end())
        {
            std::cout << "Failed to add container to scene with duplicate name: " << name << std::endl;
            return node;
        }
        m_NameLookup.insert(name);

        node->Name = name;
        m_Root->AddChild(node);

        return node;
    }

    SceneNode *Scene::LoadModel(const char *name, const char *filepath, const char *materialpath)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;

        std::string warn;
        std::string err;

        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath, materialpath);
        if (!ret)
        {
            std::cout << "Failed to load object!" << std::endl;
            std::cout << err << std::endl;
        }

        printf("Successfully loaded %s - Shapes: %d | Materials: %d\n", filepath, shapes.size(), materials.size());
        if (!warn.empty())
        {
            std::cout << warn << std::endl;
        }

        for (size_t i = 0; i < shapes.size(); i++)
        {
            printf("Shape %d: %s\n", i, shapes[i].name.c_str());
        }
        printf("\n");

        SceneNode *container = AddContainer(name);

        std::vector<glm::vec3> positionData;
        std::vector<glm::vec3> normalData;
        std::vector<glm::vec3> colorData;
        std::vector<glm::vec2> texCoordData;

        for (size_t s = 0; s < shapes.size(); s++)
        {
            int offset = 0;
            for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
            {
                // color = diffuse * ambient
                glm::vec3 color;

                int currentMaterialID = shapes[s].mesh.material_ids[f];
                if ((currentMaterialID < 0) || (currentMaterialID >= static_cast<int>(materials.size())))
                {
                    // currentMaterialID = materials.size() - 1;
                    // std::cout << "Invalid current material id for mesh" << std::endl;
                    color = glm::vec3(0.0f, 0.0f, 0.0f);
                }
                else
                {
                    color.x = materials[currentMaterialID].diffuse[0] * materials[currentMaterialID].ambient[0];
                    color.y = materials[currentMaterialID].diffuse[1] * materials[currentMaterialID].ambient[1];
                    color.z = materials[currentMaterialID].diffuse[2] * materials[currentMaterialID].ambient[2];
                }

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
                    colorData.push_back(color);
                    texCoordData.push_back(texCoord);
                }

                offset += 3;
            }

            // TODO: This is a memory leak, mesh pointer is lost
            Mesh *mesh = new Mesh(positionData, normalData, colorData, texCoordData);
            mesh->Construct();

            SceneNode *node = new SceneNode();
            node->Name = shapes[s].name;
            node->NodeMesh = mesh;

            container->AddChild(node);

            positionData.clear();
            normalData.clear();
            colorData.clear();
            texCoordData.clear();
        }

        return container;
    }
}