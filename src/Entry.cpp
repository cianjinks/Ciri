#include <iostream>
#include <fstream>

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

// Window
const int WINDOW_WIDTH = 1080;
const int WINDOW_HEIGHT = 720;

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

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
}

void poll_input(GLFWwindow *window)
{
    if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
    {
        return;
    }

    float cameraSpeed = 5.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        cameraSpeed = 10.0f * deltaTime;
    }
    else
    {
        cameraSpeed = 5.0f * deltaTime;
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

void loadOBJ(std::vector<tinyobj::real_t> &data)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;
    std::string path = "resources/mesh/cube/";
    std::string objFile = "cube.obj";
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
    }

    for (size_t s = 0; s < shapes.size(); s++)
    {
        int offset = 0;

        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
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
                if (attrib.texcoords.size() > 0)
                {
                    tx = attrib.texcoords[2 * index.texcoord_index + 0];
                    ty = attrib.texcoords[2 * index.texcoord_index + 1];
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
                data.push_back(tx);
                data.push_back(ty);
            }

            offset += 3;
        }
    }

#if 0
    for (size_t s = 0; s < shapes.size(); s++)
    {
        for (size_t f = 0; f < shapes[s].mesh.indices.size(); f += 3)
        {
            tinyobj::index_t idx0 = shapes[s].mesh.indices[f + 0];
            tinyobj::index_t idx1 = shapes[s].mesh.indices[f + 1];
            tinyobj::index_t idx2 = shapes[s].mesh.indices[f + 2];

            float v[3][3];
            for (int k = 0; k < 3; k++)
            {
                int f0 = idx0.vertex_index;
                int f1 = idx1.vertex_index;
                int f2 = idx2.vertex_index;
                assert(f0 >= 0);
                assert(f1 >= 0);
                assert(f2 >= 0);

                v[0][k] = attrib.vertices[3 * f0 + k];
                v[1][k] = attrib.vertices[3 * f1 + k];
                v[2][k] = attrib.vertices[3 * f2 + k];
            }

            float n[3][3];
            {
                bool invalid_normal_index = false;
                if (attrib.normals.size() > 0)
                {
                    int nf0 = idx0.normal_index;
                    int nf1 = idx1.normal_index;
                    int nf2 = idx2.normal_index;

                    if ((nf0 < 0) || (nf1 < 0) || (nf2 < 0))
                    {
                        invalid_normal_index = true;
                    }
                    else
                    {
                        for (int k = 0; k < 3; k++)
                        {
                            assert(size_t(3 * nf0 + k) < attrib.normals.size());
                            assert(size_t(3 * nf1 + k) < attrib.normals.size());
                            assert(size_t(3 * nf2 + k) < attrib.normals.size());
                            n[0][k] = attrib.normals[3 * nf0 + k];
                            n[1][k] = attrib.normals[3 * nf1 + k];
                            n[2][k] = attrib.normals[3 * nf2 + k];
                        }
                    }
                }
                else
                {
                    invalid_normal_index = true;
                }

                if (invalid_normal_index)
                {
                    n[1][0] = 0.0f;
                    n[1][1] = 0.0f;
                    n[1][2] = 0.0f;
                    n[2][0] = 0.0f;
                    n[2][1] = 0.0f;
                    n[2][2] = 0.0f;
                }
            }

            for (int k = 0; k < 3; k++)
            {
                data.push_back(v[k][0]);
                data.push_back(v[k][1]);
                data.push_back(v[k][2]);
                data.push_back(n[k][0]);
                data.push_back(n[k][1]);
                data.push_back(n[k][2]);
            }
        }
    }
#endif
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

    // Test .obj loading
    std::vector<tinyobj::real_t> data;
    loadOBJ(data);

#if 0
    // Test Cube
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,

        -0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,

        -0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f};

    unsigned int indices[] = {0, 1, 5, 5, 1, 6, 1, 2, 6, 6, 2, 7, 2, 3, 7, 7, 3, 8, 3, 4, 8, 8, 4, 9, 10, 11, 0, 0, 11, 1, 5, 6, 12, 12, 6, 13};
#endif

    std::string vertexShader = ParseShaderFromFile("resources/shader/test.vert");
    std::string fragmentShader = ParseShaderFromFile("resources/shader/test.frag");
    const char *vertexSrc = vertexShader.c_str();
    const char *fragSrc = fragmentShader.c_str();

    glEnable(GL_DEPTH_TEST);

    GLuint vaoID;
    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    GLuint vboID;
    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // GLuint iboID;
    // glGenBuffers(1, &iboID);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    GLuint vertShaderObj = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShaderObj, 1, &vertexSrc, NULL);
    glCompileShader(vertShaderObj);

    ErrorHandleShader(vertShaderObj);

    GLuint fragShaderObj = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShaderObj, 1, &fragSrc, NULL);
    glCompileShader(fragShaderObj);

    ErrorHandleShader(fragShaderObj);

    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertShaderObj);
    glAttachShader(programID, fragShaderObj);
    glLinkProgram(programID);
    glValidateProgram(programID);

    GLuint mvp_location = glGetUniformLocation(programID, "u_MVP");

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
        ImGui::Text("Triangles: %d", data.size() / (3 + 3));
        ImGui::Text("Frame Time: %fms", deltaTime * 1000);
        ImGui::Text("FPS: %f", 1.0f / deltaTime);
        ImGui::End();

        ImGui::Render();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(1.0f, 1.0f, 0.0f, 1.0f);

        poll_input(window);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        // Projection
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 model = glm::mat4(1.0f);
        // model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        glm::mat4 mvp = projection * view * model;

        glUseProgram(programID);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));
        // glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glDrawArrays(GL_TRIANGLES, 0, data.size() / (3 + 3 + 2)); // (3 pos, 3 normal, 2 texcoord)

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}