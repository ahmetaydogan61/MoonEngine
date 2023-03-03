#include "mpch.h"
#include "EditorLayer.h"

#include <MoonEngine.h>
#include <Gui/ImGuiUtils.h>
#include <Gui/ImFileBrowser.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <imguizmo/ImGuizmo.h>

namespace MoonEngine
{
	Shared<Texture> cameraTexture;

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
			.Flags = ImGuiFileBrowserFlags_CloseWithEsc | ImGuiFileBrowserFlags_AllowCreateFolder | ImGuiFileBrowserFlags_ResetSelected
			 | ImGuiFileBrowserFlags_CreateFile | ImGuiFileBrowserFlags_AllowDelete | ImGuiFileBrowserFlags_SelectFiles | ImGuiFileBrowserFlags_DoneAfterCreateFile,
			.StartDirectory = "Resource/Assets/Scenes",
			.RootDirectory = "Resource/Assets"
		};

		m_PlayTexture = MakeShared<Texture>("Resource/EditorIcons/Play.png");
		m_StopTexture = MakeShared<Texture>("Resource/EditorIcons/Stop.png");
		m_PauseTexture = MakeShared<Texture>("Resource/EditorIcons/Pause.png");

		m_SelectTexture = MakeShared<Texture>("Resource/EditorIcons/Select.png");
		m_TranslateTexture = MakeShared<Texture>("Resource/EditorIcons/Translate.png");
		m_ResizeTexture = MakeShared<Texture>("Resource/EditorIcons/Resize.png");
		m_RotateTexture = MakeShared<Texture>("Resource/EditorIcons/Rotate.png");

		m_AssetsView = MakeShared<AssetsView>("Resource/Assets");

		//+ Events Initilization
		font16 = ImGuiUtils::AddFont("Resource/Fonts/Roboto/Roboto-Medium.ttf", 16.0f, false);
		ImGuiUtils::AddIconFont("Resource/Fonts/material-design-icons/MaterialIcons-Regular.ttf", 16.0f, (ImWchar)ICON_MIN_MD, (ImWchar)ICON_MAX_MD);
		font24 = ImGuiUtils::AddFont("Resource/Fonts/Roboto/Roboto-Medium.ttf", 24.0f);
		ImGuiUtils::AddIconFont("Resource/Fonts/material-design-icons/MaterialIcons-Regular.ttf", 24.0f, (ImWchar)ICON_MIN_MD, (ImWchar)ICON_MAX_MD);

		ApplicationPrefs& prefs = Application::GetPrefs();
		HandleDpiChange(prefs.Resolution.Dpi);

		Input::OnKeyPress += BIND_ACTION(EditorLayer::KeyEvents);
		Window::OnDpiChange += HandleDpiChange;
		//- Events Initilization

		//+Viewport Initilization
		{
			FramebufferProps props = { {FramebufferTextureFormat::RGBA8}, {FramebufferTextureFormat::RED_INTEGER}, {FramebufferTextureFormat::DEPTH} };
			m_ViewportFbo = MakeShared<Framebuffer>(props);
			m_EditorCameraController.SetCamera(&m_EditorCamera);
		}
		//-Viewport Initilization

		//+GameView Initiization
		{
			FramebufferProps props = { {FramebufferTextureFormat::RGBA8}, {FramebufferTextureFormat::DEPTH} };
			m_GameFbo = MakeShared<Framebuffer>(props);
		}
		//+GameView Initiization

		Renderer::Init();
		NewScene();

		cameraTexture = MakeShared<Texture>("Resource/EditorIcons/Camera.png");
	}

	void EditorLayer::Update()
	{
		//+Viewport Update
		if (m_ShowViewport)
		{
			const auto& viewportSize = m_ViewportData.ViewportSize;
			if (viewportSize.x != m_ViewportFbo->GetWidth() || viewportSize.y != m_ViewportFbo->GetHeight())
			{
				m_ViewportFbo->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
				m_EditorCamera.Resize(viewportSize.x / viewportSize.y);
			}

			m_EditorCameraController.Update(m_ViewportData.ViewportFocused, m_ViewportData.ViewportHovered);

			m_GizmosData.IsSnapping = Input::GetKey(Keycode::LeftControl);

			//+Render Viewport
			m_ViewportFbo->Bind();

			Renderer::SetRenderData(m_EditorCamera.GetViewProjection());
			Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f });
			Renderer::Begin();
			m_ViewportFbo->ClearColorAttachment(1, (void*)-1);

			auto view = m_Scene->m_Registry.view<const TransformComponent, const SpriteComponent>();
			for (auto [entity, transform, sprite] : view.each())
			{
				Renderer::DrawEntity(transform, sprite, (int)entity);
			}

			Renderer::End();

			Renderer::SetLineWidth(2.0f);

			auto cameraView = m_Scene->m_Registry.view<const TransformComponent, const CameraComponent>();
			for (auto [entity, transformComponent, cameraComponent] : cameraView.each())
			{
				const glm::mat4& rotationMat = glm::toMat4(glm::quat(transformComponent.Rotation));
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), transformComponent.Position)
					* rotationMat * glm::scale(glm::mat4(1.0f), glm::vec3(cameraComponent.Size * 2.0f, cameraComponent.Size * 2.0f, 0.0f));

				Renderer::DrawRect(transform, { 1.0f, 1.0f, 1.0f, 1.0f }, (int)entity);

				transform = glm::translate(glm::mat4(1.0f), transformComponent.Position)
					* glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.0f));

				Renderer::DrawEntity(transform, cameraTexture, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f }, (int)entity);
			}

			Renderer::End();

			Renderer::SetLineWidth(3.0f);

			if (m_SelectedEntity)
			{
				TransformComponent& transformComponent = m_SelectedEntity.GetComponent<TransformComponent>();
				const glm::mat4& rotationMat = glm::toMat4(glm::quat(transformComponent.Rotation));
				const glm::mat4& transform = glm::translate(glm::mat4(1.0f), transformComponent.Position)
					* rotationMat * glm::scale(glm::mat4(1.0f), transformComponent.Scale);

				if (!m_SelectedEntity.HasComponent<CameraComponent>())
					Renderer::DrawRect(transform, { 0.0f, 150.0f / 255.0f, 1.0f, 1.0f });
			}

			Renderer::End();

			//+MousePicking
			{
				if (m_ViewportData.ViewportFocused)
				{
					const auto& viewportPositon = m_ViewportData.ViewportPosition;
					ImVec2 mousePos = ImGui::GetMousePos();
					mousePos.x -= viewportPositon.x;
					mousePos.y -= viewportPositon.y;
					mousePos.y = viewportSize.y - mousePos.y;

					int mouseX = (int)mousePos.x;
					int mouseY = (int)mousePos.y;

					if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && (int)mouseY < viewportSize.y && Input::GetMouseButtonDown(0))
					{
						int pixelData = m_ViewportFbo->ReadPixel(1, mouseX, mouseY);
						if (!m_GizmosData.IsUsing)
							m_SelectedEntity = pixelData == -1 ? Entity{} : Entity{ (entt::entity)pixelData, m_Scene.get() };
					}
				}
			}//-MousePicking


			m_ViewportFbo->Unbind();
			//-Render Viewport
		}
		//-Viewport Update

		//+Gameview Update

		if (m_ShowGameView)
		{
			const auto& viewportSize = m_GameViewData.ViewportSize;
			if (viewportSize.x != m_GameFbo->GetWidth() || viewportSize.y != m_GameFbo->GetHeight())
			{
				m_GameFbo->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
			}

			CameraComponent* sceneCamera = nullptr;
			glm::vec3 cameraPosition = glm::vec3(0.0f);

			auto cameraView = m_Scene->m_Registry.view<CameraComponent, const TransformComponent>();
			cameraView.each([&](auto& camera, const auto& transform)
			{
				{
					if (camera.IsMain)
					{
						sceneCamera = &camera;
						cameraPosition = transform.Position;
						return;
					}
				}
			});

			glm::mat4 viewProjection;
			if (sceneCamera)
			{
				sceneCamera->Resize(viewportSize.x / viewportSize.y);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(cameraPosition.x, cameraPosition.y, 0.0f));
				glm::mat4 m_View = glm::inverse(transform);
				viewProjection = sceneCamera->GetProjection() * m_View;
			}

			//+Render Game
			m_GameFbo->Bind();

			Renderer::SetRenderData(viewProjection);
			Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f });
			Renderer::Begin();

			auto view = m_Scene->m_Registry.view<const TransformComponent, const SpriteComponent>();
			for (auto [entity, transform, sprite] : view.each())
			{
				Renderer::DrawEntity(transform, sprite, (int)entity);
			}

			Renderer::End();
			m_GameFbo->Unbind();
			//-Render Game
		}
		//Gameview Update
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
					saveDialog.OpenFileBrowser();
				break;
			}
			case Keycode::L:
			{
				if (control)
					loadDialog.OpenFileBrowser();
				break;
			}
			case Keycode::Q:
			{
				m_GizmosData.GizmoSelection = GIZMOSELECTION::NONE;
				break;
			}
			case Keycode::W:
			{
				m_GizmosData.GizmoSelection = GIZMOSELECTION::TRANSLATE;
				break;
			}
			case Keycode::E:
			{
				m_GizmosData.GizmoSelection = GIZMOSELECTION::SCALE;
				break;
			}
			case Keycode::R:
			{
				m_GizmosData.GizmoSelection = GIZMOSELECTION::RORTATE;
				break;
			}
		}
	}

	void EditorLayer::NewScene()
	{
		if (m_EditorState == EditorState::Play)
			m_Scene->StopPlay();

		m_EditorState = EditorState::Edit;
		m_EditorScene = MakeShared<Scene>();
		m_Scene = m_EditorScene;
		m_Scene->StartEdit();
		m_SelectedEntity = {};
		m_HierarchyView.SetScene(m_Scene);
	}

	void EditorLayer::SaveScene(const std::string& path)
	{
		SceneSerializer::Serialize(m_Scene, path);
	}

	void EditorLayer::LoadScene(const std::string& path)
	{
		m_SelectedEntity = {};
		NewScene();
		SceneSerializer::Deserialize(m_Scene, path);
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
				LoadScene(path);
		}

		saveDialog.BeginFileBrowser();
		if (saveDialog.IsDone())
		{
			const std::string& path = saveDialog.GetSelected().string();
			if (path != "")
				SaveScene(path);
		}

		Viewport(m_ShowViewport);
		GameView(m_ShowGameView);
		m_AssetsView->Render(m_ShowAssets);
		m_HierarchyView.Render(m_ShowHierarchy, m_ShowInspector);

		Statusbar();
		ImGui::End();
	}

	void EditorLayer::Overlay()
	{
		if (!m_ShowOverlay)
			return;

		ImGuiIO& io = ImGui::GetIO();
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing;

		float buttonSize = ImGui::GetFontSize() + 1.0f;
		auto& gizmoSelection = m_GizmosData.GizmoSelection;

		if (ImGui::Begin("Overlay", &m_ShowOverlay, window_flags))
		{
			ImGuiUtils::AddPadding(0.0f, buttonSize);

			if (ImGuiUtils::ButtonSelectable((ImTextureID)m_SelectTexture->GetTextureId(), buttonSize, buttonSize, GIZMOSELECTION::NONE == gizmoSelection))
				gizmoSelection = GIZMOSELECTION::NONE;

			if (ImGuiUtils::ButtonSelectable((ImTextureID)m_TranslateTexture->GetTextureId(), buttonSize, buttonSize, GIZMOSELECTION::TRANSLATE == gizmoSelection))
				gizmoSelection = GIZMOSELECTION::TRANSLATE;

			if (ImGuiUtils::ButtonSelectable((ImTextureID)m_ResizeTexture->GetTextureId(), buttonSize, buttonSize, GIZMOSELECTION::SCALE == gizmoSelection))
				gizmoSelection = GIZMOSELECTION::SCALE;

			if (ImGuiUtils::ButtonSelectable((ImTextureID)m_RotateTexture->GetTextureId(), buttonSize, buttonSize, GIZMOSELECTION::RORTATE == gizmoSelection))
				gizmoSelection = GIZMOSELECTION::RORTATE;
		}
		ImGui::End();
	}

	void EditorLayer::Viewport(bool& render)
	{
		if (!render)
			return;

		ImGuizmo::BeginFrame();

		ImGuiIO& io = ImGui::GetIO();
		ImGuiWindowFlags flags = ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin(ICON_MD_CAMERA "Viewport", &render, flags);

		m_ViewportData.ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportData.ViewportHovered = ImGui::IsWindowHovered();

		const auto& viewportSize = ImGui::GetContentRegionAvail();
		m_ViewportData.ViewportSize.x = viewportSize.x;
		m_ViewportData.ViewportSize.y = viewportSize.y;

		const auto& viewportPos = ImGui::GetCursorScreenPos();
		m_ViewportData.ViewportPosition.x = viewportPos.x;
		m_ViewportData.ViewportPosition.y = viewportPos.y;

		Overlay();

		ImGui::Image((void*)m_ViewportFbo->GetTexID(), { viewportSize.x, viewportSize.y }, { 0, 1 }, { 1, 0 });

		auto& gizmoSelection = m_GizmosData.GizmoSelection;
		if (m_SelectedEntity && gizmoSelection != GIZMOSELECTION::NONE)
		{
			const glm::mat4& view = m_EditorCamera.GetView();
			const glm::mat4& projection = m_EditorCamera.GetProjection();

			TransformComponent& component = m_SelectedEntity.GetComponent<TransformComponent>();
			glm::mat4 rotation = glm::toMat4(glm::quat(component.Rotation));
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), component.Position) * rotation * glm::scale(glm::mat4(1.0f), component.Scale);

			ImGuizmo::SetRect(viewportPos.x, viewportPos.y, viewportSize.x, viewportSize.y);
			ImGuizmo::SetOrthographic(true);
			ImGuizmo::SetDrawlist();

			float snapAmount = 0;

			if (gizmoSelection != GIZMOSELECTION::RORTATE)
				snapAmount = m_GizmosData.SnapAmount;
			else
				snapAmount = 45.0f;

			ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), (ImGuizmo::OPERATION)gizmoSelection, ImGuizmo::LOCAL, glm::value_ptr(transform), NULL, m_GizmosData.IsSnapping ? &snapAmount : NULL);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 finalPos, finalRot, finalSiz;
				Maths::DecomposeTransform(transform, finalPos, finalRot, finalSiz);
				glm::vec3 deltaRotation = finalRot - component.Rotation;
				component.Position = finalPos;
				component.Rotation += deltaRotation;
				component.Scale = finalSiz;
				m_GizmosData.IsUsing = true;
			}
			else
				m_GizmosData.IsUsing = false;
		}
		else
			m_GizmosData.IsUsing = false;

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorLayer::GameView(bool& render)
	{
		if (!render)
			return;

		ImGuiIO& io = ImGui::GetIO();
		ImGuiWindowFlags flags = ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin(ICON_MD_LANDSCAPE "GameView", &render, flags);

		m_GameViewData.ViewportFocused = ImGui::IsWindowFocused();
		m_GameViewData.ViewportHovered = ImGui::IsWindowHovered();

		const auto& viewportSize = ImGui::GetContentRegionAvail();
		m_GameViewData.ViewportSize.x = viewportSize.x;
		m_GameViewData.ViewportSize.y = viewportSize.y;

		const auto& viewportPos = ImGui::GetCursorScreenPos();
		m_GameViewData.ViewportPosition.x = viewportPos.x;
		m_GameViewData.ViewportPosition.y = viewportPos.y;

		ImGui::Image((void*)m_GameFbo->GetTexID(), { viewportSize.x, viewportSize.y }, { 0, 1 }, { 1, 0 });

		ImGui::End();
		ImGui::PopStyleVar();
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

		ImGui::Text("Viewport State");
		ImGui::Text("Hovered: %d ", m_ViewportData.ViewportHovered);
		ImGui::Text("Focused: %d ", m_ViewportData.ViewportFocused);
		ImGui::Text("Viewport Size: %.2f %.2f ", m_ViewportData.ViewportSize.x, m_ViewportData.ViewportSize.y);
		ImGui::Text("Viewport Position: %.2f %.2f ", m_ViewportData.ViewportPosition.x, m_ViewportData.ViewportPosition.y);

		ImGui::Separator();
		ImGuiUtils::AddPadding(0.0f, 10.0f);

		const auto& renderStats = Renderer::GetStats();
		ImGui::Text("Viewport Renderer Data");
		ImGui::Text("Draw Calls: %d", renderStats.DrawCalls);
		ImGui::Text("Vertex Count: %d", renderStats.VertexCount);
		ImGui::Text("Quad Count: %d", renderStats.QuadCount);

		ImGui::Separator();
		ImGuiUtils::AddPadding(0.0f, 10.0f);

		ImGui::Text("GameView State");
		ImGui::Text("Hovered: %d ", m_GameViewData.ViewportHovered);
		ImGui::Text("Focused: %d ", m_GameViewData.ViewportFocused);
		ImGui::Text("Viewport Size: %.2f %.2f ", m_GameViewData.ViewportSize.x, m_GameViewData.ViewportSize.y);
		ImGui::Text("Viewport Position: %.2f %.2f ", m_GameViewData.ViewportPosition.x, m_GameViewData.ViewportPosition.y);

		ImGui::Separator();
		ImGuiUtils::AddPadding(0.0f, 10.0f);

		ApplicationPrefs& prefs = Application::GetPrefs();
		ImGui::Text("Application Prefs");
		ImGui::Text("Vsync: %s", prefs.VsyncOn ? "On" : "Off");
		if (ImGui::Button("Set Vsync"))
			Application::SetVsync(!prefs.VsyncOn);

		ImGui::Text("Resolution x: %d y: %d", prefs.Resolution.Width, prefs.Resolution.Height);
		ImGui::Text("Resolution Dpi: %.2f", prefs.Resolution.Dpi);

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
					saveDialog.OpenFileBrowser();

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
				if (ImGui::MenuItem("Viewport", " ", m_ShowViewport, true))
					m_ShowViewport = !m_ShowViewport;

				if (ImGui::MenuItem("GameView", " ", m_ShowGameView, true))
					m_ShowGameView = !m_ShowGameView;

				if (ImGui::MenuItem("Hierarchy", " ", m_ShowHierarchy, true))
					m_ShowHierarchy = !m_ShowHierarchy;

				if (ImGui::MenuItem("Inspector", " ", m_ShowInspector, true))
					m_ShowInspector = !m_ShowInspector;

				if (ImGui::MenuItem("Overlay", " ", m_ShowOverlay, true))
					m_ShowOverlay = !m_ShowOverlay;

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
				Shared<Texture> icon = isEditing ? m_PlayTexture : m_PauseTexture;

				auto& gizmoSelection = m_GizmosData.GizmoSelection;

				if (ImGuiUtils::ButtonSelectable((ImTextureID)m_SelectTexture->GetTextureId(), buttonSize, buttonSize, GIZMOSELECTION::NONE == gizmoSelection))
					gizmoSelection = GIZMOSELECTION::NONE;

				if (ImGuiUtils::ButtonSelectable((ImTextureID)m_TranslateTexture->GetTextureId(), buttonSize, buttonSize, GIZMOSELECTION::TRANSLATE == gizmoSelection))
					gizmoSelection = GIZMOSELECTION::TRANSLATE;

				if (ImGuiUtils::ButtonSelectable((ImTextureID)m_ResizeTexture->GetTextureId(), buttonSize, buttonSize, GIZMOSELECTION::SCALE == gizmoSelection))
					gizmoSelection = GIZMOSELECTION::SCALE;

				if (ImGuiUtils::ButtonSelectable((ImTextureID)m_RotateTexture->GetTextureId(), buttonSize, buttonSize, GIZMOSELECTION::RORTATE == gizmoSelection))
					gizmoSelection = GIZMOSELECTION::RORTATE;

				ImGuiUtils::AddPadding((ImGui::GetContentRegionAvail().x * 0.5f) - (buttonSize * 0.5f), 0.0f);

				if (ImGuiUtils::ButtonSelectable((ImTextureID)icon->GetTextureId(), buttonSize, buttonSize, m_EditorState == EditorState::Pause))
				{
					if (m_EditorState == EditorState::Edit)
					{
						m_EditorState = EditorState::Play;
						m_Scene->StopEdit();
						m_Scene = Scene::CopyScene(m_EditorScene);
						m_Scene->StartPlay();
						m_HierarchyView.SetScene(m_Scene);
					}
					else if (m_EditorState == EditorState::Play)
						m_EditorState = EditorState::Pause;
					else if (m_EditorState == EditorState::Pause)
						m_EditorState = EditorState::Play;
				}

				if (m_EditorState != EditorState::Edit && ImGui::ImageButton((ImTextureID)m_StopTexture->GetTextureId(), { buttonSize, height * 0.5f }))
				{
					m_EditorState = EditorState::Edit;
					m_Scene->StopPlay();
					m_Scene = m_EditorScene;
					m_Scene->StartEdit();
					m_HierarchyView.SetScene(m_Scene);
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