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
            float dt = m_Window->GetTimeStep();
            m_Camera->OnUpdate(dt);
            m_Renderer->RenderScene(dt, m_Scene, m_Camera, UI::IsActive());
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

#if 0
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
        TransformComponent &winter_transform = winter_entity.GetComponent<TransformComponent>();
        winter_transform.Transform.SetLocalScale(glm::vec3(10.0f));
        winter_entity.UpdateTransforms();

        Entity point_light1 = m_Scene->CreateEntity("Point Light 1");
        point_light1.AddComponent<LightComponent>(LightType::POINT, glm::vec3(1.0f, 1.0f, 0.0f));
        TransformComponent &pl1_transform = point_light1.GetComponent<TransformComponent>();
        pl1_transform.Transform.SetLocalTranslation({0.0f, 30.0f, 50.0f});
        point_light1.UpdateTransforms();

        Entity point_light2 = m_Scene->CreateEntity("Point Light 2");
        point_light2.AddComponent<LightComponent>(LightType::POINT, glm::vec3(0.0f, 1.0f, 0.0f));
        TransformComponent &pl2_transform = point_light2.GetComponent<TransformComponent>();
        pl2_transform.Transform.SetLocalTranslation({40.0f, 30.0f, 0.0f});
        point_light2.UpdateTransforms();

        Entity point_light3 = m_Scene->CreateEntity("Point Light 3");
        point_light3.AddComponent<LightComponent>(LightType::POINT, glm::vec3(0.5f, 0.0f, 0.5f));
        TransformComponent &pl3_transform = point_light3.GetComponent<TransformComponent>();
        pl3_transform.Transform.SetLocalTranslation({-40.0f, 30.0f, 0.0f});
        point_light3.UpdateTransforms();

        Entity spot_light1 = m_Scene->CreateEntity("Spot Light 1");
        LightComponent& lc = spot_light1.AddComponent<LightComponent>(LightType::SPOT, glm::vec3(1.0f, 1.0f, 1.0f));
        lc.InnerCutoff = 10.0f;
        lc.OuterCutoff = 20.0f;
        TransformComponent &sl1_transform = spot_light1.GetComponent<TransformComponent>();
        sl1_transform.Transform.SetLocalTranslation({-70.5f, 37.5f, 44.5f});
        spot_light1.UpdateTransforms();
        CIRI_LOG("Scene Initialised");
    }

#else

    void Application::DefineScene()
    {
        Entity quad_entity = m_Scene->CreateEntity("Quad");
        S<Quad> quad_mesh = CreateS<Quad>();
        quad_mesh->Construct();
        quad_entity.AddComponent<MeshComponent>(quad_mesh);
        MaterialSpecification spec = {};
        spec.name = "Quad";
        spec.baseColor = {0.0f, 1.0f, 1.0f};
        S<Material> quad_mat = m_Scene->CreateMaterial(spec);
        quad_entity.AddComponent<MaterialComponent>(quad_mat);
        TransformComponent &quad_transform = quad_entity.GetComponent<TransformComponent>();
        quad_transform.Transform.SetLocalScale({50.0f, 1.0f, 50.0f});
        quad_entity.UpdateTransforms();

        Entity point_light1 = m_Scene->CreateEntity("Point Light 1");
        point_light1.AddComponent<LightComponent>(LightType::POINT, glm::vec3(1.0f, 1.0f, 0.0f));
        TransformComponent &pl1_transform = point_light1.GetComponent<TransformComponent>();
        pl1_transform.Transform.SetLocalTranslation({0.0f, 5.0f, -20.0f});
        point_light1.UpdateTransforms();

        Entity spot_light1 = m_Scene->CreateEntity("Spot Light 1");
        spot_light1.AddComponent<LightComponent>(LightType::SPOT, glm::vec3(0.0f, 1.0f, 1.0f));
        TransformComponent &sl1_transform = spot_light1.GetComponent<TransformComponent>();
        sl1_transform.Transform.SetLocalTranslation({0.0f, 5.0f, 20.0f});
        spot_light1.UpdateTransforms();

        Entity elk = m_Scene->LoadModel(Importer::ASSIMP, "elk", "resources/model/gltf/elk/scene.gltf");
        TransformComponent &elk_transform = elk.GetComponent<TransformComponent>();
        elk_transform.Transform.SetLocalScale(glm::vec3(4.0f));
        elk.UpdateTransforms();

        // Entity vampire = m_Scene->LoadModel(Importer::ASSIMP, "vampire", "resources/model/dae/dancing_vampire.dae");

        CIRI_LOG("Scene Initialised");
    }
#endif
}