#include "EditorLayer.h"
#include "Renderer/Texture.h"
#include "Core/ImGuiLayer.h"
#include "ImGuiUtils.h"
#include <imgui/imgui_internal.h>

namespace MoonEngine
{
	bool demoWindow = false;

	void EditorLayer::Create()
	{
		LayerName = "Editor Layer";
		m_Scene = new Scene();

		m_EditorCamera = new EditorCamera();
		m_ViewportFramebuffer = new Framebuffer();
		m_HierarchyView.SetScene(m_Scene);

		ImGuiUtils::StyleCustomDark(0);
		Renderer::SetClearColor(glm::vec4{ 0.2f, 0.2f, 0.2f, 1.0f });
	}

	void EditorLayer::OnEvent(Event& event)
	{
		m_EditorCamera->OnEvent(event);
	}

	void EditorLayer::Update()
	{
		if (m_ViewportSize.x != m_ViewportFramebuffer->GetWidth() || m_ViewportSize.x != m_ViewportFramebuffer->GetHeight())
		{
			m_ViewportFramebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_EditorCamera->Resize(m_ViewportSize.x, m_ViewportSize.y);
			m_Scene->ResizeViewport(m_ViewportSize.x, m_ViewportSize.y);
		}

		if (!m_IsPlaying)
		{
			if (Input::MousePressed(0) && m_ViewportHovered)
				m_HierarchyView.MouseSelect();

			if(m_ViewportFocused)
				m_EditorCamera->Update();
		}

		m_ViewportFramebuffer->Bind();

		m_IsPlaying ? m_Scene->UpdateRuntime() : m_Scene->UpdateEditor(m_EditorCamera);

		m_ViewportFramebuffer->Unbind();
	}

	void EditorLayer::ViewportView(bool& state)
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar;
		ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGuiIO& io = ImGui::GetIO();

		ImGui::Begin("Viewport", &state, flags);

		m_ViewportPosition.x = ImGui::GetCursorScreenPos().x;
		m_ViewportPosition.y = ImGui::GetCursorScreenPos().y;
		m_ViewportSize.x = ImGui::GetContentRegionAvail().x;
		m_ViewportSize.y = ImGui::GetContentRegionAvail().y;

		m_ViewportHovered = ImGui::IsWindowHovered();
		m_ViewportFocused = ImGui::IsWindowFocused();

		ImGui::Image((void*)(uint64_t)m_ViewportFramebuffer->GetTexID(), { m_ViewportSize.x, m_ViewportSize.y }, { 0, 1 }, { 1, 0 });

		ImGui::End();
		ImGui::PopStyleVar();

		ImGuiLayer::ViewportPosition = m_ViewportPosition;
		ImGuiLayer::ViewportSize = m_ViewportSize;
		ImGuiLayer::CameraProjection = m_EditorCamera->GetViewProjection();
	}

	void EditorLayer::DrawGUI()
	{
		Dockspace(); //Start Dockspace
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
		Menubar();
		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImGui::GetStyle().Colors[ImGuiCol_DockingEmptyBg]);
		Sidemenubar();
		ImGui::PopStyleColor(2);

		if (m_IsHierarchyActive)
			m_HierarchyView.BeginHierarchyView(m_IsHierarchyActive);

		if (m_IsInspectorActive)
			m_HierarchyView.BeginInspectorView(m_IsInspectorActive);

		if (m_IsViewportActive)
			ViewportView(m_IsViewportActive);

		if (m_IsEditorSettingsActive)
			m_EditorSettingsView.BeginEditorSettings(m_IsEditorSettingsActive);

		if (m_IsDebugActive)
			DebugView(m_IsDebugActive);

		if (demoWindow)
			ImGui::ShowDemoWindow();

		ImGui::End(); //End Dockspace
	}

	void EditorLayer::Menubar()
	{
		ImGui::BeginMainMenuBar();
		{
			if (ImGui::BeginMenu("Views", true))
			{
				if (ImGui::MenuItem("Viewport", " ", m_IsViewportActive, true))
					m_IsViewportActive = !m_IsViewportActive;

				if (ImGui::MenuItem("Hierarchy", " ", m_IsHierarchyActive, true))
					m_IsHierarchyActive = !m_IsHierarchyActive;

				if (ImGui::MenuItem("Inspector", " ", m_IsInspectorActive, true))
					m_IsInspectorActive = !m_IsInspectorActive;

				if (ImGui::MenuItem("Editor Settings", " ", m_IsEditorSettingsActive, true))
					m_IsEditorSettingsActive = !m_IsEditorSettingsActive;

				if (ImGui::MenuItem("Debug", " ", m_IsDebugActive, true))
					m_IsDebugActive = !m_IsDebugActive;

				if (ImGui::MenuItem("Demo Window", " ", demoWindow, true))
					demoWindow = !demoWindow;

				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}

	void EditorLayer::Sidemenubar()
	{
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
		float height = ImGui::GetFrameHeight();

		if (ImGui::BeginViewportSideBar("##SecondaryMenuBar", NULL, ImGuiDir_Up, height, window_flags))
		{
			if (ImGui::BeginMenuBar())
			{
				float buttonSize = 50.0f;
				ImGuiUtils::AddPadding((ImGui::GetContentRegionAvail().x / 2.0f) - (buttonSize / 2.0f), 0);
				if (ImGui::Button(m_IsPlaying ? "Pause" : "Play", { buttonSize, height }))
					m_IsPlaying = !m_IsPlaying;
				ImGui::EndMenuBar();
			}
		}
		ImGui::End();
	}

	void EditorLayer::PlayScene()
	{}

	void EditorLayer::PauseScene()
	{}

	void EditorLayer::Dockspace()
	{
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin("DockSpace", nullptr, window_flags);

		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
	}

	void EditorLayer::DebugView(bool& state)
	{
		ImGui::Begin("Debug", &state);
		ImGui::Text("FPS: %.1f FPS (%.2f ms/frame) ", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("Mouse X: %.1f, Mouse Y: %.1f", Input::GetX(), Input::GetY());
		ImGui::Text("Ortho X: %.1f, Ortho Y: %.1f", Input::OrthoX(), Input::OrthoY());
		ImGui::End();
	}

	void EditorLayer::Destroy()
	{
		delete m_EditorCamera;
	}
}