project "ImGui"
    kind "StaticLib"
    language "C++"
    staticruntime "On"
    systemversion "latest"

    targetdir(dirTarget)
    objdir(dirObj)

    defines { "_CRT_SECURE_NO_WARNINGS" }

    files
	{
		"imconfig.h",
		"imgui.h",
		"imgui.cpp",
		"imgui_draw.cpp",
		"imgui_internal.h",
		"imgui_tables.cpp",
		"imgui_widgets.cpp",
		"imstb_rectpack.h",
		"imstb_textedit.h",
		"imstb_truetype.h",
		"imgui_demo.cpp"
	}

    filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"