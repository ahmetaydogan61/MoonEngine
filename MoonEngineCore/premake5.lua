project "MoonEngineCore"
	kind "StaticLib"
	language "C++"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-obj/" .. outputdir .. "/%{prj.name}")

	pchheader "mpch.h"
	pchsource "src/mpch.cpp"

	defines { "_CRT_SECURE_NO_WARNINGS", "GLFW_INCLUDE_NONE" }

	files
	{
		"src/**.h",
		"src/**.cpp",
		"Includes/GLM/glm/**.hpp",
		"Includes/GLM/glm/**.inl",
		"Includes/ImGuizmo/ImGuizmo.h",
		"Includes/ImGuizmo/ImGuizmo.cpp"
	}

	includedirs
	{
		"src",
		"%{IncludeDirs.GLFW}",
		"%{IncludeDirs.Glad}",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.ImGui}/imgui",
		"%{IncludeDirs.glm}",
		"%{IncludeDirs.stb_image}",
		"%{IncludeDirs.entt}",
		"%{IncludeDirs.ImGuizmo}"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "files:Includes/ImGuizmo/**.cpp"
	flags { "NoPCH" }

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"
		defines { "ENGINE_PLATFORM_WIN", "ENGINE_BUILD" }

	filter "configurations:Debug"
		defines {"ENGINE_DEBUG"}
		symbols "On"

	filter "configurations:Release"
		defines {"ENGINE_RELEASE"}
		optimize "On"
	
	filter "configurations:Release"
		defines {"ENGINE_FINAL"}
		optimize "On"