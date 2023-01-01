#include "Application.h"

#include "imgui.h"
#include <glm/gtx/common.hpp>

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
            UpdateScene(dt);
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
        Entity winter_entity = m_Scene->LoadModel(Importer::ASSIMP, "winter", "resources/model/gltf/submission_scene/winter_edit.gltf");
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

        Entity elk1 = m_Scene->LoadModel(Importer::ASSIMP, "elk1", "resources/model/gltf/elk/scene.gltf");
        TransformComponent &elk1_transform = elk1.GetComponent<TransformComponent>();
        elk1_transform.Transform.SetLocalScale(glm::vec3(10.0f));
        elk1_transform.Transform.SetLocalTranslation(glm::vec3(277.54f, -2.84f, 45.35f));
        elk1_transform.Transform.SetLocalRotation(glm::radians(glm::vec3(90.0f, -90.0f, 180.0f)));
        elk1.UpdateTransforms();

        Entity elk2 = m_Scene->LoadModel(Importer::ASSIMP, "elk2", "resources/model/gltf/elk/scene.gltf");
        TransformComponent &elk2_transform = elk2.GetComponent<TransformComponent>();
        elk2_transform.Transform.SetLocalScale(glm::vec3(10.0f));
        elk2_transform.Transform.SetLocalTranslation(glm::vec3(298.0f, -2.84f, 19.9f));
        elk2_transform.Transform.SetLocalRotation(glm::radians(glm::vec3(90.0f, -90.0f, 180.0f)));
        elk2.UpdateTransforms();

        Entity elk3 = m_Scene->LoadModel(Importer::ASSIMP, "elk3", "resources/model/gltf/elk/scene.gltf");
        TransformComponent &elk3_transform = elk3.GetComponent<TransformComponent>();
        elk3_transform.Transform.SetLocalScale(glm::vec3(10.0f));
        elk3_transform.Transform.SetLocalTranslation(glm::vec3(298.0f, -2.84f, 63.28f));
        elk3_transform.Transform.SetLocalRotation(glm::radians(glm::vec3(90.0f, -90.0f, 180.0f)));
        elk3.UpdateTransforms();

        animated_elk.push_back(elk1);
        animated_elk.push_back(elk2);
        animated_elk.push_back(elk3);

        const static int num_pedestals = 6;
        glm::vec3 pedestal_positions[num_pedestals] = {
            {97.19, -0.1, -10.35},
            {97.19, -0.1, -29.43},
            {112.0, -0.1, -41.34},
            {130.0, -0.1, -41.34},
            {148.0, 0.3, -41.34},
            {166.0, 0.3, -41.34},
        };
        for (int p = 0; p < num_pedestals; p++)
        {
            Entity pedestal = m_Scene->LoadModel(Importer::ASSIMP, "pedestal" + std::to_string(p), "resources/model/gltf/pedestal/scene.gltf");
            TransformComponent &ptransform = pedestal.GetComponent<TransformComponent>();
            ptransform.Transform.SetLocalTranslation(pedestal_positions[p]);
            pedestal.UpdateTransforms();
        }

        // Objects on pedestals
        glm::vec3 pedestal_object_offset = glm::vec3(0.0f, 15.0f, 0.0f);
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
        sphere_transform.Transform.SetLocalTranslation(pedestal_positions[0] + pedestal_object_offset);
        sphere_transform.Transform.SetLocalScale(glm::vec3(3.0f));

        Entity cube_entity = m_Scene->CreateEntity("Cube");
        S<Cube> cube_mesh = CreateS<Cube>();
        cube_mesh->Construct();
        cube_entity.AddComponent<MeshComponent>(cube_mesh);
        spec.name = "Cube";
        spec.baseColor = {0.0f, 1.0f, 0.0f};
        S<Material> cube_mat = m_Scene->CreateMaterial(spec);
        cube_entity.AddComponent<MaterialComponent>(cube_mat);
        TransformComponent &cube_transform = cube_entity.GetComponent<TransformComponent>();
        cube_transform.Transform.SetLocalTranslation(pedestal_positions[1] + pedestal_object_offset);
        cube_transform.Transform.SetLocalScale(glm::vec3(3.0f));

        Entity sphere2_entity = m_Scene->CreateEntity("Sphere 2");
        S<Sphere> sphere2_mesh = CreateS<Sphere>(32, 32);
        sphere2_mesh->Construct();
        sphere2_entity.AddComponent<MeshComponent>(sphere2_mesh);
        spec.name = "Sphere 2";
        spec.baseColor = {1.0f, 0.0f, 1.0f};
        S<Material> sphere2_mat = m_Scene->CreateMaterial(spec);
        sphere2_entity.AddComponent<MaterialComponent>(sphere2_mat);
        TransformComponent &sphere2_transform = sphere2_entity.GetComponent<TransformComponent>();
        sphere2_transform.Transform.SetLocalTranslation(pedestal_positions[2] + pedestal_object_offset);
        sphere2_transform.Transform.SetLocalScale(glm::vec3(3.0f));

        Entity cube2_entity = m_Scene->CreateEntity("Cube 2");
        S<Cube> cube2_mesh = CreateS<Cube>();
        cube2_mesh->Construct();
        cube2_entity.AddComponent<MeshComponent>(cube2_mesh);
        spec.name = "Cube 2";
        spec.baseColor = {0.0f, 0.0f, 1.0f};
        S<Material> cube2_mat = m_Scene->CreateMaterial(spec);
        cube2_entity.AddComponent<MaterialComponent>(cube2_mat);
        TransformComponent &cube2_transform = cube2_entity.GetComponent<TransformComponent>();
        cube2_transform.Transform.SetLocalTranslation(pedestal_positions[3] + pedestal_object_offset);
        cube2_transform.Transform.SetLocalScale(glm::vec3(2.0f));
        cube2_transform.Transform.SetLocalRotation(glm::radians(glm::vec3(0.0f, 45.0f, 45.0f)));

        Entity sphere3_entity = m_Scene->CreateEntity("Sphere 3");
        S<Sphere> sphere3_mesh = CreateS<Sphere>(8, 8);
        sphere3_mesh->Construct();
        sphere3_entity.AddComponent<MeshComponent>(sphere3_mesh);
        spec.name = "Sphere 3";
        spec.baseColor = {0.5f, 0.0f, 1.0f};
        S<Material> sphere3_mat = m_Scene->CreateMaterial(spec);
        sphere3_entity.AddComponent<MaterialComponent>(sphere3_mat);
        TransformComponent &sphere3_transform = sphere3_entity.GetComponent<TransformComponent>();
        sphere3_transform.Transform.SetLocalTranslation(pedestal_positions[4] + pedestal_object_offset);
        sphere3_transform.Transform.SetLocalScale(glm::vec3(3.0f));
        sphere3_transform.Transform.SetLocalRotation(glm::radians(glm::vec3(45.0f, 0.0f, 0.0f)));

        Entity cube3_entity = m_Scene->CreateEntity("Cube 3");
        S<Cube> cube3_mesh = CreateS<Cube>();
        cube3_mesh->Construct();
        cube3_entity.AddComponent<MeshComponent>(cube3_mesh);
        spec.name = "Cube 3";
        spec.baseColor = {0.75f, 0.75f, 0.0f};
        S<Material> cube3_mat = m_Scene->CreateMaterial(spec);
        cube3_entity.AddComponent<MaterialComponent>(cube3_mat);
        TransformComponent &cube3_transform = cube3_entity.GetComponent<TransformComponent>();
        cube3_transform.Transform.SetLocalTranslation(pedestal_positions[5] + pedestal_object_offset);
        cube3_transform.Transform.SetLocalScale(glm::vec3(2.0f));

        CIRI_LOG("Scene Initialised");
    }

    void Application::UpdateScene(float dt)
    {
        float distance = 410.0f;
        float speed = 50.0f;
        float starts[3] = {277.54f, 298.0f, 298.0f};

        for (int e = 0; e < animated_elk.size(); e++)
        {
            Entity elk = animated_elk[e];
            auto& tc = elk.GetComponent<TransformComponent>();
            glm::vec3 translation = tc.Transform.GetLocalTranslation();
            translation.x = starts[e] - glm::fmod(glm::abs((translation.x - (speed * dt)) - starts[e]), distance);
            tc.Transform.SetLocalTranslation(translation);
            elk.UpdateTransforms();
        }
    }
}