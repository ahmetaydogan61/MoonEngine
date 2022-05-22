#include "EditorSettingsView.h"
#include "../ImGuiUtils.h"

namespace MoonEngine
{
    void EditorSettingsView::BeginEditorSettings(bool& open)
    {
        if (!open)
            return;

        ImGui::Begin("Editor Settings", &open);
        static int style_idx = -1;
        if (ImGui::Combo("Style", &style_idx, "Light\0Dark\0Dark3D\0"))
        {
            switch (style_idx)
            {
                case 0: ImGui::StyleColorsLight(); break;
                case 1: ImGuiUtils::StyleCustomDark(0); break;
                case 2: ImGuiUtils::StyleCustomDark(1); break;
            }
        }
        ImGui::End();
    }
}