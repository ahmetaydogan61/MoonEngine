#pragma once
#include "Views/Viewport/Viewport.h"
#include "Views/Game/GameView.h"

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
		
		Shared<Scene> m_Scene, m_EditorScene;
		Entity m_SelectedEntity = {};

		Shared<ViewportView> m_ViewportView;
		Shared<GameView> m_GameView;

		void KeyEvents(KeyPressEvent& e);
		void OnSceneChange();
		
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