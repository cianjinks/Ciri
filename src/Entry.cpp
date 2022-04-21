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

#include "Render/Renderer.h"
#include "Render/Shader.h"
#include "Render/Material.h"
#include "Scene/Scene.h"
#include "Scene/Camera.h"
#include "Mesh/Primitive.h"

// Window
const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 900;
bool mouseCaptured = false;

// Camera
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool firstMouse = true;
float lastX = (float)WINDOW_WIDTH / 2;
float lastY = (float)WINDOW_HEIGHT / 2;

bool debug = false;
bool mipmap = true;

// Camera & Renderer Struct (REMOVE)
struct TempCR
{
	Ciri::Renderer *renderer;
	Ciri::Camera *camera;
};

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	TempCR *ptr = (TempCR *)glfwGetWindowUserPointer(window);
	Ciri::Renderer *renderer = ptr->renderer;

	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		// Yikes :(
		Ciri::ShaderType next =
			static_cast<Ciri::ShaderType>(
				(static_cast<int>(renderer->GetCurrentShader()) + 1) % renderer->GetShaderLibrary()->GetShaderCount());
		renderer->SetCurrentShader(next);
	}
}

void poll_input(Ciri::Camera *camera, GLFWwindow *window)
{
	if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
	{
		return;
	}

	float cameraSpeed = camera->SpeedLow;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		cameraSpeed = camera->SpeedHigh * deltaTime;
	}
	else
	{
		cameraSpeed = camera->SpeedLow * deltaTime;
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera->Position += cameraSpeed * camera->Front;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera->Position -= cameraSpeed * camera->Front;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera->Position -= glm::normalize(glm::cross(camera->Front, camera->Up)) * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera->Position += glm::normalize(glm::cross(camera->Front, camera->Up)) * cameraSpeed;
	}
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
{
	if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
	{
		return;
	}

	TempCR *ptr = (TempCR *)glfwGetWindowUserPointer(window);
	Ciri::Camera *camera = ptr->camera;

	if (firstMouse)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

	float xoffset = (float)xpos - lastX;
	float yoffset = lastY - (float)ypos;
	lastX = (float)xpos;
	lastY = (float)ypos;

	float sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	camera->Yaw += xoffset;
	camera->Pitch += yoffset;

	if (camera->Pitch > 89.0f)
		camera->Pitch = 89.0f;
	if (camera->Pitch < -89.0f)
		camera->Pitch = -89.0f;

	camera->RecalcDirection();
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
	if (type == GL_DEBUG_TYPE_ERROR)
	{
		CIRI_ERROR("GL ERROR: type = 0x{:x}, severity = 0x{:x}, message = {}",
				   type, severity, message);
	}
	else
	{
		CIRI_WARN("GL CALLBACK: type = 0x{:x}, severity = 0x{:x}, message = {}",
				  type, severity, message);
	}
}

static Ciri::SceneNode *selected_node = nullptr;
void SceneUI(Ciri::SceneNode *root, int ptr_id)
{
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

	// Leaf
	bool leaf = root->Children.empty();
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
	bool node_open = ImGui::TreeNodeEx((void *)(intptr_t)ptr_id, node_flags, "%s %s", root->NodeMesh ? ICON_FK_CUBE : ICON_FK_SQUARE_O, root->Name.c_str());

	// Update selected node on click but not expand toggle click
	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
	{
		selected_node = root;
	}

	if (node_open && !leaf)
	{
		for (Ciri::SceneNode *node : root->Children)
		{
			SceneUI(node, ptr_id++);
		}

		ImGui::TreePop();
	}
	ImGui::PopStyleVar();
}

void MaterialLibraryUI(Ciri::MaterialLibrary &library, Ciri::Material *&selected_material, ImGuiWindowFlags &flags)
{
	ImGui::Begin("Materials", NULL, flags);
	for (auto &pair : library.GetMaterials())
	{
		std::string name = pair.first;
		Ciri::Material *material = pair.second;
		std::string text = ICON_FK_CODEPEN + std::string(" ") + name;
		if (ImGui::Selectable(text.c_str(), selected_material == material))
		{
			selected_material = material;
		}
	}
	ImGui::End();
}

void CreateTextureCombo(const char *name, std::string &material_tex_name, uint32_t &material_tex_id, Ciri::Scene *scene)
{
	std::string texture_name = "None";
	if (!material_tex_name.empty())
	{
		texture_name = material_tex_name;
	}

	if (ImGui::BeginCombo(name, texture_name.c_str(), 0))
	{
		for (auto &pair : scene->MatLib.GetTextures())
		{
			std::string tex_name = pair.first;
			uint32_t texture_id = pair.second;
			const bool is_selected = (material_tex_id == texture_id);
			if (ImGui::Selectable(tex_name.c_str(), is_selected))
			{
				material_tex_id = texture_id;
				material_tex_name = tex_name;
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
}

void MaterialSettingsUI(Ciri::Material *material, Ciri::Scene *scene, ImGuiWindowFlags &flags)
{
	ImGui::Begin("Material Settings", NULL, flags);

	if (material)
	{
		ImGui::Text(material->info.name.c_str());
		ImGui::InputFloat3("Base Color", &material->baseColor.x);
		CreateTextureCombo("Albedo Texture", material->info.baseColorFilepath, material->baseColorTextureID, scene);
		CreateTextureCombo("Normal Texture", material->info.normalFilepath, material->normalTextureID, scene);
		CreateTextureCombo("Metallic Texture", material->info.metallicFilepath, material->metallicTextureID, scene);
		CreateTextureCombo("Roughness Texture", material->info.roughnessFilepath, material->roughnessTextureID, scene);
		CreateTextureCombo("Emissive Texture", material->info.emissiveFilepath, material->emissiveTextureID, scene);
		CreateTextureCombo("Occlusion Texture", material->info.occlusionFilepath, material->occlusionTextureID, scene);
		ImGui::InputFloat("Subsurface", &material->subsurface);
		ImGui::InputFloat("Metallic", &material->metallic);
		ImGui::InputFloat("Specular", &material->specular);
		ImGui::InputFloat("Specular Tint", &material->specularTint);
		ImGui::InputFloat("Roughness", &material->roughness);
		ImGui::InputFloat("Anisotropic", &material->anisotropic);
		ImGui::InputFloat("Sheen", &material->sheen);
		ImGui::InputFloat("Sheen Tint", &material->sheenTint);
		ImGui::InputFloat("Clearcoat", &material->clearcoat);
		ImGui::InputFloat("Clearcoat Gloss", &material->clearcoatGloss);
	}

	ImGui::End();
}

int main()
{
	Ciri::Log::Init();
	CIRI_LOG("Logger Initialised");

	GLFWwindow *window;

	// GLFW and Glad
	if (!glfwInit())
	{
		CIRI_ERROR("GLFW Initialization Failed");
		return 0;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Ciri", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		CIRI_ERROR("GLFW Window Creation Failed");
		return 0;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	if (!status)
	{
		CIRI_ERROR("GLAD Initialization Failed");
		return 0;
	}

	if (debug)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(MessageCallback, 0);
	}

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

	// Renderer
	Ciri::Renderer *renderer = new Ciri::Renderer(WINDOW_WIDTH, WINDOW_HEIGHT);

	// Camera
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
	glm::vec3 cameraDir = glm::vec3(0.0f, 0.0f, -1.0f);
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	Ciri::Camera *camera = new Ciri::Camera(cameraPos, cameraDir, -90.0f, 0.0f, float(width), float(height));

	// GLFW Callbacks
	TempCR *windowPTR = new TempCR{renderer, camera};
	glfwSetWindowUserPointer(window, windowPTR);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// Shaders
	auto &shaders = renderer->GetShaderLibrary()->GetShaderList();

	// Scene
	Ciri::Scene *mainScene = new Ciri::Scene("Main Scene");

	Ciri::Mesh *sphere = new Ciri::Sphere(100, 100, false);
	Ciri::Mesh *cube1 = new Ciri::Cube();
	Ciri::Mesh *cube2 = new Ciri::Cube();
	Ciri::Mesh *cube3 = new Ciri::Cube();
	sphere->Construct();
	cube1->Construct();
	cube2->Construct();
	cube3->Construct();
	Ciri::SceneNode *sphereNode = mainScene->AddMesh("sphere", sphere);
	Ciri::SceneNode *cube1Node = mainScene->AddMesh("cube1", cube1);
	Ciri::SceneNode *cube2Node = mainScene->AddMesh("cube2", cube2);
	Ciri::SceneNode *cube3Node = mainScene->AddMesh("cube3", cube3);
	sphereNode->Position = glm::vec3(0.0f, 15.0f, 0.0f);
	sphereNode->Scale = glm::vec3(3.0f);
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
	int h_segments = 4;
	int v_segments = 4;
	bool flat_shade = false; // UV spherer
	std::strcpy(addmesh_filepath, default_filepath);

	Ciri::Material *selected_material = nullptr;

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
		ImGui::SliderFloat("Camera Speed High", &camera->SpeedHigh, 1.0f, 1000.0f);
		ImGui::SliderFloat("Camera Speed Low", &camera->SpeedLow, 1.0f, 500.0f);
		ImGui::SliderFloat("Render Distance", &camera->Far, 10.0f, 1000.0f);
		if (ImGui::BeginCombo("Shading", shaders[renderer->GetCurrentShader()]->name, 0))
		{
			for (auto &pair : shaders)
			{
				Ciri::Shader *shader = pair.second;
				const bool is_selected = (renderer->GetCurrentShader() == shader->type);
				if (ImGui::Selectable(shader->name, is_selected))
					renderer->SetCurrentShader(shader->type);
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
			const char *mesh_types[] = {"Cube", "Quad", "Sphere", "Load Model.."};
			static int type = 0;
			ImGui::Combo("Type", &type, mesh_types, IM_ARRAYSIZE(mesh_types));
			ImGui::Separator();

			ImGui::InputText("Name", addmesh_name, 20);
			ImGui::InputFloat3("Position", &addmesh_position.x);
			ImGui::InputFloat3("Scale", &addmesh_scale.x);
			if (type == 2)
			{
				ImGui::InputInt("H Segments", &h_segments);
				ImGui::InputInt("V Segments", &v_segments);
				ImGui::Checkbox("Flat Shade", &flat_shade);
			}
			else if (type == 3)
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
					Ciri::Mesh *sphere = new Ciri::Sphere(h_segments, v_segments, flat_shade);
					sphere->Construct();
					Ciri::SceneNode *sphereNode = mainScene->AddMesh(addmesh_name, sphere);
					sphereNode->Position = addmesh_position;
					sphereNode->Scale = addmesh_scale;
					break;
				}
				case 3:
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
				h_segments = 4;
				v_segments = 4;
				flat_shade = false;
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

			// Material setting
			if (selected_node->NodeMesh)
			{
				if (ImGui::BeginCombo("Material", selected_node->NodeMaterial->info.name.c_str(), 0))
				{
					for (auto &pair : mainScene->MatLib.GetMaterials())
					{
						Ciri::Material *material = pair.second;
						const bool is_selected = (selected_node->NodeMaterial == material);
						if (ImGui::Selectable(material->info.name.c_str(), is_selected))
							selected_node->NodeMaterial = material;
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
			}
		}
		ImGui::End();

		MaterialLibraryUI(mainScene->MatLib, selected_material, flags);
		MaterialSettingsUI(selected_material, mainScene, flags);

		ImGui::PopStyleColor(1);
		ImGui::Render();

		// Input
		poll_input(camera, window);

		// Rendering
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glViewport(0, 0, width, height);
		renderer->Render(mainScene, camera);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete windowPTR;

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}