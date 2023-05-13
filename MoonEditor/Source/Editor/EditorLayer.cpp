#include "mpch.h"
#include "EditorLayer.h"
#include "Editor/EditorAssets.h"

#include <MoonEngine.h>
#include <Gui/ImGuiUtils.h>
#include <Gui/ImFileBrowser.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <imguizmo/ImGuizmo.h>

namespace MoonEngine
{
	ImFont* font16;
	ImFont* font24;

	ImGui::ImGuiFileBrowser loadDialog;
	ImGui::ImGuiFileBrowser saveDialog;

	void HandleDpiChange(float scale)
	{
		auto& io = ImGui::GetIO();
		if (scale >= 1.5f)
			io.FontDefault = font24;
		else
			io.FontDefault = font16;
	}

	void EditorLayer::Init()
	{
		s_EditorLayer = this;

		loadDialog = ImGuiFileBrowserConfig
		{
			.Label = "Load Scene",
			.Flags = ImGuiFileBrowserFlags_CloseWithEsc | ImGuiFileBrowserFlags_SelectFiles | ImGuiFileBrowserFlags_DoubleClickOkay
			| ImGuiFileBrowserFlags_ResetSelected | ImGuiFileBrowserFlags_SelectDirectories | ImGuiFileBrowserFlags_AllowRename | ImGuiFileBrowserFlags_AllowDelete,
			.StartDirectory = "Resource/Assets/Scenes",
			.RootDirectory = "Resource/Assets"
		};

		saveDialog = ImGuiFileBrowserConfig
		{
			.Label = "Save Scene",
			.Flags = ImGuiFileBrowserFlags_CloseWithEsc | ImGuiFileBrowserFlags_AllowCreateFolder | ImGuiFileBrowserFlags_ResetSelected |
			 ImGuiFileBrowserFlags_AllowDelete | ImGuiFileBrowserFlags_OkayTriggersEnter | ImGuiFileBrowserFlags_SelectFiles | ImGuiFileBrowserFlags_DoneAfterCreateFile,
			.StartDirectory = "Resource/Assets/Scenes",
			.RootDirectory = "Resource/Assets"
		};

		m_AssetsView = MakeShared<AssetsView>("Resource/Assets");

		//+ Events Initilization
		font16 = ImGuiUtils::AddFont("Resource/Fonts/Roboto/Roboto-Medium.ttf", 16.0f, false);
		ImGuiUtils::AddIconFont("Resource/Fonts/material-design-icons/MaterialIcons-Regular.ttf", 16.0f, (ImWchar)ICON_MIN_MD, (ImWchar)ICON_MAX_MD);
		font24 = ImGuiUtils::AddFont("Resource/Fonts/Roboto/Roboto-Medium.ttf", 24.0f);
		ImGuiUtils::AddIconFont("Resource/Fonts/material-design-icons/MaterialIcons-Regular.ttf", 24.0f, (ImWchar)ICON_MIN_MD, (ImWchar)ICON_MAX_MD);

		WindowPrefs& prefs = Application::GetWindowPrefs();
		HandleDpiChange(prefs.Dpi);

		Input::OnKeyPress += BIND_ACTION(EditorLayer::KeyEvents);
		Window::OnDpiChange += HandleDpiChange;
		//- Events Initilization

		m_ViewportView = MakeShared<ViewportView>();
		m_GameView = MakeShared<GameView>();
		m_InspectroView = MakeShared<InspectorView>();
		m_HierarchyView = MakeShared<HierarchyView>();

		NewScene();
	}

	void EditorLayer::KeyEvents(KeyPressEvent& e)
	{
		if (e.IsRepeat() || ImGui::GetIO().WantTextInput)
			return;

		bool control = Input::GetKey(Keycode::LeftControl);
		bool shift = Input::GetKey(Keycode::LeftShift);

		switch (e.Key())
		{
			case Keycode::Delete:
			{
				DestroySelectedEntity();
				break;
			}
			case Keycode::D:
			{
				if (control)
					DuplicateSelectedEntity();
				break;
			}
			case Keycode::N:
			{
				if (control)
					NewScene();
				break;
			}
			case Keycode::S:
			{
				if (control && m_EditorState == EditorState::Edit)
					QuickSave();
				break;
			}
			case Keycode::L:
			{
				if (control)
					loadDialog.OpenFileBrowser();
				break;
			}
		}
		if (m_ViewportView->ViewFocused)
			m_ViewportView->OnKeyEvent(e.Key());
	}

	void EditorLayer::OnSceneChange()
	{
		m_SelectedEntity = {};

		m_HierarchyView->Scene = m_Scene.get();

		m_ViewportView->Scene = m_Scene.get();
		m_GameView->Scene = m_Scene.get();
	}

	void EditorLayer::NewScene()
	{
		if (m_EditorState == EditorState::Play)
			m_Scene->StopRuntime();

		m_EditorState = EditorState::Edit;
		m_EditorScene = MakeShared<Scene>();
		m_Scene = m_EditorScene;
		m_Scene->StartEdit();
		m_ScenePath.clear();
		OnSceneChange();
	}

	void EditorLayer::SaveScene(const std::string& path)
	{
		SceneSerializer::Serialize(m_Scene, path);
	}

	void EditorLayer::LoadScene(const std::string& path)
	{
		NewScene();
		SceneSerializer::Deserialize(m_Scene, path);
	}

	void EditorLayer::QuickSave()
	{
		if (m_ScenePath == "")
			saveDialog.OpenFileBrowser();
		else
		{
			if ((m_Scene->SceneName + ".moonscn") != m_ScenePath.filename())
				saveDialog.OpenFileBrowser();
			else
				SaveScene(m_ScenePath.string());
		}
	}

	void EditorLayer::Update()
	{
		m_Scene->UpdateRuntime(EditorLayer::State() == EditorLayer::EditorState::Play);
		m_ViewportView->Update();
		m_GameView->Update();
	}

	void EditorLayer::DrawGui()
	{
		Dockspace();
		Menubar();
		Debug();

		loadDialog.BeginFileBrowser();
		if (loadDialog.IsDone())
		{
			const std::string& path = loadDialog.GetSelected().string();
			if (path != "")
			{
				LoadScene(path);
				m_ScenePath = path;
			}
		}

		saveDialog.BeginFileBrowser();
		if (saveDialog.IsDone())
		{
			const auto& path = saveDialog.GetCurrentPath();
			std::string pathstr = saveDialog.GetCurrentPath().string();

			if (pathstr != "")
			{
				pathstr += "/" + m_Scene->SceneName + ".moonscn";
				SaveScene(pathstr);
				m_ScenePath = pathstr;
			}
		}

		m_ViewportView->Render();
		m_GameView->Render();
		m_AssetsView->Render();
		m_HierarchyView->Render();
		m_InspectroView->Render();

		Statusbar();
		ImGui::End();
	}

	void EditorLayer::Debug()
	{
		if (!m_ShowDebug)
			return;

		static bool showDemoWindow = false;

		ImGui::Begin(ICON_MD_BUG_REPORT "Debug", &m_ShowDebug);
		ImGui::Text("FPS: %.1f FPS (%.2f ms/frame) ", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Checkbox("Demo Window", &showDemoWindow);

		if (showDemoWindow)
			ImGui::ShowDemoWindow();

		ImGui::Separator();
		ImGuiUtils::AddPadding(0.0f, 10.0f);

		if (m_Scene)
		{
			ImGui::Text("Scene Name: %s", m_Scene->SceneName.c_str());
			ImGui::Text("Scene Path: %s", m_ScenePath.string().c_str());
			ImGui::Text("Scene Filename: %s", m_ScenePath.filename().string().c_str());

			ImGui::Separator();
			ImGuiUtils::AddPadding(0.0f, 10.0f);
		}

		const auto& renderStats = Renderer::GetStats();
		ImGui::Text("Viewport Renderer Data");
		ImGui::Text("Draw Calls: %d", renderStats.DrawCalls);
		//ImGui::Text("Vertex Count: %d", renderStats.VertexCount);
		//ImGui::Text("Quad Count: %d", renderStats.QuadCount);

		ImGui::Separator();
		ImGuiUtils::AddPadding(0.0f, 10.0f);

		WindowPrefs& prefs = Application::GetWindowPrefs();
		ImGui::Text("Application Prefs");
		ImGui::Text("Vsync: %s", prefs.VsyncOn ? "On" : "Off");
		if (ImGui::Button("Set Vsync"))
			Application::SetVsync(!prefs.VsyncOn);

		ImGui::Text("Resolution x: %d y: %d", prefs.Resolution.Width, prefs.Resolution.Height);
		ImGui::Text("Resolution Dpi: %.2f", prefs.Dpi);

		ImGui::End();
	}

	void EditorLayer::Menubar()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		float borderSize = style.WindowBorderSize;
		style.WindowBorderSize = 0;
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));

		ImGui::BeginMainMenuBar();
		{
			if (ImGui::BeginMenu("File", true))
			{
				if (ImGui::MenuItem("New Scene", "Ctrl+N", nullptr, true))
					NewScene();

				if (ImGui::MenuItem("Save", "Ctrl+S", nullptr, true))
					QuickSave();

				if (ImGui::MenuItem("Load", "Ctrl+L ", nullptr, true))
					loadDialog.OpenFileBrowser();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit", true))
			{
				//if (ImGui::MenuItem("Editor Settings", " ", m_ShowEditorSettingsView, true))
				//	m_ShowEditorSettingsView = !m_ShowEditorSettingsView;

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Views", true))
			{
				m_ViewportView->GetMenuItem();
				m_GameView->GetMenuItem();
				m_AssetsView->GetMenuItem();
				m_HierarchyView->GetMenuItem();
				m_InspectroView->GetMenuItem();

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		SubMenubar();
		ImGui::PopStyleColor();
		style.WindowBorderSize = borderSize;
	}

	void EditorLayer::SubMenubar()
	{
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoNav;
		float height = ImGui::GetFrameHeight();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 6, 6 });

		if (ImGui::BeginViewportSideBar("##SubMenubar", NULL, ImGuiDir_Up, height, window_flags))
		{
			if (ImGui::BeginMenuBar())
			{
				float buttonSize = height * 0.5f;

				bool isEditing = m_EditorState == EditorState::Edit;
				Shared<Texture> icon = isEditing ? EditorAssets::PlayTexture : EditorAssets::PauseTexture;

				ImGuiUtils::AddPadding((ImGui::GetContentRegionAvail().x * 0.5f) - (buttonSize * 0.5f), 0.0f);

				if (ImGuiUtils::ButtonSelectable((ImTextureID)icon->GetTextureId(), buttonSize, buttonSize, m_EditorState == EditorState::Pause))
				{
					if (m_EditorState == EditorState::Edit)
					{
						m_EditorState = EditorState::Play;
						m_Scene->StopEdit();
						m_Scene = Scene::CopyScene(m_EditorScene);
						m_Scene->StartRuntime();
						OnSceneChange();
					}
					else if (m_EditorState == EditorState::Play)
						m_EditorState = EditorState::Pause;
					else if (m_EditorState == EditorState::Pause)
						m_EditorState = EditorState::Play;
				}

				if (m_EditorState != EditorState::Edit && ImGui::ImageButton((ImTextureID)EditorAssets::StopTexture->GetTextureId(), { buttonSize, height * 0.5f }))
				{
					m_EditorState = EditorState::Edit;
					m_Scene->StopRuntime();
					m_Scene = m_EditorScene;
					m_Scene->StartEdit();
					OnSceneChange();
				}
				ImGui::EndMenuBar();
			}
		}
		ImGui::End();

		ImGui::PopStyleVar();
	}

	void EditorLayer::Statusbar()
	{
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoNav;
		float height = ImGui::GetFrameHeight();

		if (ImGui::BeginViewportSideBar("##Statusbar", NULL, ImGuiDir_Down, height, window_flags))
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::Text(m_EditorState == EditorState::Edit ? "Edit Mode" : "Play Mode");
				const std::string& sceneName = "Active Scene: " + m_Scene->SceneName;
				const auto& itemWidth = ImGui::CalcTextSize(sceneName.c_str());
				ImGuiUtils::AddPadding(ImGui::GetContentRegionAvail().x - itemWidth.x - ImGui::GetStyle().FramePadding.x, 0.0f);
				ImGui::Text("Active Scene: %s", m_Scene->SceneName.c_str());
				ImGui::EndMenuBar();
			}
		}
		ImGui::End();
	}

	void EditorLayer::Dockspace()
	{
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoNav;
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