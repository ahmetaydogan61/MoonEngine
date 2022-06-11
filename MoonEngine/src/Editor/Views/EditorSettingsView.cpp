#include "EditorSettingsView.h"
#include "../ImGuiUtils.h"
#include "Utils/IconsFontAwesome.h"
#include "Renderer/Renderer.h"

namespace MoonEngine
{
	void EditorSettingsView::BeginEditorSettings(bool& open)
	{
		if (ImGui::Begin(ICON_FK_COG "Editor Settings", &open))
		{
			static int style_idx = 0;
			if (ImGui::Combo("Style", &style_idx, "Dark\0DarkCartoon\0Gray\0Gray3D\0"))
			{
				switch (style_idx)
				{
					case 0: ImGuiUtils::StyleCustomDark(0); break;
					case 1: ImGuiUtils::StyleCustomDark(1); break;
					case 2: ImGuiUtils::StyleCustomGray(0); break;
					case 3: ImGuiUtils::StyleCustomGray(1); break;
				}
			}

			glm::vec4& clearColor = Renderer::GetClearColor();
			ImGui::ColorEdit4("Clear Color:", &clearColor[0]);
			Renderer::SetClearColor(clearColor);
		}
		ImGui::End();
	}
}