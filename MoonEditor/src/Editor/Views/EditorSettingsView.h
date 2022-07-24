#pragma once
#include "MoonEngine.h"
namespace MoonEngine
{
    enum class EditorStyle
    {
        Dark = 0,
        DarkCartoon,
        Gray,
        Gray3D
    };

    class EditorSettingsView
    {
    private:
        EditorStyle m_CurrentStyle = EditorStyle::Dark;
    public:
        void BeginEditorSettings(bool& open);
        void SetStyle(EditorStyle style);
    };
}
