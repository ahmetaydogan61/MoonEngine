#include "EditorSettingsView.h"
#include "../ImGuiUtils.h"
#include "Utils/IconsFontAwesome.h"
#include "Renderer/Renderer.h"

namespace MoonEngine
{
	void EditorSettingsView::SetStyle(EditorStyle style)
	{
		switch (style)
		{
			case EditorStyle::Dark: ImGuiUtils::StyleCustomDark(0); break;
			case EditorStyle::DarkCartoon: ImGuiUtils::StyleCustomDark(1); break;
			case EditorStyle::Gray:   ImGuiUtils::StyleCustomGray(0); break;
			case EditorStyle::Gray3D: ImGuiUtils::StyleCustomGray(1); break;
			default: break;
		}
	}

	void EditorSettingsView::BeginEditorSettings(bool& open)
	{
		if (ImGui::Begin(ICON_FK_COG "Editor Settings", &open))
		{
			int currentStyle = (int)m_CurrentStyle;
			if (ImGui::Combo("Style", &currentStyle, "Dark\0DarkCartoon\0Gray\0Gray3D\0"))
			{
				m_CurrentStyle = (EditorStyle)currentStyle;
				SetStyle(m_CurrentStyle);
			}
			glm::vec4& clearColor = Renderer::GetClearColor();
			ImGui::ColorEdit4("Clear Color", &clearColor[0]);
			Renderer::SetClearColor(clearColor);
		}
		ImGui::End();
	}
}