#include "mpch.h"
#include "Gui/ImGuiLayer.h"
#include "Gui/ImGuiUtils.h"

#include "Core/Application.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_glfw.cpp>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_opengl3.cpp>

#include <imguizmo/ImGuizmo.h>

#include <GLFW/glfw3.h>

namespace MoonEngine
{
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

		ImGuiUtils::StyleCustomDark(0);
		Resolution res = Application::GetResoultion();
		ImGui::GetIO().FontGlobalScale = 1.0f;

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL(Application::GetWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 330");

		io.Fonts->AddFontDefault();
	}

	void ImGuiLayer::BeginDrawGUI()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::EndDrawGUI()
	{
		auto& io = ImGui::GetIO();
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
}