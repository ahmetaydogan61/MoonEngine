#pragma once
#include "Core.h"
#include "Views/HierarchyView.h"
#include "Views/EditorSettingsView.h"
#include "Renderer/Frambuffer.h"
#include "Renderer/Texture.h"

namespace MoonEngine
{
	enum class GIZMOSELECTION
	{
		//These are the values from ImGuizmo OPERATION enum
		NONE = -1, 
		TRANSLATE = 7,
		SCALE = 896,
	};

	class EditorLayer : public Layer
	{
	private:
		Scene* m_Scene;
		EditorCamera* m_EditorCamera;
		Texture* m_PlayTexture;
		Texture* m_StopTexture;
		Texture* m_SelectTexture;
		Texture* m_TranslateTexture;
		Texture* m_ResizeTexture;

		HierarchyView m_HierarchyView;
		EditorSettingsView m_EditorSettingsView;
		bool m_IsHierarchyActive = true;
		bool m_IsInspectorActive = true;
		bool m_IsViewportActive = true;
		bool m_IsEditorSettingsActive = false;
		bool m_IsDebugActive = true;
		
		bool m_IsPlaying = false;
		void PlayScene();
		void StopScene();

		void Dockspace();
		void Menubar();
		void Sidemenubar();
		void ViewportView(bool& state);
		void DebugView(bool& state);

		Framebuffer* m_ViewportFramebuffer = nullptr;
		glm::vec2 m_ViewportPosition;
		glm::vec2 m_ViewportSize;
		bool m_ViewportHovered = false;
		bool m_ViewportFocused = false;

		bool GizmoSelectButton(Texture* texture, float width, float height, bool selected);
		GIZMOSELECTION m_GizmoSelection = GIZMOSELECTION::TRANSLATE;
	public:
		void Create();
		void Update();
		void OnEvent(Event& event);
		void DrawGUI();
		void Destroy();
	};
}