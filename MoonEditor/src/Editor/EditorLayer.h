#pragma once
#include "MoonEngine.h"

#include "Editor/Views/ContentView.h"
#include "Editor/Views/EditorSettingsView.h"
#include "Editor/Views/HierarchyView.h"

namespace MoonEngine
{
	enum class GIZMOSELECTION
	{
		//These are the values from ImGuizmo OPERATION enum
		NONE = -1, 
		TRANSLATE = 7,
		RORTATE = 120,
		SCALE = 896,
	};

	class EditorLayer : public Layer
	{
	private:
		Ref<Scene> m_Scene, m_EditorScene;
		std::string m_ScenePath;
		
		bool EditorLayer::KeyEvents(Event& event);

		bool m_IsPlaying = false;
		void OnPlay();
		void OnStop();

		void NewScene();
		void SaveScene(const std::string& path);
		void LoadScene(const std::string& path);

		EditorCamera m_EditorCamera;
		HierarchyView m_HierarchyView;
		EditorSettingsView m_EditorSettingsView;
		ContentView m_ContentView;
		bool m_IsHierarchyActive = true;
		bool m_IsInspectorActive = true;
		bool m_IsViewportActive = true;
		bool m_IsGameViewActive = true;
		bool m_IsContentActive = true;
		bool m_IsEditorSettingsActive = false;
		bool m_IsDebugActive = true;

		void Dockspace();
		void Menubar();
		void Sidemenubar();
		void Statusbar();
		void ViewportView(bool& state);
		void GameView(bool& state);
		void DebugView(bool& state);
		
		Ref<Framebuffer> m_ViewportFramebuffer = nullptr;
		Ref<Framebuffer> m_GameViewFramebuffer = nullptr;
		glm::vec2 m_ViewportPosition;
		glm::vec2 m_ViewportSize;
		glm::vec2 m_GameViewPosition;
		glm::vec2 m_GameViewSize;
		bool m_ViewportHovered = false;
		bool m_ViewportFocused = false;

		GIZMOSELECTION m_GizmoSelection = GIZMOSELECTION::TRANSLATE;
		bool m_IsSnapping = false;
		float m_SnapAmount = 0.25f;

		Ref<Texture> m_PlayTexture;
		Ref<Texture> m_StopTexture;
		Ref<Texture> m_SelectTexture;
		Ref<Texture> m_TranslateTexture;
		Ref<Texture> m_RotateTexture;
		Ref<Texture> m_ResizeTexture;
	public:
		void Create();
		void Update();
		void OnEvent(Event& event);
		void DrawGUI();
	};
}