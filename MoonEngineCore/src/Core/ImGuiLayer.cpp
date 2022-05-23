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
#include "Utils/IconsFontAwesome.h"

namespace MoonEngine
{
	glm::vec2 ImGuiLayer::ViewportPosition;
	glm::vec2 ImGuiLayer::ViewportSize;
	glm::mat4 ImGuiLayer::CameraProjection;
	bool ImGuiLayer::m_EventsAllowed;

	void ImGuiLayer::Init()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		ImGui::StyleColorsDark();
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL(Window::GetWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 330");

		//io.Fonts->AddFontDefault();
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
		ImFont* font = io.Fonts->AddFontFromFileTTF("res/Fonts/Roboto/Roboto-Medium.ttf", 16.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
		ImFontConfig config;
		config.MergeMode = true;

		config.GlyphExtraSpacing.x = 5.0f;
		config.OversampleH = 3;
		config.OversampleV = 1;
		static const ImWchar icon_ranges[] = { ICON_MIN_FK, ICON_MAX_16_FK, 0};
		io.Fonts->AddFontFromFileTTF("res/Fonts/ForkAwesome/forkawesome-webfont.ttf", 14.0f, &config, icon_ranges);
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
}
