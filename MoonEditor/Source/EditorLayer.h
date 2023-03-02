#pragma once
#include "Utils/EditorCamera.h"
#include "Views/AssetsView.h"
#include "Views/HierarchyView.h"

#include <Core/ApplicationLayer.h>
#include <Event/Action.h>
#include <Engine/Entity.h>

namespace MoonEngine
{
	class Framebuffer;
	class Scene;

	enum class GIZMOSELECTION
	{
		//These are the values from ImGuizmo OPERATION enum
		NONE = -1,
		TRANSLATE = 7,
		RORTATE = 120,
		SCALE = 896,
	};

	struct GizmosData
	{
		GIZMOSELECTION GizmoSelection = GIZMOSELECTION::TRANSLATE;
		float SnapAmount = 0.25f;
		bool IsSnapping = false;
		bool IsUsing = false;
	};

	struct ViewportData
	{
		bool ViewportFocused = false;
		bool ViewportHovered = false;

		glm::vec2 ViewportSize = glm::vec2(0.0f);
		glm::vec2 ViewportPosition = glm::vec2(0.0f);
	};

	class EditorLayer : public ApplicationLayer
	{
		enum class EditorState
		{
			Play,
			Edit,
			Pause
		};

	private:
		EditorState m_EditorState = EditorState::Edit;
		Shared<Texture> m_PlayTexture, m_StopTexture, m_PauseTexture;
		Shared<Texture> m_SelectTexture, m_TranslateTexture, m_ResizeTexture, m_RotateTexture;

		Shared<Scene> m_Scene, m_EditorScene;
		Entity m_SelectedEntity;

		void KeyEvents(KeyPressEvent& a);

		//+Viewport Stuff
		ViewportData m_ViewportData;
		GizmosData m_GizmosData;
		Shared<Framebuffer> m_ViewportFbo;

		Camera m_EditorCamera;
		CameraController m_EditorCameraController;

		bool m_ShowViewport = true;
		bool m_ShowOverlay = true;
		void Viewport(bool& render);
		void Overlay();
		//-Viewport Stuff

		//+GameView Stuff
		ViewportData m_GameViewData;
		Shared<Framebuffer> m_GameFbo;

		bool m_ShowGameView = true;
		void GameView(bool& render);
		//-GameView Stuff

		//+Hierarchy Stuff
		bool m_ShowHierarchy = true;
		bool m_ShowInspector = true;
		HierarchyView m_HierarchyView;
		//-Hierarchy Stuff

		//+Assets Stuff
		bool m_ShowAssets = true;
		Shared<AssetsView> m_AssetsView;
		//-Assets Stuff

		bool m_ShowDebug = true;
		void Debug();

		void Dockspace();
		void Menubar();
		void SubMenubar();
		void Statusbar();

		inline static EditorLayer* s_EditorLayer;
	public:
		void Init();
		void Update();
		void DrawGui();

		void NewScene();
		void SaveScene(const std::string& path);
		void LoadScene(const std::string& path);

		void SetSelectedEntity(Entity entity) { m_SelectedEntity = entity; }
		Entity& GetSelectedEntity() { return m_SelectedEntity; }
		void DuplicateSelectedEntity() { if (!m_SelectedEntity) return; m_SelectedEntity = m_Scene->DuplicateEntity(m_SelectedEntity); }
		void DestroySelectedEntity() { if (!m_SelectedEntity) return; m_SelectedEntity.Destroy(); m_SelectedEntity = {}; }

		static EditorLayer* Get() { if (s_EditorLayer) return s_EditorLayer; else return nullptr; }
	};
}