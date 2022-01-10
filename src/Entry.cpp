#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>

#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "tiny_obj_loader.h"

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

// Window
const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 900;

// Camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool firstMouse = true;
float lastX = (float)WINDOW_WIDTH / 2;
float lastY = (float)WINDOW_HEIGHT / 2;
float yaw = -90.0f;
float pitch = 0.0f;

float cameraSpeedHigh = 10.0f;
float cameraSpeedLow = 5.0f;
float renderDistance = 10000.0f;

bool debug = false;
bool mipmap = true;
bool normal = false;

struct Mesh
{
    GLuint vaoID = 0;
    GLuint vboID = 0;
    GLuint textureID = 0;
    int triCount = 0;
};

struct RenderData
{
    std::vector<Mesh> meshes;
    std::map<std::string, GLuint> textureMap;
    int totalTriCount = 0;
};

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
}

void poll_input(GLFWwindow *window)
{
    if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
    {
        return;
    }

    float cameraSpeed = cameraSpeedLow;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        cameraSpeed = cameraSpeedHigh * deltaTime;
    }
    else
    {
        cameraSpeed = cameraSpeedLow * deltaTime;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPos += cameraSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPos -= cameraSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
    {
        return;
    }

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        firstMouse = true;
    }
}

void GLAPIENTRY MessageCallback(GLenum source,
                                GLenum type,
                                GLuint id,
                                GLenum severity,
                                GLsizei length,
                                const GLchar *message,
                                const void *userParam)
{
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type, severity, message);
}

void ErrorHandleShader(GLuint &shader)
{
    GLint result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        std::cout << "Shader " << shader << " failed:" << std::endl;
        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        GLchar *log = new GLchar[length];
        glGetShaderInfoLog(shader, length, &length, log);
        std::cout << log << std::endl;
        delete log;
        glDeleteShader(shader);
    }
}

std::string ParseShaderFromFile(const char *filename)
{
    std::ifstream in(filename, std::ios::in);
    if (in)
    {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return contents;
    }
    else
    {
        std::cout << "Error parsing shader!" << std::endl;
        return NULL;
    }
}

void loadOBJ(RenderData *renderData)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;
    std::string path = "resources/mesh/sponza/";
    std::string objFile = "sponza.obj";
    std::string obj = path + objFile;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, obj.c_str(), path.c_str());
    if (!ret)
    {
        std::cout << "Failed to load object!" << std::endl;
        std::cout << err << std::endl;
    }

    printf("Successfully loaded %s - Shapes: %d | Materials: %d\n", objFile.c_str(), shapes.size(), materials.size());
    if (!warn.empty())
    {
        std::cout << warn << std::endl;
    }
    for (size_t i = 0; i < shapes.size(); i++)
    {
        printf("Shape %d: %s\n", i, shapes[i].name.c_str());
    }
    printf("\n");
    materials.push_back(tinyobj::material_t());
    for (size_t i = 0; i < materials.size(); i++)
    {
        printf("Material %d: %s\n", i, materials[i].name.c_str());
        if (materials[i].diffuse_texname.length() > 0)
        {
            printf("    Diffuse Texture: %s\n", materials[i].diffuse_texname.c_str());
        }
    }

    for (size_t m = 0; m < materials.size(); m++)
    {
        if (materials[m].diffuse_texname.length() > 0)
        {
            // Only load the texture if it is not already loaded
            if (renderData->textureMap.find(materials[m].diffuse_texname) == renderData->textureMap.end())
            {
                GLuint textureID;
                int w, h;
                int nrChannels;

                std::string texture_filename = path + materials[m].diffuse_texname;
                std::replace(texture_filename.begin(), texture_filename.end(), '\\', '/');
                unsigned char *image = stbi_load(texture_filename.c_str(), &w, &h, &nrChannels, STBI_default);
                if (!image)
                {
                    std::cerr << "Unable to load texture: " << std::endl;
                }
                std::cout << "Loaded texture: " << texture_filename << ", w = " << w << ", h = " << h << ", nrChannels = " << nrChannels << std::endl;

                glGenTextures(1, &textureID);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, textureID);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                if (mipmap)
                {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                }
                else
                {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                }
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                if (nrChannels == 3)
                {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
                    if (mipmap)
                        glGenerateMipmap(GL_TEXTURE_2D);
                }
                else if (nrChannels == 4)
                {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
                    if (mipmap)
                        glGenerateMipmap(GL_TEXTURE_2D);
                }
                else
                {
                    std::cout << "Invalid number of channels for texture " << std::endl;
                }
                glBindTexture(GL_TEXTURE_2D, textureID);

                stbi_image_free(image);
                renderData->textureMap.insert(std::make_pair(materials[m].diffuse_texname, textureID));
            }
        }
        else
        {
            printf("No diffuse texture for material: %s\n", materials[m].name.c_str());
        }
    }

    for (size_t s = 0; s < shapes.size(); s++)
    {
        int offset = 0;
        Mesh mesh;
        std::vector<tinyobj::real_t> data;

        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            int currentMaterialID = shapes[s].mesh.material_ids[f];
            if ((currentMaterialID < 0) || (currentMaterialID >= static_cast<int>(materials.size())))
            {
                currentMaterialID = materials.size() - 1;
                std::cout << "Invalid current material id for mesh" << std::endl;
            }
            // color = diffuse * ambient
            float cx = materials[currentMaterialID].diffuse[0] * materials[currentMaterialID].ambient[0];
            float cy = materials[currentMaterialID].diffuse[1] * materials[currentMaterialID].ambient[1];
            float cz = materials[currentMaterialID].diffuse[2] * materials[currentMaterialID].ambient[2];

            for (int v = 0; v < 3; v++)
            {
                tinyobj::index_t index = shapes[s].mesh.indices[offset + v];

                // Vertices
                tinyobj::real_t vx = attrib.vertices[3 * index.vertex_index + 0];
                tinyobj::real_t vy = attrib.vertices[3 * index.vertex_index + 1];
                tinyobj::real_t vz = attrib.vertices[3 * index.vertex_index + 2];

                // Normals
                tinyobj::real_t nx;
                tinyobj::real_t ny;
                tinyobj::real_t nz;
                if (attrib.normals.size() > 0)
                {
                    nx = attrib.normals[3 * index.normal_index + 0];
                    ny = attrib.normals[3 * index.normal_index + 1];
                    nz = attrib.normals[3 * index.normal_index + 2];
                }
                else
                {
                    nx = 0.0f;
                    ny = 0.0f;
                    nz = 0.0f;
                }

                // Texture Coordinates
                tinyobj::real_t tx;
                tinyobj::real_t ty;
                size_t tindex_x = 2 * index.texcoord_index + 0;
                size_t tindex_y = 2 * index.texcoord_index + 1;
                if (attrib.texcoords.size() > 0 && tindex_x < attrib.texcoords.size() && tindex_y < attrib.texcoords.size())
                {
                    tx = attrib.texcoords[tindex_x];
                    ty = attrib.texcoords[tindex_y];
                }
                else
                {
                    tx = 0.0f;
                    ty = 0.0f;
                }

                data.push_back(vx);
                data.push_back(vy);
                data.push_back(vz);
                data.push_back(nx);
                data.push_back(ny);
                data.push_back(nz);
                data.push_back(cx);
                data.push_back(cy);
                data.push_back(cz);
                data.push_back(tx);
                data.push_back(ty);
            }

            offset += 3;
        }

        // Texture (no per face textures yet)
        int materialID = shapes[s].mesh.material_ids[0];
        if ((materialID < 0) || (materialID >= static_cast<int>(materials.size())))
        {
            materialID = materials.size() - 1;
            std::cout << "Invalid material id for mesh" << std::endl;
        }

        std::string diffuse_texname = materials[materialID].diffuse_texname;
        if (renderData->textureMap.find(diffuse_texname) != renderData->textureMap.end())
        {
            mesh.textureID = renderData->textureMap[diffuse_texname];
        }

        // Vertex Data
        if (data.size() > 0)
        {
            glGenVertexArrays(1, &mesh.vaoID);
            glBindVertexArray(mesh.vaoID);

            glGenBuffers(1, &mesh.vboID);
            glBindBuffer(GL_ARRAY_BUFFER, mesh.vboID);
            glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), 0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)(6 * sizeof(float)));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)(9 * sizeof(float)));
            glEnableVertexAttribArray(3);

            mesh.triCount = data.size() / (3 + 3 + 3 + 2) / 3; // (3 pos, 3 normal, 3 color, 2 texcoord)
            renderData->totalTriCount += mesh.triCount;
        }

        renderData->meshes.push_back(mesh);
    }
}

int main()
{
    GLFWwindow *window;

    // GLFW and Glad
    if (!glfwInit())
    {
        std::cout << "GLFW Initialization Failed" << std::endl;
        return 0;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OBJ Loading", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        std::cout << "GLFW Window Creation Failed" << std::endl;
        return 0;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    if (!status)
    {
        std::cout << "GLAD Initialization Failed" << std::endl;
        return 0;
    }

    if (debug)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(MessageCallback, 0);
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // ImGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460 core");

    stbi_set_flip_vertically_on_load(true);
    glEnable(GL_DEPTH_TEST);

    // .obj loading
    RenderData renderData;
    loadOBJ(&renderData);

    // Shaders
    Ciri::ShaderLibrary *shaderLibrary = new Ciri::ShaderLibrary();

    while (!glfwWindowShouldClose(window))
    {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ImGui::ShowDemoWindow();

        ImGui::Begin("Test Window");
        ImGui::Text("Version: OpenGL %s", glGetString(GL_VERSION));
        ImGui::Text("Vendor: %s", glGetString(GL_VENDOR));
        ImGui::Text("Renderer: %s", glGetString(GL_RENDERER));
        ImGui::Separator();
        ImGui::Text("Mesh Count: %d", renderData.meshes.size());
        ImGui::Text("Triangles: %d", renderData.totalTriCount);
        ImGui::Text("Frame Time: %fms", deltaTime * 1000);
        ImGui::Text("FPS: %f", 1.0f / deltaTime);
        ImGui::Separator();
        ImGui::SliderFloat("Camera Speed High", &cameraSpeedHigh, 1.0f, 1000.0f);
        ImGui::SliderFloat("Camera Speed Low", &cameraSpeedLow, 1.0f, 500.0f);
        ImGui::SliderFloat("Render Distance", &renderDistance, 10.0f, 10000.0f);
        ImGui::Checkbox("Normals", &normal);
        ImGui::End();

        ImGui::Render();

        poll_input(window);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(1.0f, 1.0f, 0.0f, 1.0f);

        // Projection
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, renderDistance);
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 model = glm::mat4(1.0f);
        // model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        glm::mat4 mvp = projection * view * model;

        for (Mesh &m : renderData.meshes)
        {
            if (normal)
            {
                shaderLibrary->BindShader(Ciri::ShaderType::FLAT_NORMAL);
            }
            else if (m.textureID == 0)
            {
                shaderLibrary->BindShader(Ciri::ShaderType::DIFFUSE);
            }
            else
            {
                shaderLibrary->BindShader(Ciri::ShaderType::TEXTURE);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, m.textureID);
            }

            uint32_t currentProgramID = shaderLibrary->GetShader()->program_id;
            GLuint mvp_location = glGetUniformLocation(currentProgramID, "u_MVP");
            GLuint texture_location = glGetUniformLocation(currentProgramID, "u_DiffuseTexture");
            glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));
            glUniform1i(texture_location, 0);

            glBindVertexArray(m.vaoID);

            glDrawArrays(GL_TRIANGLES, 0, 3 * m.triCount);

            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
            shaderLibrary->BindShader(Ciri::ShaderType::NONE);
        }

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}