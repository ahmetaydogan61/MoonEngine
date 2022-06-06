#include "EditorLayer.h"
#include "Core/ImGuiLayer.h"
#include "ImGuiUtils.h"
#include "Utils/IconsFontAwesome.h"
#include "ImGuizmo.h"
#include "Engine/Components.h"

#include "Utils/Serializer.h"

#include <imgui/imgui_internal.h>
#include "imfilebrowser.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Engine/Systems/ParticleSystem.h"

namespace MoonEngine
{
	bool demoWindow = false;
	
	ImGui::FileBrowser saveDialog{ ImGuiFileBrowserFlags_SelectDirectory | ImGuiFileBrowserFlags_CreateNewDir };
	ImGui::FileBrowser loadDialog{ ImGuiFileBrowserFlags_EnterNewFilename };

	void EditorLayer::Create()
	{
		LayerName = "Editor Layer";
		m_Scene = CreateRef<Scene>();

		m_EditorCamera = new EditorCamera();
		m_ViewportFramebuffer = new Framebuffer();
		m_HierarchyView.SetScene(m_Scene);

		ImGuiUtils::StyleCustomDark(0);
		Renderer::SetClearColor(glm::vec4{ 0.1f, 0.1f, 0.1f, 1.0f });

		m_PlayTexture = CreateRef<Texture>("res/EditorIcons/Play.png");
		m_StopTexture = CreateRef<Texture>("res/EditorIcons/Stop.png");
		m_SelectTexture = CreateRef<Texture>("res/EditorIcons/Select.png");
		m_TranslateTexture = CreateRef<Texture>("res/EditorIcons/Translate.png");
		m_RotateTexture = CreateRef<Texture>("res/EditorIcons/Rotate.png");
		m_ResizeTexture = CreateRef<Texture>("res/EditorIcons/Resize.png");

		Window::SetIcon("res/EditorIcons/Logo.png");

		saveDialog.SetTitle("Save Scene");
		saveDialog.SetTypeFilters({ ".moon" });
		saveDialog.SetPwd("res/Assets/Scenes");

		loadDialog.SetTitle("Load Scene");
		loadDialog.SetTypeFilters({ ".moon" });
		loadDialog.SetPwd("res/Assets/Scenes");
	}

	void EditorLayer::OnEvent(Event& event)
	{
		m_EditorCamera->OnEvent(event);
	}

	void EditorLayer::OnPlay()
	{
		m_Scene->OnPlay();
	}

	void EditorLayer::OnStop()
	{
		m_Scene->OnReset();
	}

	void EditorLayer::DrawGUI()
	{
		Dockspace(); //Start Dockspace
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
		ImGuiStyle& style = ImGui::GetStyle();
		float borderSize = style.WindowBorderSize;
		style.WindowBorderSize = 0;
		Menubar();
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 6, 6 });
		Sidemenubar();
		style.WindowBorderSize = borderSize;
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();

		if (demoWindow)
			ImGui::ShowDemoWindow();

		if (m_IsDebugActive)
			DebugView(m_IsDebugActive);

		if (m_IsEditorSettingsActive)
			m_EditorSettingsView.BeginEditorSettings(m_IsEditorSettingsActive);

		if (m_IsContentActive)
			m_ContentView.BeginContentView(m_IsContentActive);

		if (m_IsInspectorActive)
			m_HierarchyView.BeginInspectorView(m_IsInspectorActive);

		if (m_IsHierarchyActive)
			m_HierarchyView.BeginHierarchyView(m_IsHierarchyActive);

		if (m_IsViewportActive)
			ViewportView(m_IsViewportActive);

		Statusbar();
		ImGui::End(); //End Dockspace
	}

	void EditorLayer::Update()
	{
		if (m_ViewportSize.x != m_ViewportFramebuffer->GetWidth() || m_ViewportSize.y != m_ViewportFramebuffer->GetHeight())
		{
			m_ViewportFramebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_EditorCamera->Resize(m_ViewportSize.x, m_ViewportSize.y);
		}

		if (!m_IsPlaying)
		{
			if (Input::MousePressed(0) && m_ViewportHovered && !ImGuizmo::IsUsing())
				m_HierarchyView.MouseSelect();

			if (m_ViewportFocused)
				m_EditorCamera->UpdateFocused();

			if (m_ViewportHovered)
				m_EditorCamera->UpdateHovered();

			m_EditorCamera->Update();

			if (!Input::GetKey(KEY_LEFT_CONTROL) && m_ViewportFocused)
				if (Input::GetKey(KEY_Q))
					m_GizmoSelection = GIZMOSELECTION::NONE;
				else if (Input::GetKey(KEY_W))
					m_GizmoSelection = GIZMOSELECTION::TRANSLATE;
				else if (Input::GetKey(KEY_E))
					m_GizmoSelection = GIZMOSELECTION::RORTATE;
				else if (Input::GetKey(KEY_R))
					m_GizmoSelection = GIZMOSELECTION::SCALE;

			m_IsSnapping = Input::GetKey(KEY_LEFT_CONTROL);
		}
		else
			m_Scene->ResizeViewport(m_ViewportSize.x, m_ViewportSize.y);

		m_ViewportFramebuffer->Bind();

		m_IsPlaying ? m_Scene->UpdateRuntime() : m_Scene->UpdateEditor(m_EditorCamera);

		m_ViewportFramebuffer->Unbind();
	}

	void EditorLayer::ViewportView(bool& state)
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar;
		ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGuiIO& io = ImGui::GetIO();

		if (ImGui::Begin(ICON_FK_GAMEPAD "Viewport", &state, flags))
		{
			m_ViewportPosition.x = ImGui::GetCursorScreenPos().x;
			m_ViewportPosition.y = ImGui::GetCursorScreenPos().y;
			m_ViewportSize.x = ImGui::GetContentRegionAvail().x;
			m_ViewportSize.y = ImGui::GetContentRegionAvail().y;

			m_ViewportHovered = ImGui::IsWindowHovered();
			m_ViewportFocused = ImGui::IsWindowFocused();

			ImGui::Image((void*)m_ViewportFramebuffer->GetTexID(), { m_ViewportSize.x, m_ViewportSize.y }, { 0, 1 }, { 1, 0 });

			Entity entity = m_HierarchyView.GetSelectedEntity();
			if (entity && !m_IsPlaying)
			{
				//Gizmo Selector Window
				ImGuiStyle& style = ImGui::GetStyle();
				ImGuiWindowFlags childFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
				float buttonSize = 25.0f;
				const auto& currentWinPos = ImGui::GetWindowPos();
				ImGui::SetNextWindowSize({ buttonSize + style.FramePadding.x * 2.0f, (buttonSize * 2) * 3 });

				float windowBorder = style.WindowBorderSize;
				style.WindowBorderSize = 0.0f;

				ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.0f, 0.0f, 0.0f, 0.0f });
				if (ImGui::Begin("GizmoWindow", &m_IsViewportActive, childFlags))
				{

					ImGui::SetWindowPos({ currentWinPos.x + ImGui::GetStyle().FramePadding.x * 2.0f, currentWinPos.y + (buttonSize * 4.0f) / 2.0f });

					if (GizmoSelectButton(m_SelectTexture, buttonSize, buttonSize, GIZMOSELECTION::NONE == m_GizmoSelection))
						m_GizmoSelection = GIZMOSELECTION::NONE;

					if (GizmoSelectButton(m_TranslateTexture, buttonSize, buttonSize, GIZMOSELECTION::TRANSLATE == m_GizmoSelection))
						m_GizmoSelection = GIZMOSELECTION::TRANSLATE;

					if (GizmoSelectButton(m_RotateTexture, buttonSize, buttonSize, GIZMOSELECTION::RORTATE == m_GizmoSelection))
						m_GizmoSelection = GIZMOSELECTION::RORTATE;

					if (GizmoSelectButton(m_ResizeTexture, buttonSize, buttonSize, GIZMOSELECTION::SCALE == m_GizmoSelection))
						m_GizmoSelection = GIZMOSELECTION::SCALE;

					style.WindowBorderSize = windowBorder;

					ImGui::End();
				}
				ImGui::PopStyleColor();
				//End Gizmo Selector Window

				if (m_GizmoSelection != GIZMOSELECTION::NONE)
				{
					const glm::mat4& view = m_EditorCamera->GetView();
					const glm::mat4& projection = m_EditorCamera->GetProjection();

					TransformComponent& component = entity.GetComponent<TransformComponent>();
					glm::mat4 rotation = glm::toMat4(glm::quat(component.Rotation));
					glm::mat4 transform = glm::translate(glm::mat4(1.0f), component.Position) * rotation * glm::scale(glm::mat4(1.0f), component.Size);

					ImGuizmo::SetRect(m_ViewportPosition.x, m_ViewportPosition.y, m_ViewportSize.x, m_ViewportSize.y);
					ImGuizmo::SetOrthographic(true);
					ImGuizmo::SetDrawlist();
					ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), (ImGuizmo::OPERATION)m_GizmoSelection, ImGuizmo::LOCAL, glm::value_ptr(transform), NULL, m_IsSnapping ? &m_SnapAmount : NULL);

					if (ImGuizmo::IsUsing())
					{
						glm::vec3 finalPos, finalRot, finalSiz;
						Maths::DecomposeTransform(transform, finalPos, finalRot, finalSiz);
						glm::vec3 deltaRotation = finalRot - component.Rotation;
						component.Position = finalPos;
						component.Rotation += deltaRotation;
						component.Size = finalSiz;
					}
				}
			}
		}
		ImGui::End();
		ImGui::PopStyleVar();
		ImGuiLayer::ViewportPosition = m_ViewportPosition;
		ImGuiLayer::ViewportSize = m_ViewportSize;
		ImGuiLayer::CameraProjection = m_EditorCamera->GetViewProjection();
	}

	void EditorLayer::Menubar()
	{
		saveDialog.Display();
		loadDialog.Display();

		if (saveDialog.HasSelected())
		{
			//Save Scene
			Serializer serializer{ m_Scene };
			serializer.Serialize(saveDialog.GetSelected().string() + "/" + m_Scene->SceneName.c_str() + ".moon");
			saveDialog.ClearSelected();
		}

		if (loadDialog.HasSelected())
		{
			//Load Scene
			m_Scene = CreateRef<Scene>();
			m_IsPlaying = false;
			OnStop();
			m_HierarchyView.SetScene(m_Scene);
			Serializer serializer{ m_Scene };
			serializer.Deserialize(loadDialog.GetSelected().string());
			loadDialog.ClearSelected();
		}

		ImGui::BeginMainMenuBar();
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Scene", " "))
				{
					m_Scene = CreateRef<Scene>();
					m_HierarchyView.SetScene(m_Scene);
				}

				if (ImGui::MenuItem("Save", " "))
					saveDialog.Open();

				if (ImGui::MenuItem("Load", " "))
					loadDialog.Open();

				ImGui::EndMenu();
			}

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

				if (ImGui::MenuItem("Content Browser", " ", m_IsContentActive, true))
					m_IsContentActive = !m_IsContentActive;

				if (ImGui::MenuItem("Debug", " ", m_IsDebugActive, true))
					m_IsDebugActive = !m_IsDebugActive;
#ifdef ENGINE_DEBUG
				if (ImGui::MenuItem("Demo Window", " ", demoWindow, true))
					demoWindow = !demoWindow;
#endif // ENGINE_DEBUG
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
				float buttonSize = height / 2.0f;

				if (GizmoSelectButton(m_SelectTexture, buttonSize, height / 2.0f, GIZMOSELECTION::NONE == m_GizmoSelection))
					m_GizmoSelection = GIZMOSELECTION::NONE;

				if (GizmoSelectButton(m_TranslateTexture, buttonSize, height / 2.0f, GIZMOSELECTION::TRANSLATE == m_GizmoSelection))
					m_GizmoSelection = GIZMOSELECTION::TRANSLATE;

				if (GizmoSelectButton(m_RotateTexture, buttonSize, height / 2.0f, GIZMOSELECTION::RORTATE == m_GizmoSelection))
					m_GizmoSelection = GIZMOSELECTION::RORTATE;

				if (GizmoSelectButton(m_ResizeTexture, buttonSize, height / 2.0f, GIZMOSELECTION::SCALE == m_GizmoSelection))
					m_GizmoSelection = GIZMOSELECTION::SCALE;

				ImGuiUtils::AddPadding((ImGui::GetContentRegionAvail().x / 2.0f) - (buttonSize / 2.0f), 0.0f);
				Ref<Texture> icon = m_IsPlaying ? m_StopTexture : m_PlayTexture;
				if (ImGui::ImageButton((ImTextureID)icon->GetID(), { buttonSize, height / 2.0f }))
				{
					m_IsPlaying = !m_IsPlaying;
					if (m_IsPlaying)
						OnPlay();
					else
						OnStop();
				}

				float sceneNameTextSize = 200.0f;
				static char sceneName[255];
				ImGuiUtils::AddPadding(ImGui::GetContentRegionAvail().x - sceneNameTextSize , 0.0f);
				if (ImGui::InputTextWithHint("##SceneName", "Scene Name", sceneName, 255, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					m_Scene->SceneName = sceneName;
					memset(sceneName, 0, 255);
				}
				ImGui::EndMenuBar();
			}
		}
		ImGui::End();
	}

	void EditorLayer::Statusbar()
	{
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
		float height = ImGui::GetFrameHeight();

		if (ImGui::BeginViewportSideBar("##Statusbar", NULL, ImGuiDir_Down, height, window_flags))
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::Text(m_IsPlaying ? "Play Mode" : "Edit Mode");
				std::string sceneName = "Active Scene: " + m_Scene->SceneName;
				auto& itemWidth = ImGui::CalcTextSize(sceneName.c_str());
				ImGuiUtils::AddPadding(ImGui::GetContentRegionAvail().x - itemWidth.x - ImGui::GetStyle().FramePadding.x, 0.0f);
				ImGui::Text("Active Scene: %s", m_Scene->SceneName.c_str());
				ImGui::EndMenuBar();
			}
		}
		ImGui::End();
	}

	bool EditorLayer::GizmoSelectButton(Ref<Texture> texture, float width, float height, bool selected)
	{
		if (selected)
			ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);

		bool returnValue = ImGuiUtils::ImageButton((ImTextureID)texture->GetID(), { width, height });

		if (selected)
			ImGui::PopStyleColor();

		return returnValue;
	}

	void EditorLayer::DebugView(bool& state)
	{
		ImGui::Begin(ICON_FK_CODE "Debug", &state);
		ImGui::Text("FPS: %.1f FPS (%.2f ms/frame) ", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("Drawcalls: %d", Renderer::GetRenderData().DrawCalls);
		ImGui::Text("Mouse X: %.1f, Mouse Y: %.1f", Input::GetX(), Input::GetY());
		ImGui::Text("Ortho X: %.1f, Ortho Y: %.1f", Input::OrthoX(), Input::OrthoY());
		ImGui::End();
	}

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

	void EditorLayer::Destroy()
	{
		delete m_EditorCamera;
	}
}