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

#if 1
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
        TransformComponent &sphere_transform = sphere_entity.GetComponent<TransformComponent>();
        sphere_transform.Transform.SetLocalTranslation({0.0f, 1.0f, 0.0f});

        Entity cube_entity = m_Scene->CreateEntity("Cube");
        S<Cube> cube_mesh = CreateS<Cube>();
        cube_mesh->Construct();
        cube_entity.AddComponent<MeshComponent>(cube_mesh);
        spec.name = "Cube";
        spec.baseColor = {0.0f, 1.0f, 0.0f};
        S<Material> cube_mat = m_Scene->CreateMaterial(spec);
        cube_entity.AddComponent<MaterialComponent>(cube_mat);
        TransformComponent &cube_transform = cube_entity.GetComponent<TransformComponent>();
        cube_transform.Transform.SetLocalTranslation({-3.0f, 0.0f, 0.0f});

        Entity quad_entity = m_Scene->CreateEntity("Quad");
        S<Quad> quad_mesh = CreateS<Quad>();
        quad_mesh->Construct();
        quad_entity.AddComponent<MeshComponent>(quad_mesh);
        spec.name = "Quad";
        spec.baseColor = {0.0f, 1.0f, 1.0f};
        S<Material> quad_mat = m_Scene->CreateMaterial(spec);
        quad_entity.AddComponent<MaterialComponent>(quad_mat);
        TransformComponent &quad_transform = quad_entity.GetComponent<TransformComponent>();
        quad_transform.Transform.SetLocalTranslation({6.0f, 0.0f, 0.0f});

        Entity empty1 = m_Scene->CreateEntity("Empty 1");
        Entity empty2 = m_Scene->CreateEntity("Empty 2");

        cube_entity.SetParent(sphere_entity);
        quad_entity.SetParent(cube_entity);
        empty1.SetParent(sphere_entity);
        empty2.SetParent(cube_entity);

        // Entity sponza_entity = m_Scene->LoadModel(Importer::ASSIMP, "sponza", "resources/model/gltf/Sponza/glTF/Sponza.gltf");
        // TransformComponent &sponza_transform = sponza_entity.GetComponent<TransformComponent>();
        // sponza_transform.Transform.SetLocalScale({0.05f, 0.05f, 0.05f});
        // sponza_entity.UpdateTransforms();

        Entity winter_entity = m_Scene->LoadModel(Importer::ASSIMP, "winter", "resources/model/gltf/Winter/scene.gltf");

        Entity point_light = m_Scene->CreateEntity("Point Light 1");
        point_light.AddComponent<LightComponent>(LightType::POINT, glm::vec3(1.0f, 1.0f, 0.0f));
        TransformComponent &pl_transform = point_light.GetComponent<TransformComponent>();
        pl_transform.Transform.SetLocalTranslation({0.0f, 10.0f, 0.0f});

        CIRI_LOG("Scene Initialised");
    }

#else

    void Application::DefineScene()
    {
        Entity cube1_entity = m_Scene->CreateEntity("Cube 1");
        S<Cube> cube1_mesh = CreateS<Cube>();
        cube1_mesh->Construct();
        cube1_entity.AddComponent<MeshComponent>(cube1_mesh);
        MaterialSpecification spec = {};
        spec.name = "Cube 1";
        spec.baseColor = {0.0f, 1.0f, 0.0f};
        S<Material> cube1_mat = m_Scene->CreateMaterial(spec);
        cube1_entity.AddComponent<MaterialComponent>(cube1_mat);
        TransformComponent &cube1_transform = cube1_entity.GetComponent<TransformComponent>();
        cube1_transform.Transform.SetLocalTranslation({0.0f, 0.0f, 0.0f});

        Entity cube2_entity = m_Scene->CreateEntity("Cube 2");
        S<Cube> cube2_mesh = CreateS<Cube>();
        cube2_mesh->Construct();
        cube2_entity.AddComponent<MeshComponent>(cube2_mesh);
        spec.name = "Cube 2";
        spec.baseColor = {1.0f, 0.0f, 0.0f};
        S<Material> cube2_mat = m_Scene->CreateMaterial(spec);
        cube2_entity.AddComponent<MaterialComponent>(cube2_mat);
        TransformComponent &cube2_transform = cube2_entity.GetComponent<TransformComponent>();
        cube2_transform.Transform.SetLocalTranslation({0.0f, -2.0f, 0.0f});
        cube2_transform.Transform.SetLocalScale({0.5f, 0.5f, 0.5f});

        Entity cube3_entity = m_Scene->CreateEntity("Cube 3");
        S<Cube> cube3_mesh = CreateS<Cube>();
        cube3_mesh->Construct();
        cube3_entity.AddComponent<MeshComponent>(cube3_mesh);
        spec.name = "Cube 3";
        spec.baseColor = {0.0f, 1.0f, 1.0f};
        S<Material> cube3_mat = m_Scene->CreateMaterial(spec);
        cube3_entity.AddComponent<MaterialComponent>(cube3_mat);
        TransformComponent &cube3_transform = cube3_entity.GetComponent<TransformComponent>();
        cube3_transform.Transform.SetLocalTranslation({0.0f, -4.0f, 0.0f});
        cube3_transform.Transform.SetLocalScale({0.25f, 0.25f, 0.25f});

        cube2_entity.SetParent(cube1_entity);
        cube3_entity.SetParent(cube2_entity);

        CIRI_LOG("Scene Initialised");
    }
#endif
}