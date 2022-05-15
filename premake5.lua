workspace "MoonEngine"
	architecture "x64"
	startproject "MoonEngine"
	configurations { "Debug", "Release", "Final" }

outputdir = "%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}"

IncludeDirs = {}
IncludeDirs["GLFW"] = "MoonEngineCore/Includes/GLFW/include"
IncludeDirs["Glad"] = "MoonEngineCore/Includes/Glad/include"
IncludeDirs["glm"] = "MoonEngineCore/Includes/GLM"
IncludeDirs["stb_image"] = "MoonEngineCore/Includes/STB_IMAGE"
IncludeDirs["entt"] = "MoonEngineCore/Includes/ENTT"

include "MoonEngineCore/Includes/GLFW"
include "MoonEngineCore/Includes/Glad"

project "MoonEngineCore"
	location "MoonEngineCore"
	kind "StaticLib"
	language "C++"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-obj/" .. outputdir .. "/%{prj.name}")

	pchheader "mpch.h"
	pchsource "MoonEngineCore/src/mpch.cpp"

	defines { "_CRT_SECURE_NO_WARNINGS" }

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/Includes/GLM/glm/**.hpp",
		"%{prj.name}/Includes/GLM/glm/**.inl"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{IncludeDirs.GLFW}",
		"%{IncludeDirs.Glad}",
		"%{IncludeDirs.glm}",
		"%{IncludeDirs.stb_image}",
		"%{IncludeDirs.entt}"
	}

	links
	{
		"GLFW",
		"Glad",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"
		defines { "ENGINE_PLATFORM_WIN", "ENGINE_BUILD", "GLFW_INCLUDE_NONE" }

	filter "configurations:Debug"
		defines {"ENGINE_DEBUG"}
		symbols "On"

	filter "configurations:Release"
		defines {"ENGINE_RELEASE"}
		optimize "On"
	
	filter "configurations:Release"
		defines {"ENGINE_FINAL"}
		optimize "On"

project "MoonEngine"
	location "MoonEngine"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-obj/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"MoonEngineCore/src",
		"%{IncludeDirs.glm}"
	}   

	links
	{
		"MoonEngineCore"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"
		defines {"ENGINE_PLATFORM_WIN"}
		
		filter "configurations:Debug"
		defines {"ENGINE_DEBUG"}
		symbols "On"

	filter "configurations:Release"
		defines {"ENGINE_RELEASE"}
		optimize "On"
	
	filter "configurations:Release"
		defines {"ENGINE_FINAL"}
		optimize "On"