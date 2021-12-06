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

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void ErrorHandleShader(GLuint &shader)
{
    GLint result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
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

int main()
{
    GLFWwindow *window;

    // GLFW and Glad
    if (!glfwInit())
    {
        std::cout << "GLFW Initialization Failed" << std::endl;
        return 0;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    const int WINDOW_WIDTH = 1080;
    const int WINDOW_HEIGHT = 720;

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Simple example", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        std::cout << "GLFW Window Creation Failed" << std::endl;
        return 0;
    }

    glfwMakeContextCurrent(window);
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    if (!status)
    {
        std::cout << "GLAD Initialization Failed" << std::endl;
        return 0;
    }

    // ImGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460 core");

    // Test .obj loading
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn;
    std::string err;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "resources/mesh/cube.obj");
    if (!ret)
    {
        std::cout << "Failed to load object!" << std::endl;
        std::cout << err << std::endl;
    }

    // Spinning Triangle Test
    static const struct
    {
        float x, y;
        float r, g, b;
    } vertices[3] =
        {
            {-0.6f, -0.4f, 1.f, 0.f, 0.f},
            {0.6f, -0.4f, 0.f, 1.f, 0.f},
            {0.f, 0.6f, 0.f, 0.f, 1.f}};

    std::string vertexShader = ParseShaderFromFile("resources/shader/test.vert");
    std::string fragmentShader = ParseShaderFromFile("resources/shader/test.frag");
    const char *vertexSrc = vertexShader.c_str();
    const char *fragSrc = fragmentShader.c_str();

    GLuint vaoID;
    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    GLuint vboID;
    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (void *)(sizeof(float) * 2));
    glEnableVertexAttribArray(1);

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
        ImGui::End();

        ImGui::Render();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(1.0f, 1.0f, 0.0f, 1.0f);

        float ratio;
        int width, height;
        glm::mat4 m, p, mvp;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        m = glm::mat4(1.0f);
        m = glm::rotate(m, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        p = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 1.0f, -1.0f);
        mvp = p * m;

        glUseProgram(programID);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));
        glDrawArrays(GL_TRIANGLES, 0, 3);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}