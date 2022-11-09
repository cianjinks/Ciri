#include "Application.h"

#include "imgui.h"

namespace Ciri
{
    Application::Application()
    {
        Log::Init();
        m_Window = CreateS<Window>(Name, 1280, 720);
        m_Window->SetEventCallback([this](Event &event)
                                   { OnEvent(event); });
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
            m_Camera->OnUpdate(m_Window->GetTimeStep());
            m_Renderer->RenderScene(m_Scene, m_Camera, UI::IsActive());
            m_Viewport->OnUpdate();
            UI::PreRender();
            OnUIRender();
            UI::PostRender();
            m_Window->OnUpdate();
        }
        m_Window->End();
    }

    void Application::OnEvent(Event &event)
    {
        m_Window->OnEvent(event);
        m_Camera->OnEvent(event);
        m_Renderer->OnEvent(event);

        /* TODO: Create event dispatcher class. */
        EventType type = event.GetEventType();
        switch (type)
        {
        case EventType::KEY_PRESS:
            KeyEvent &key_event = static_cast<KeyEvent &>(event);
            int key = key_event.GetKey();
            if (key == GLFW_KEY_F)
            {
                UI::ToggleActive();
                if (!UI::IsActive())
                {
                    m_Camera->Resize(m_Window->Width, m_Window->Height);
                    m_Renderer->Resize(m_Window->Width, m_Window->Height);
                }
            }
            break;
        }
    }

    void Application::OnUIRender()
    {
        if (!UI::IsActive())
        {
            return;
        }

        // m_Viewport->GetGizmo()->SetSelectedNode(m_SceneHierarchyPanel->GetSelectedNode());
        m_Viewport->OnUIRender();

        // ImGui::ShowDemoWindow();
        m_StatisticsPanel->OnUIRender();
        m_SceneHierarchyPanel->OnUIRender();
        // m_MeshSettingsPanel->SetSelectedNode(m_SceneHierarchyPanel->GetSelectedNode());
        // m_MeshSettingsPanel->OnUIRender();
        // m_MaterialLibraryPanel->OnUIRender();
        // m_MaterialSettingsPanel->SetSelectedNode(m_MaterialLibraryPanel->GetSelectedMaterial());
        // m_MaterialSettingsPanel->OnUIRender();
        m_RendererSettingsPanel->OnUIRender();
    }

    void Application::DefineUI()
    {
        m_Viewport = CreateU<Viewport>(m_Camera, m_Renderer);

        m_StatisticsPanel = CreateU<StatisticsPanel>(m_Scene);
        m_SceneHierarchyPanel = CreateU<SceneHierarchyPanel>(m_Scene);
        // m_MeshSettingsPanel = CreateU<MeshSettingsPanel>(m_Scene->MatLib);
        // m_MaterialLibraryPanel = CreateU<MaterialLibraryPanel>(m_Scene->MatLib);
        // m_MaterialSettingsPanel = CreateU<MaterialSettingsPanel>(m_Scene->MatLib);
        m_RendererSettingsPanel = CreateU<RendererSettingsPanel>(m_Renderer);
    }

    void Application::DefineScene()
    {
        Entity sphere_entity = m_Scene->CreateEntity("Sphere");
        S<Sphere> sphere_mesh = CreateS<Sphere>(32, 32);
        sphere_mesh->Construct();
        sphere_entity.AddComponent<MeshComponent>(sphere_mesh);
        TransformComponent &sphere_transform = sphere_entity.AddComponent<TransformComponent>();

        Entity cube_entity = m_Scene->CreateEntity("Cube");
        S<Cube> cube_mesh = CreateS<Cube>();
        cube_mesh->Construct();
        cube_entity.AddComponent<MeshComponent>(cube_mesh);
        TransformComponent &cube_transform = cube_entity.AddComponent<TransformComponent>();
        cube_transform.Transform.Translation = glm::vec3(-3.0f, 0.0f, 0.0f);

        Entity quad_entity = m_Scene->CreateEntity("Quad");
        S<Quad> quad_mesh = CreateS<Quad>();
        quad_mesh->Construct();
        quad_entity.AddComponent<MeshComponent>(quad_mesh);
        TransformComponent &quad_transform = quad_entity.AddComponent<TransformComponent>();
        quad_transform.Transform.Translation = glm::vec3(3.0f, 0.0f, 0.0f);

        CIRI_LOG("Scene Initialised");
    }
}