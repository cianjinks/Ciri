#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
#include <cstring>

#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "tiny_obj_loader.h"

#include "IconsForkAwesome.h"

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Render/Shader.h"
#include "Render/Material.h"
#include "Scene/Scene.h"
#include "Mesh/Primitive.h"

// Window
const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 900;
bool mouseCaptured = false;

// Camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool firstMouse = true;
float lastX = (float)WINDOW_WIDTH / 2;
float lastY = (float)WINDOW_HEIGHT / 2;
float yaw = -90.0f;
float pitch = 0.0f;

float cameraSpeedHigh = 30.0f;
float cameraSpeedLow = 15.0f;
float renderDistance = 10000.0f;

bool debug = false;
bool mipmap = true;

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
        mouseCaptured = true;
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        firstMouse = true;
        mouseCaptured = false;
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

static Ciri::SceneNode *selected_node = nullptr;
void SceneUI(Ciri::SceneNode *root, int ptr_id)
{
    ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

    // Leaf
    bool leaf = root->m_Children.empty();
    if (leaf)
    {
        node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    // This node is selected
    if (selected_node == root)
    {
        node_flags |= ImGuiTreeNodeFlags_Selected;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 2.0f));
    // TODO: Blank root name is buggy and icon handling is rough
    bool node_open = ImGui::TreeNodeEx((void *)(intptr_t)ptr_id, node_flags, "%s %s", root->NodeMesh ? ICON_FK_CUBE "" : ICON_FK_SQUARE_O "", root->Name.c_str());

    // Update selected node on click but not expand toggle click
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
    {
        selected_node = root;
    }

    if (node_open && !leaf)
    {
        for (Ciri::SceneNode *node : root->m_Children)
        {
            SceneUI(node, ptr_id++);
        }

        ImGui::TreePop();
    }
    ImGui::PopStyleVar();
}

void RenderScene(Ciri::SceneNode *root, Ciri::ShaderType &selected, Ciri::ShaderLibrary *library, glm::mat4 &proj, glm::mat4 &view, glm::mat4 model)
{
    for (Ciri::SceneNode *node : root->m_Children)
    {
        glm::mat4 accumulateModel = model;
        accumulateModel = glm::translate(accumulateModel, node->Position);
        accumulateModel = glm::scale(accumulateModel, node->Scale); // Accumulative scaling doesnt work correctly

        // No mesh means this is just a container
        if (node->NodeMesh)
        {
            glm::mat4 mvp = proj * view * accumulateModel;

            library->BindShader(selected);
            library->SetMat4f("u_MVP", glm::value_ptr(mvp));

            Ciri::Material *material = node->NodeMaterial;
            library->SetInt1i("u_BaseColorTexture", 0);
            library->SetVec3f("u_BaseColor", material->baseColor);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, material->baseColorTextureID);
            /**
            library->SetInt1i("u_NormalTexture", 1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, material->normalTextureID);
            library->SetInt1i("u_MetallicTexture", 2);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, material->metallicTextureID);
            library->SetInt1i("u_RoughnessTexture", 3);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, material->roughnessTextureID);
            library->SetInt1i("u_EmissiveTexture", 4);
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, material->emissiveTextureID);
            library->SetInt1i("u_OcclusionTexture", 5);
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_2D, material->occlusionTextureID);
            **/

            Ciri::Mesh *mesh = node->NodeMesh;
            glBindVertexArray(mesh->m_VAO);

            glDrawArrays(GL_TRIANGLES, 0, 3 * mesh->TriCount);

            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
            library->BindShader(Ciri::ShaderType::NONE);
        }

        RenderScene(node, selected, library, proj, view, accumulateModel);
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

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Ciri", nullptr, nullptr);
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

    // Merge in icons from Fork Awesome
    io.Fonts->AddFontDefault();
    static const ImWchar icons_ranges[] = {ICON_MIN_FK, ICON_MAX_FK, 0};
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.GlyphOffset = {0.f, 2.f};
    icons_config.GlyphMinAdvanceX = 20.0f; // necessary?
    io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FK, 16.0f, &icons_config, icons_ranges);

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460 core");

    glEnable(GL_DEPTH_TEST);

    // Shaders
    Ciri::ShaderLibrary *shaderLibrary = new Ciri::ShaderLibrary();
    auto &shaders = shaderLibrary->GetShaderList();
    Ciri::ShaderType selected = Ciri::ShaderType::FLAT_NORMAL;

    // Scene
    Ciri::Scene *mainScene = new Ciri::Scene("Main Scene");
    Ciri::Mesh *cube1 = new Ciri::Cube();
    Ciri::Mesh *cube2 = new Ciri::Cube();
    Ciri::Mesh *cube3 = new Ciri::Cube();
    cube1->Construct();
    cube2->Construct();
    cube3->Construct();
    Ciri::SceneNode *cube1Node = mainScene->AddMesh("cube1", cube1);
    Ciri::SceneNode *cube2Node = mainScene->AddMesh("cube2", cube2);
    Ciri::SceneNode *cube3Node = mainScene->AddMesh("cube3", cube3);
    cube1Node->Position = glm::vec3(-3.0f, 10.0f, 0.0f);
    cube1Node->Scale = glm::vec3(0.5f);
    cube2Node->Position = glm::vec3(0.0f, 10.0f, 0.0f);
    cube2Node->Scale = glm::vec3(0.75f);
    cube3Node->Position = glm::vec3(3.0f, 10.0f, 0.0f);

    Ciri::SceneNode *sponzaNode = mainScene->LoadModel("sponza", "resources/mesh/sponza/sponza.obj");
    Ciri::SceneNode *dragonNode = mainScene->LoadModel("dragon", "resources/mesh/dragon/dragon.obj");
    sponzaNode->Scale = glm::vec3(0.05f);
    dragonNode->Position = glm::vec3(10.0f, 10.0f, 0.0f);
    dragonNode->Scale = glm::vec3(10.0f);

    // UI
    char *addmesh_name = new char[20]();
    glm::vec3 addmesh_position = glm::vec3(0.0f);
    glm::vec3 addmesh_scale = glm::vec3(1.0f);
    char *addmesh_filepath = new char[100];
    const char *default_filepath = "resources/mesh/dragon/dragon.obj";
    std::strcpy(addmesh_filepath, default_filepath);

    while (!glfwWindowShouldClose(window))
    {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ImGui::ShowDemoWindow();

        // Options
        ImGuiWindowFlags flags = mouseCaptured ? ImGuiWindowFlags_NoInputs : 0;
        ImGui::PushStyleColor(ImGuiCol_ResizeGrip, 0);

        ImGui::Begin("Debug Info", NULL, flags);
        ImGui::Text("Version: OpenGL %s", glGetString(GL_VERSION));
        ImGui::Text("Vendor: %s", glGetString(GL_VENDOR));
        ImGui::Text("Renderer: %s", glGetString(GL_RENDERER));
        ImGui::Separator();
        ImGui::Text("Mesh Count: %d", mainScene->GetMeshCount());
        ImGui::Text("Triangles: %d", mainScene->GetTotalTriCount());
        ImGui::Text("Frame Time: %fms", deltaTime * 1000);
        ImGui::Text("FPS: %f", 1.0f / deltaTime);
        ImGui::Separator();
        ImGui::Text("Camera Position: %.3f, %.3f, %.3f", cameraPos.x, cameraPos.y, cameraPos.z);
        ImGui::SliderFloat("Camera Speed High", &cameraSpeedHigh, 1.0f, 1000.0f);
        ImGui::SliderFloat("Camera Speed Low", &cameraSpeedLow, 1.0f, 500.0f);
        ImGui::SliderFloat("Render Distance", &renderDistance, 10.0f, 10000.0f);
        if (ImGui::BeginCombo("Shading", shaders[selected]->name, 0))
        {
            for (auto &pair : shaders)
            {
                Ciri::Shader *shader = pair.second; // TODO: Blank root name is buggy and icon handling is rough
                const bool is_selected = (selected == shader->type);
                if (ImGui::Selectable(shader->name, is_selected))
                    selected = shader->type;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::Separator();

        if (ImGui::Button("Add Mesh.."))
            ImGui::OpenPopup("Add Mesh");

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Add Mesh", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            const char *mesh_types[] = {"Cube", "Quad", "Load Model.."};
            static int type = 0;
            ImGui::Combo("Type", &type, mesh_types, IM_ARRAYSIZE(mesh_types));
            ImGui::Separator();

            ImGui::InputText("Name", addmesh_name, 20);
            ImGui::InputFloat3("Position", &addmesh_position.x);
            ImGui::InputFloat3("Scale", &addmesh_scale.x);
            if (type == 2)
            {
                ImGui::InputText("File Path", addmesh_filepath, 100);
            }
            ImGui::Separator();

            if (ImGui::Button("OK", ImVec2(120, 0)))
            {
                switch (type)
                {
                case 0:
                {
                    Ciri::Mesh *cube = new Ciri::Cube();
                    cube->Construct();
                    Ciri::SceneNode *cubeNode = mainScene->AddMesh(addmesh_name, cube);
                    cubeNode->Position = addmesh_position;
                    cubeNode->Scale = addmesh_scale;
                    break;
                }
                case 1:
                {
                    Ciri::Mesh *quad = new Ciri::Quad();
                    quad->Construct();
                    Ciri::SceneNode *quadNode = mainScene->AddMesh(addmesh_name, quad);
                    quadNode->Position = addmesh_position;
                    quadNode->Scale = addmesh_scale;
                    break;
                }
                case 2:
                {
                    Ciri::SceneNode *dragonNode = mainScene->LoadModel(addmesh_name, addmesh_filepath);
                    dragonNode->Position = addmesh_position;
                    dragonNode->Scale = addmesh_scale;
                    break;
                }
                }
                ImGui::CloseCurrentPopup();

                // Reset values as opposed to saving them
                std::memset(addmesh_name, 0, 20);
                addmesh_position = glm::vec3(0.0f);
                addmesh_scale = glm::vec3(1.0f);
                std::strcpy(addmesh_filepath, default_filepath);
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::End();

        ImGui::Begin("Scene", NULL, flags);
        SceneUI(mainScene->GetRoot(), 0);
        ImGui::End();

        ImGui::Begin("Mesh Settings", NULL, flags);
        if (selected_node)
        {
            ImGui::Text(selected_node->Name.c_str());
            ImGui::InputFloat3("Position", &selected_node->Position.x);
            ImGui::InputFloat3("Scale", &selected_node->Scale.x);
        }
        ImGui::End();

        ImGui::PopStyleColor(1);
        ImGui::Render();

        // Input
        poll_input(window);

        // Rendering
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(1.0f, 1.0f, 0.0f, 1.0f);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, renderDistance);

        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        glm::mat4 model = glm::mat4(1.0f);
        RenderScene(mainScene->GetRoot(), selected, shaderLibrary, projection, view, model);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}