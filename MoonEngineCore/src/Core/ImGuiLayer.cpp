#include "mpch.h"
#include "Core/Window.h"
#include "ImGuiLayer.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_glfw.cpp"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_opengl3.cpp"
#include "GLFW/glfw3.h"
#include "Event/Events.h"

namespace MoonEngine
{
	glm::vec2 ImGuiLayer::ViewportSize;
	glm::vec2 ImGuiLayer::ViewportPosition;
	bool ImGuiLayer::m_EventsAllowed;

	void ImGuiLayer::Init()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		StyleCustomDark();

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL(Window::GetWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 330");

		//io.Fonts->AddFontDefault();
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
		ImFont* font = io.Fonts->AddFontFromFileTTF("res/Fonts/Roboto/Roboto-Medium.ttf", 16.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	}

	void ImGuiLayer::BeginDrawUI()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::EndDrawUI()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void ImGuiLayer::OnEvent(Event& event)
	{
		if (m_EventsAllowed)
		{
			EventHandler handler = { event };
			handler.HandleEvent(EventType::Mouse_Scroll, EVENT_FN_POINTER(OnMouseScroll));
		}
	}

	void ImGuiLayer::BlockEvent(bool state)
	{
		m_EventsAllowed = state;
	}

	bool ImGuiLayer::OnMouseScroll(Event& event)
	{
		MouseScrollEvent& e = (MouseScrollEvent&)event;
		ImGuiIO io = ImGui::GetIO();
		io.MouseWheel = e.ScrollY();
		io.MouseWheelH = e.ScrollX();
		return true;
	}

	void ImGuiLayer::StyleCustomDark(int is3D)
	{
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec4* colors = style.Colors;

		//Colors
		colors[ImGuiCol_Text] = ImColor(255, 255, 255, 255);
		colors[ImGuiCol_TextDisabled] = ImColor(150, 150, 150, 255);

		ImColor window = ImColor(35, 35, 35, 255);
		colors[ImGuiCol_WindowBg] = window;
		colors[ImGuiCol_ChildBg] = window;
		colors[ImGuiCol_PopupBg] = window;

		colors[ImGuiCol_Border] = ImColor(75, 75, 75, 150);
		colors[ImGuiCol_BorderShadow] = ImColor(150, 150, 150, 50);

		colors[ImGuiCol_FrameBg] = ImColor(100, 100, 100, 255);
		colors[ImGuiCol_FrameBgHovered] = ImColor(125, 125, 125, 255);
		colors[ImGuiCol_FrameBgActive] = ImColor(150, 150, 150, 255);

		colors[ImGuiCol_TitleBg] = ImColor(20, 20, 20, 255);
		colors[ImGuiCol_TitleBgActive] = ImColor(10, 10, 10, 255);
		colors[ImGuiCol_TitleBgCollapsed] = ImColor(20, 20, 20, 255);

		colors[ImGuiCol_MenuBarBg] = window;

		colors[ImGuiCol_ScrollbarBg] = ImColor(75, 75, 75, 150);
		colors[ImGuiCol_ScrollbarGrab] = ImColor(150, 150, 150, 255);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(125, 125, 125, 255);
		colors[ImGuiCol_ScrollbarGrabActive] = ImColor(100, 100, 100, 255);

		colors[ImGuiCol_CheckMark] = ImColor(255, 255, 255, 255);
		colors[ImGuiCol_SliderGrab] = ImColor(255, 255, 255, 255);
		colors[ImGuiCol_SliderGrabActive] = ImColor(0, 0, 0, 150);

		colors[ImGuiCol_Button] = ImColor(100, 100, 100, 255);
		colors[ImGuiCol_ButtonHovered] = ImColor(125, 125, 125, 255);
		colors[ImGuiCol_ButtonActive] = ImColor(175, 175, 175, 255);

		colors[ImGuiCol_Header] = ImColor(100, 100, 100, 225);
		colors[ImGuiCol_HeaderHovered] = ImColor(125, 125, 125, 255);
		colors[ImGuiCol_HeaderActive] = ImColor(150, 150, 150, 255);

		colors[ImGuiCol_Separator] = ImColor(0, 0, 0, 150);
		colors[ImGuiCol_SeparatorHovered] = ImColor(200, 200, 200, 150);
		colors[ImGuiCol_SeparatorActive] = ImColor(250, 250, 250, 255);

		colors[ImGuiCol_ResizeGrip] = ImColor(100, 100, 100, 50);
		colors[ImGuiCol_ResizeGripHovered] = ImColor(125, 125, 125, 200);
		colors[ImGuiCol_ResizeGripActive] = ImColor(125, 125, 125, 250);

		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.73f, 0.73f, 0.73f, 0.35f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);

		//Sizes
		//Main
		style.WindowPadding = ImVec2(5, 5);
		style.FramePadding = ImVec2(6, 4);
		style.ItemSpacing = ImVec2(6, 3);
		style.ScrollbarSize = 14;
		style.GrabMinSize = 10;

		//Borders
		style.WindowBorderSize = 1;
		style.ChildBorderSize = 1;
		style.PopupBorderSize = 0;
		style.FrameBorderSize = is3D;
		style.PopupBorderSize = 0;

		style.WindowRounding = 3;
		style.ChildRounding = 3;
		style.FrameRounding = 3;
		style.PopupRounding = 3;
		style.ScrollbarRounding = 3;
		style.GrabRounding = 3;

		style.WindowMenuButtonPosition = ImGuiDir_Right;

#ifdef IMGUI_HAS_DOCK 
		style.TabBorderSize = is3D;
		style.TabRounding = 3;

		colors[ImGuiCol_Tab] = ImColor(75, 75, 75, 255);
		colors[ImGuiCol_TabHovered] = ImColor(150, 150, 150, 255);
		colors[ImGuiCol_TabActive] = ImColor(125, 125, 125, 255);
		colors[ImGuiCol_TabUnfocused] = ImColor(100, 100, 100, 150);
		colors[ImGuiCol_TabUnfocusedActive] = ImColor(125, 125, 125, 150);
		colors[ImGuiCol_DockingPreview] = ImColor(255, 255, 255, 50);
		colors[ImGuiCol_DockingEmptyBg] = ImColor(50, 50, 50, 255);

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
#endif
	}
}
