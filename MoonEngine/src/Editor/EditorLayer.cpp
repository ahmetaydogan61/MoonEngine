#include "EditorLayer.h"
#include "ImGuiUtils.h"
#include "Utils/IconsFontAwesome.h"
#include "ImGuizmo.h"

#include <imgui_internal.h>
#include "imfilebrowser.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace MoonEngine
{
	bool demoWindow = false;

	ImGui::FileBrowser saveDialog{ ImGuiFileBrowserFlags_SelectDirectory | ImGuiFileBrowserFlags_CreateNewDir };
	ImGui::FileBrowser loadDialog{};

	void EditorLayer::Create()
	{
		LayerName = "Editor Layer";
		
		m_EditorScene = CreateRef<Scene>();
		m_Scene = m_EditorScene;

		ResourceManagerDesc desc
		{
			m_EditorScene,
			"Assets",
			"res/Assets"
		};
		ResourceManager::SetResourceManager(desc);

		m_ViewportFramebuffer = CreateRef<Framebuffer>();
		m_HierarchyView.SetScene(m_Scene);
		m_ContentView.SetDirectoryPath(ResourceManager::GetAssetPath());
		m_EditorSettingsView.SetStyle(EditorStyle::Dark);
		
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
		saveDialog.SetPwd(ResourceManager::GetAssetPath());

		loadDialog.SetTitle("Load Scene");
		loadDialog.SetTypeFilters({ ".moon" });
		loadDialog.SetPwd(ResourceManager::GetAssetPath());
	}

	void EditorLayer::OnEvent(Event& event)
	{
		m_EditorCamera.OnEvent(event);

		EventHandler handler = { event };
		handler.HandleEvent(EventType::KeyPress, EVENT_FN_POINTER(KeyEvents));
	}

	bool EditorLayer::KeyEvents(Event& event)
	{
		KeyPressEvent& e = (KeyPressEvent&)event;

		if (e.Repeat())
			return false;

		bool control = Input::GetKey(KEY_LEFT_CONTROL);
		bool shift = Input::GetKey(KEY_LEFT_SHIFT);

		bool canPress = !m_IsPlaying && !ImGui::IsAnyItemActive() && !shift ;

		switch (e.Key())
		{
			case KEY_DELETE:
			{
				m_HierarchyView.DeleteSelectedEntity();
				break;
			}
			case KEY_Q:
			{
				if (canPress)
					m_GizmoSelection = GIZMOSELECTION::NONE;
				break;
			}
			case KEY_W:
			{
				if (!control && canPress)
					m_GizmoSelection = GIZMOSELECTION::TRANSLATE;
				else if (control)
					m_HierarchyView.DeleteSelectedEntity();
				break;
			}
			case KEY_E:
			{
				if (canPress)
					m_GizmoSelection = GIZMOSELECTION::SCALE;
				break;
			}
			case KEY_R:
			{
				if (canPress)
					m_GizmoSelection = GIZMOSELECTION::RORTATE;
				break;
			}
			case KEY_D:
			{
				if (control)
					m_HierarchyView.DuplicateSelectedEntity();
				break;
			}
			case KEY_N:
			{
				if (control)
					NewScene();
				break;
			}
			case KEY_S:
			{
				if (control)
					if (m_ScenePath.empty())
						saveDialog.Open();
					else
						SaveScene(m_ScenePath);
				break;
			}
			case KEY_L:
			{
				if (control)
					loadDialog.Open();
				break;
			}
			default:
				break;
		}
	}

	void EditorLayer::Update()
	{
		if (m_ViewportSize.x != m_ViewportFramebuffer->GetWidth() || m_ViewportSize.y != m_ViewportFramebuffer->GetHeight())
		{
			m_ViewportFramebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_EditorCamera.Resize(m_ViewportSize.x, m_ViewportSize.y);
		}

		if (!m_IsPlaying)
		{
			if (Input::MousePressed(0) && m_ViewportHovered && !ImGuizmo::IsUsing())
				m_HierarchyView.MouseSelect();

			if (m_ViewportFocused && !ImGui::IsAnyItemActive())
				m_EditorCamera.UpdateFocused();

			if (m_ViewportHovered && !ImGui::IsAnyItemActive())
				m_EditorCamera.UpdateHovered();

			m_EditorCamera.Update();
			m_IsSnapping = Input::GetKey(KEY_LEFT_CONTROL);
		}
		else
			m_Scene->ResizeViewport(m_ViewportSize.x, m_ViewportSize.y);

		m_ViewportFramebuffer->Bind();
		m_IsPlaying ? m_Scene->UpdateRuntime() : m_Scene->UpdateEditor(&m_EditorCamera, m_HierarchyView.GetSelectedEntity());
		m_ViewportFramebuffer->Unbind();
	}

	void EditorLayer::OnPlay()
	{
		m_IsPlaying = true;
		m_Scene = Scene::CopyScene(m_EditorScene);
		m_Scene->OnEditorStop();
		m_Scene->OnRuntimeStart();
		m_HierarchyView.SetScene(m_Scene);
	}

	void EditorLayer::OnStop()
	{
		m_IsPlaying = false;
		m_Scene->OnRuntimeStop();
		m_Scene->OnEditorStart();
		m_Scene = m_EditorScene;
		m_HierarchyView.SetScene(m_Scene);
	}

	void EditorLayer::NewScene()
	{
		m_IsPlaying = false;
		m_EditorScene = CreateRef<Scene>();
		m_Scene = m_EditorScene;
		m_HierarchyView.SetScene(m_Scene);
		m_ScenePath.clear();
		ResourceManager::OnSceneChange(m_EditorScene);
	}

	void EditorLayer::SaveScene(const std::string& path)
	{
		Serializer serializer{ m_Scene };
		serializer.Serialize(path);
		m_ScenePath = path;
	}

	void EditorLayer::LoadScene(const std::string& path)
	{
		if (Serializer::IsValid(path, "moon"))
		{
			NewScene();
			Serializer serializer{ m_Scene };
			serializer.Deserialize(path);
			m_ScenePath = path;
		}
	}

	void EditorLayer::DrawGUI()
	{
		Dockspace(); //+Dockspace

		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
		
		ImGuiStyle& style = ImGui::GetStyle();
		float borderSize = style.WindowBorderSize;
		style.WindowBorderSize = 0;
		Menubar();
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 6, 6 });
		Sidemenubar();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
		style.WindowBorderSize = borderSize;

		if (demoWindow) //Debug Only
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
		ImGui::End(); //-Dockspace
	}

	void EditorLayer::ViewportView(bool& state)
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGuiIO& io = ImGui::GetIO();

		if (ImGui::Begin(ICON_FK_GAMEPAD "Viewport", &state, flags))
		{
			m_ViewportPosition.x = ImGui::GetCursorScreenPos().x;
			m_ViewportPosition.y = ImGui::GetCursorScreenPos().y;
			
			m_ViewportSize.x = ImGui::GetContentRegionAvail().x;
			m_ViewportSize.y = ImGui::GetContentRegionAvail().y;

			m_ViewportHovered = ImGui::IsWindowHovered();
			m_ViewportFocused = ImGui::IsWindowFocused();

			if (!m_IsPlaying)
				ImGuiLayer::BlockEvent(!m_ViewportHovered);
			else
				ImGuiLayer::BlockEvent(true);

			ImGui::Image((void*)m_ViewportFramebuffer->GetTexID(), { m_ViewportSize.x, m_ViewportSize.y }, { 0, 1 }, { 1, 0 });

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MNE_AssetItem"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path scenePath = std::filesystem::path("res/Assets") / path;
					LoadScene(scenePath.string());
				}
				ImGui::EndDragDropTarget();
			}

			Entity entity = m_HierarchyView.GetSelectedEntity();
			if (entity && !m_IsPlaying)
			{
				//+GizmoSelectorWindow
				ImGuiStyle& style = ImGui::GetStyle();
				ImGuiWindowFlags childFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
				float buttonSize = 25.0f;
				auto& currentWinPos = ImGui::GetWindowPos();
				ImGui::SetNextWindowSize({ buttonSize + style.FramePadding.x * 2.0f, (buttonSize * 2) * 3 });

				float windowBorder = style.WindowBorderSize;
				style.WindowBorderSize = 0.0f;

				ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.0f, 0.0f, 0.0f, 0.0f });
				if (ImGui::Begin("GizmoWindow", &m_IsViewportActive, childFlags))
				{
					ImGui::SetWindowPos({ currentWinPos.x + ImGui::GetStyle().FramePadding.x * 2.0f, currentWinPos.y + (buttonSize * 4.0f) / 2.0f });

					if (ImGuiUtils::ButtonSelectable((ImTextureID)m_SelectTexture->GetID(), buttonSize, buttonSize, GIZMOSELECTION::NONE == m_GizmoSelection))
						m_GizmoSelection = GIZMOSELECTION::NONE;

					if (ImGuiUtils::ButtonSelectable((ImTextureID)m_TranslateTexture->GetID(), buttonSize, buttonSize, GIZMOSELECTION::TRANSLATE == m_GizmoSelection))
						m_GizmoSelection = GIZMOSELECTION::TRANSLATE;

					if (ImGuiUtils::ButtonSelectable((ImTextureID)m_ResizeTexture->GetID(), buttonSize, buttonSize, GIZMOSELECTION::SCALE == m_GizmoSelection))
						m_GizmoSelection = GIZMOSELECTION::SCALE;

					if (ImGuiUtils::ButtonSelectable((ImTextureID)m_RotateTexture->GetID(), buttonSize, buttonSize, GIZMOSELECTION::RORTATE == m_GizmoSelection))
						m_GizmoSelection = GIZMOSELECTION::RORTATE;

					style.WindowBorderSize = windowBorder;

					ImGui::End();
				}
				ImGui::PopStyleColor();
				//-GizmoSelectorWindow

				//+ImGuizmo
				if (m_GizmoSelection != GIZMOSELECTION::NONE)
				{
					const glm::mat4& view = m_EditorCamera.GetView();
					const glm::mat4& projection = m_EditorCamera.GetProjection();

					TransformComponent& component = entity.GetComponent<TransformComponent>();
					glm::mat4 rotation = glm::toMat4(glm::quat(component.Rotation));
					glm::mat4 transform = glm::translate(glm::mat4(1.0f), component.Position) * rotation * glm::scale(glm::mat4(1.0f), component.Size);

					ImGuizmo::SetRect(m_ViewportPosition.x, m_ViewportPosition.y, m_ViewportSize.x, m_ViewportSize.y);
					ImGuizmo::SetOrthographic(true);
					ImGuizmo::SetDrawlist();

					float snapAmount = 0;

					if (m_GizmoSelection != GIZMOSELECTION::RORTATE)
						snapAmount = m_SnapAmount;
					else
						snapAmount = 45.0f;

					ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), (ImGuizmo::OPERATION)m_GizmoSelection, ImGuizmo::LOCAL, glm::value_ptr(transform), NULL, m_IsSnapping ? &snapAmount : NULL);

					if (ImGuizmo::IsUsing())
					{
						glm::vec3 finalPos, finalRot, finalSiz;
						Maths::DecomposeTransform(transform, finalPos, finalRot, finalSiz);
						glm::vec3 deltaRotation = finalRot - component.Rotation;
						component.Position = finalPos;
						component.Rotation += deltaRotation;
						component.Size = finalSiz;
					}
				}//-ImGuizmo
			}
		}
		ImGui::End();
		ImGui::PopStyleVar();
		ImGuiLayer::ViewportPosition = m_ViewportPosition;
		ImGuiLayer::ViewportSize = m_ViewportSize;
	}

	void EditorLayer::Menubar()
	{
		saveDialog.Display();
		loadDialog.Display();

		if (saveDialog.HasSelected())
		{
			//Save Scene
			SaveScene(saveDialog.GetSelected().string() + "/" + m_Scene->SceneName.c_str() + ".moon");
			saveDialog.ClearSelected();
		}

		if (loadDialog.HasSelected())
		{
			//Load Scene
			LoadScene(loadDialog.GetSelected().string());
			loadDialog.ClearSelected();
		}

		ImGui::BeginMainMenuBar();
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Scene", "Ctrl + N"))
					NewScene();

				if (ImGui::MenuItem("Save Scene", "Ctrl + S"))
					saveDialog.Open();

				if (ImGui::MenuItem("Load Scene", "Ctrl + L"))
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

				if (ImGuiUtils::ButtonSelectable((ImTextureID)m_SelectTexture->GetID(), buttonSize, height / 2.0f, GIZMOSELECTION::NONE == m_GizmoSelection))
					m_GizmoSelection = GIZMOSELECTION::NONE;

				if (ImGuiUtils::ButtonSelectable((ImTextureID)m_TranslateTexture->GetID(), buttonSize, height / 2.0f, GIZMOSELECTION::TRANSLATE == m_GizmoSelection))
					m_GizmoSelection = GIZMOSELECTION::TRANSLATE;

				if (ImGuiUtils::ButtonSelectable((ImTextureID)m_ResizeTexture->GetID(), buttonSize, height / 2.0f, GIZMOSELECTION::SCALE == m_GizmoSelection))
					m_GizmoSelection = GIZMOSELECTION::SCALE;

				if (ImGuiUtils::ButtonSelectable((ImTextureID)m_RotateTexture->GetID(), buttonSize, height / 2.0f, GIZMOSELECTION::RORTATE == m_GizmoSelection))
					m_GizmoSelection = GIZMOSELECTION::RORTATE;

				ImGuiUtils::AddPadding((ImGui::GetContentRegionAvail().x / 2.0f) - (buttonSize / 2.0f), 0.0f);
				Ref<Texture> icon = m_IsPlaying ? m_StopTexture : m_PlayTexture;
				if (ImGui::ImageButton((ImTextureID)icon->GetID(), { buttonSize, height / 2.0f }))
				{
					if (m_IsPlaying)
						OnStop();
					else
						OnPlay();
				}

				float sceneNameTextSize = 200.0f;
				static char sceneName[255];
				ImGuiUtils::AddPadding(ImGui::GetContentRegionAvail().x - sceneNameTextSize, 0.0f);
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

	void EditorLayer::DebugView(bool& state)
	{
		ImGui::Begin(ICON_FK_CODE "Debug", &state);
		ImGui::Text("FPS: %.1f FPS (%.2f ms/frame) ", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("Drawcalls: %d", Renderer::GetRenderData().DrawCalls);
		ImGui::Text("QuadCounts: %d", Renderer::GetRenderData().QuadCount);
		
		ImGuiUtils::SeparatorDistanced(5.0f);

		ImGui::Text("Mouse X: %.1f, Mouse Y: %.1f", Input::GetX(), Input::GetY());
		ImGui::Text("Ortho X: %.1f, Ortho Y: %.1f", Input::OrthoX(), Input::OrthoY());
		ImGui::Text("Viewport Hovered %d", m_ViewportHovered);
		ImGui::Text("Viewport Focused %d", m_ViewportFocused);

		ImGuiUtils::SeparatorDistanced(5.0f);

		ImGui::Text("Texture Assets Count: %d", ResourceManager::TextureCount());

		auto& map = ResourceManager::GetMap();
		for (auto element : map)
			ImGui::Text(element.first.c_str());

		ImGuiUtils::SeparatorDistanced(5.0f);
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
}