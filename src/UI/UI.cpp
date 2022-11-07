#include "UI.h"

#include "IconsForkAwesome.h"

namespace Ciri
{
    bool UI::s_HideUI = false;

    void UI::Init(const S<Window> &window)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        // Merge in icons from Fork Awesome
        static const ImWchar icons_ranges[] = {ICON_MIN_FK, ICON_MAX_FK, 0};
        ImFontConfig icons_config;
        icons_config.MergeMode = true;
        icons_config.PixelSnapH = true;
        icons_config.GlyphOffset = {0.f, 2.f};
        icons_config.GlyphMinAdvanceX = 20.0f; // necessary?
        io.Fonts->AddFontDefault();
        io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FK, 16.0f, &icons_config, icons_ranges);

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(window->GetWindow(), true);
        ImGui_ImplOpenGL3_Init("#version 460 core");
    }

    void UI::PreRender()
    {
        if (!s_HideUI)
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGuizmo::BeginFrame();
            ImGui::PushStyleColor(ImGuiCol_ResizeGrip, 0);

            ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
        }
    }

    void UI::PostRender()
    {
        if (!s_HideUI)
        {
            ImGui::PopStyleColor(1);
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
    }

    bool UI::WantInput()
    {
        ImGuiIO &io = ImGui::GetIO();
        return (io.WantCaptureMouse || io.WantCaptureKeyboard);
    }

    void UI::OnEvent(Event &event)
    {
        /* Unused. */
    }
}