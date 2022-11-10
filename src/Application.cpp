#include "Application.h"

#include "imgui.h"

namespace Ciri
{
    Application::Application()
    {
        Log::Init();
        m_Window = CreateS<Window>(Name, 1600, 900);
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

        m_Viewport->GetGizmo()->SetSelectedEntity(m_SceneHierarchyPanel->GetSelectedEntity());
        m_Viewport->OnUIRender();

        // ImGui::ShowDemoWindow();
        m_StatisticsPanel->OnUIRender();
        m_SceneHierarchyPanel->OnUIRender();
        m_EntitySettingsPanel->SetSelectedEntity(m_SceneHierarchyPanel->GetSelectedEntity());
        m_EntitySettingsPanel->OnUIRender();
        m_MaterialLibraryPanel->OnUIRender();
        m_MaterialSettingsPanel->SetSelectedNode(m_MaterialLibraryPanel->GetSelectedMaterial());
        m_MaterialSettingsPanel->OnUIRender();
        m_RendererSettingsPanel->OnUIRender();
    }

    void Application::DefineUI()
    {
        m_Viewport = CreateU<Viewport>(m_Camera, m_Renderer);

        m_StatisticsPanel = CreateU<StatisticsPanel>(m_Scene);
        m_SceneHierarchyPanel = CreateU<SceneHierarchyPanel>(m_Scene);
        m_EntitySettingsPanel = CreateU<EntitySettingsPanel>();
        m_MaterialLibraryPanel = CreateU<MaterialLibraryPanel>(m_Scene->GetMaterialLibrary());
        m_MaterialSettingsPanel = CreateU<MaterialSettingsPanel>(m_Scene->GetMaterialLibrary());
        m_RendererSettingsPanel = CreateU<RendererSettingsPanel>(m_Renderer);
    }

    void Application::DefineScene()
    {
        Entity sphere_entity = m_Scene->CreateEntity("Sphere");
        S<Sphere> sphere_mesh = CreateS<Sphere>(32, 32);
        sphere_mesh->Construct();
        sphere_entity.AddComponent<MeshComponent>(sphere_mesh);
        MaterialSpecification spec = {};
        spec.name = "Sphere";
        spec.baseColor = {1.0f, 0.0f, 0.0f};
        S<Material> sphere_mat = m_Scene->CreateMaterial(spec);
        sphere_entity.AddComponent<MaterialComponent>(sphere_mat);
        TransformComponent &sphere_transform = sphere_entity.AddComponent<TransformComponent>();

        Entity cube_entity = m_Scene->CreateEntity("Cube");
        S<Cube> cube_mesh = CreateS<Cube>();
        cube_mesh->Construct();
        cube_entity.AddComponent<MeshComponent>(cube_mesh);
        spec.name = "Cube";
        spec.baseColor = {0.0f, 1.0f, 0.0f};
        S<Material> cube_mat = m_Scene->CreateMaterial(spec);
        cube_entity.AddComponent<MaterialComponent>(cube_mat);
        TransformComponent &cube_transform = cube_entity.AddComponent<TransformComponent>();
        cube_transform.Transform.Translation = glm::vec3(-3.0f, 0.0f, 0.0f);

        Entity quad_entity = m_Scene->CreateEntity("Quad");
        S<Quad> quad_mesh = CreateS<Quad>();
        quad_mesh->Construct();
        quad_entity.AddComponent<MeshComponent>(quad_mesh);
        spec.name = "Quad";
        spec.baseColor = {0.0f, 1.0f, 1.0f};
        S<Material> quad_mat = m_Scene->CreateMaterial(spec);
        quad_entity.AddComponent<MaterialComponent>(quad_mat);
        TransformComponent &quad_transform = quad_entity.AddComponent<TransformComponent>();
        quad_transform.Transform.Translation = glm::vec3(3.0f, 0.0f, 0.0f);

        m_Scene->CreateEntity("Empty 1");
        m_Scene->CreateEntity("Empty 2");

        CIRI_LOG("Scene Initialised");
    }
}