#pragma once
#include "Core.h"
#include "Views/HierarchyView.h"
#include "Views/EditorSettingsView.h"
#include "Renderer/Frambuffer.h"

namespace MoonEngine
{
	class Texture;

	class EditorLayer : public Layer
	{
	private:
		Scene* m_Scene;
		EditorCamera* m_EditorCamera;
		Texture* m_PlayTexture;
		Texture* m_StopTexture;

		HierarchyView m_HierarchyView;
		EditorSettingsView m_EditorSettingsView;
		bool m_IsHierarchyActive = true;
		bool m_IsInspectorActive = true;
		bool m_IsViewportActive = true;
		bool m_IsEditorSettingsActive = false;
		bool m_IsDebugActive = true;
		
		bool m_IsPlaying = false;
		void PlayScene();
		void PauseScene();

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
	public:
		void Create();
		void Update();
		void OnEvent(Event& event);
		void DrawGUI();
		void Destroy();
	};
}