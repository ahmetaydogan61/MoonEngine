#pragma once
#include "Utils/EditorCamera.h"
#include "Views/Views.h"

#include "Views/Gizmos/Gizmos.h"

namespace MoonEngine
{
	struct ViewportView : public AdvancedView
	{
		ViewportView();
		~ViewportView() = default;
		void Update();
		void Render();

		void OnKeyEvent(Keycode key);

		Shared<Framebuffer> Viewbuffer;
	private:
		Shared<EditorCamera> m_EditorCamera;
		GizmosData m_GizmosData;
	};
}