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

        Entity winter_entity = m_Scene->LoadModel(Importer::ASSIMP, "winter", "resources/model/gltf/submission_scene/winter_edit_ice_lake.gltf");
        TransformComponent &winter_transform = winter_entity.GetComponent<TransformComponent>();
        winter_transform.Transform.SetLocalScale(glm::vec3(10.0f));
        winter_entity.UpdateTransforms();

        Entity tree_entity = m_Scene->LoadModel(Importer::ASSIMP, "Tree", "resources/model/gltf/tree_star/tree_star.gltf");
        TransformComponent& tree_transform = tree_entity.GetComponent<TransformComponent>();
        tree_transform.Transform.SetLocalTranslation(glm::vec3(194.0f, 0, -24.0f));
        tree_transform.Transform.SetLocalScale(glm::vec3(10.0f));
        tree_entity.UpdateTransforms();

        Entity point_light1 = m_Scene->CreateEntity("Star Light");
        auto& star_lc = point_light1.AddComponent<LightComponent>(LightType::POINT, glm::vec3(1.0f, 1.0f, 0.0f));
        star_lc.Linear = 0.007f;
        star_lc.Quadratic = 0.0002f;
        TransformComponent &pl1_transform = point_light1.GetComponent<TransformComponent>();
        pl1_transform.Transform.SetLocalTranslation({194.2f, 70.9f, -23.67f});
        point_light1.UpdateTransforms();

        std::vector<glm::vec3> street_lights = {
            {214.1f, 37.76f, 16.4f},
            {214.1f, 37.76f, 44.6f},
            {141.6f, 37.76f, 44.6f},
            {141.6f, 37.76f, 16.4f},
            {75.5f, 37.76f, 44.6f},
            {75.5f, 37.76f, 16.4f},
            {-0.36f, 37.76f, 44.6f},
            {-0.36f, 37.76f, 16.4f},
            {-70.5f, 37.76f, 44.6f},
            {-70.5f, 37.76f, 16.4f}
        };

        Entity street_lights_entity = m_Scene->CreateEntity("Street Lights");
        for (int s = 0; s < street_lights.size(); s++)
        {
            Entity street_light = m_Scene->CreateEntity("Street Light " + std::to_string(s));
            auto& lc = street_light.AddComponent<LightComponent>(LightType::SPOT, glm::vec3(1.0f, 0.9f, 0.57f));
            lc.InnerCutoff = 4.0f;
            lc.OuterCutoff = 35.0f;
            TransformComponent &sl_transform = street_light.GetComponent<TransformComponent>();
            sl_transform.Transform.SetLocalTranslation(street_lights[s]);
            street_light.UpdateTransforms();
            street_light.SetParent(street_lights_entity);
        }


        Entity elk = m_Scene->LoadModel(Importer::ASSIMP, "elk", "resources/model/gltf/elk/scene.gltf");
        TransformComponent &elk_transform = elk.GetComponent<TransformComponent>();
        elk_transform.Transform.SetLocalScale(glm::vec3(4.0f));
        elk.UpdateTransforms();

        CIRI_LOG("Scene Initialised");
    }
}