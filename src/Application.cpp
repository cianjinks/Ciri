#include "Application.h"

#include "imgui.h"

namespace Ciri
{
    Application::Application()
    {
        Log::Init();
        m_Window = CreateS<Window>(Name, 1280, 720);
        m_Window->SetEventCallback([this](Event& event) { OnEvent(event); });
        UI::Init(m_Window);
        m_Renderer = CreateS<Renderer>(m_Window->Width, m_Window->Height);
        m_Renderer->PrintDeviceInfo();
        m_Scene = CreateS<Scene>("Main Scene");
        m_Camera = CreateS<Camera>(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, -1.0f),
                                   -90.0f, 0.0f, float(m_Window->Width), float(m_Window->Height));
        DefineUI();
        DefineScene();
        CIRI_LOG("Application Initialised");
    }

    void Application::Run()
    {
        while (!m_Window->ShouldClose())
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
            
            m_Camera->OnUpdate(m_Window->GetTimeStep());
            m_Renderer->RenderScene(m_Scene, m_Camera);
            UI::PreRender();
            OnUIRender();
            UI::PostRender();
            m_Window->OnUpdate();
        }
        m_Window->End();
    }

    void Application::OnEvent(Event& event)
    {
        if (!m_Window->IsCursorCaptured()) { UI::OnEvent(event); }
        if (UI::WantInput()) { return; }

        m_Window->OnEvent(event);
        m_Camera->OnEvent(event);
        m_Renderer->OnEvent(event);
    }

    void Application::OnUIRender()
    {
        if (!UI::IsActive()) { return; }

        // ImGui::ShowDemoWindow();
        m_SceneHierarchyPanel->OnUIRender();
        m_MeshSettingsPanel->SetSelectedNode(m_SceneHierarchyPanel->GetSelectedNode());
        m_MeshSettingsPanel->OnUIRender();
        m_MaterialLibraryPanel->OnUIRender();
        m_MaterialSettingsPanel->SetSelectedNode(m_MaterialLibraryPanel->GetSelectedMaterial());
        m_MaterialSettingsPanel->OnUIRender();
    }

    void Application::DefineUI()
    {
        m_SceneHierarchyPanel = CreateU<SceneHierarchyPanel>(m_Scene);
        m_MeshSettingsPanel = CreateU<MeshSettingsPanel>(m_Scene->MatLib);
        m_MaterialLibraryPanel = CreateU<MaterialLibraryPanel>(m_Scene->MatLib);
        m_MaterialSettingsPanel = CreateU<MaterialSettingsPanel>(m_Scene->MatLib);
    }

    void Application::DefineScene()
    {
        // TODO: Use U/S and more in Scene
        Ciri::Mesh *sphere = new Ciri::Sphere(100, 100, false);
        Ciri::Mesh *cube1 = new Ciri::Cube();
        Ciri::Mesh *cube2 = new Ciri::Cube();
        Ciri::Mesh *cube3 = new Ciri::Cube();
        sphere->Construct();
        cube1->Construct();
        cube2->Construct();
        cube3->Construct();
        SceneNode *sphereNode = m_Scene->AddMesh("sphere", sphere);
        SceneNode *cube1Node = m_Scene->AddMesh("cube1", cube1);
        SceneNode *cube2Node = m_Scene->AddMesh("cube2", cube2);
        SceneNode *cube3Node = m_Scene->AddMesh("cube3", cube3);
        sphereNode->Position = glm::vec3(0.0f, 15.0f, 0.0f);
        sphereNode->Scale = glm::vec3(3.0f);
        cube1Node->Position = glm::vec3(-3.0f, 10.0f, 0.0f);
        cube1Node->Scale = glm::vec3(0.5f);
        cube2Node->Position = glm::vec3(0.0f, 10.0f, 0.0f);
        cube2Node->Scale = glm::vec3(0.75f);
        cube3Node->Position = glm::vec3(3.0f, 10.0f, 0.0f);

        SceneNode *sponzaNode = m_Scene->LoadModel("sponza", "resources/mesh/sponza/sponza.obj");
        SceneNode *dragonNode = m_Scene->LoadModel("dragon", "resources/mesh/dragon/dragon.obj");
        sponzaNode->Scale = glm::vec3(0.05f);
        dragonNode->Position = glm::vec3(10.0f, 10.0f, 0.0f);
        dragonNode->Scale = glm::vec3(10.0f);

        CIRI_LOG("Scene Initialised");
    }
}