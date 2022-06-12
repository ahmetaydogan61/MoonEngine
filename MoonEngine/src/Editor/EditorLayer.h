#pragma once
#include "Core.h"
#include "Views/HierarchyView.h"
#include "Views/EditorSettingsView.h"
#include "Views/ContentView.h"
#include "Renderer/Frambuffer.h"
#include "Renderer/Texture.h"

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
		Ref<Scene> m_Scene;
		EditorCamera m_EditorCamera;
		Ref<Texture> m_PlayTexture;
		Ref<Texture> m_StopTexture;
		Ref<Texture> m_SelectTexture;
		Ref<Texture> m_TranslateTexture;
		Ref<Texture> m_RotateTexture;
		Ref<Texture> m_ResizeTexture;

		HierarchyView m_HierarchyView; 
		EditorSettingsView m_EditorSettingsView;
		ContentView m_ContentView;
		bool m_IsHierarchyActive = true;
		bool m_IsInspectorActive = true;
		bool m_IsViewportActive = true;
		bool m_IsContentActive = true;
		bool m_IsEditorSettingsActive = false;
		bool m_IsDebugActive = false;
		
		bool m_IsPlaying = false;
		void OnPlay();
		void OnStop();

		void Dockspace();
		void Menubar();
		void Sidemenubar();
		void Statusbar();
		void ViewportView(bool& state);
		void DebugView(bool& state);

		Ref<Framebuffer> m_ViewportFramebuffer = nullptr;
		glm::vec2 m_ViewportPosition;
		glm::vec2 m_ViewportSize;
		bool m_ViewportHovered = false;
		bool m_ViewportFocused = false;

		bool GizmoSelectButton(Ref<Texture> texture, float width, float height, bool selected);
		GIZMOSELECTION m_GizmoSelection = GIZMOSELECTION::TRANSLATE;
		bool m_IsSnapping = false;
		float m_SnapAmount = 0.25f;

		bool EditorLayer::KeyEvents(Event& event);
	public:
		void Create();
		void Update();
		void OnEvent(Event& event);
		void DrawGUI();
	};
}