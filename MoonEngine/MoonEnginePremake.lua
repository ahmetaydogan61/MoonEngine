project "MoonEngine"
    kind "StaticLib"
    language "C++"
    staticruntime "On"

    targetdir(dirTarget)
    objdir(dirObj)

    defines { "_CRT_SECURE_NO_WARNINGS", "GLFW_INCLUDE_NONE", "STB_IMAGE_IMPLEMENTATION", "YAML_CPP_STATIC_DEFINE" }

    pchheader "mpch.h"
	pchsource "Source/mpch.cpp"

    files 
    {
        "**.h",
        "**.hpp",
        "**.cpp",

        "%{wks.location}/Externals/ImGuizmo/imguizmo/ImGuizmo.h",
        "%{wks.location}/Externals/ImGuizmo/imguizmo/ImGuizmo.cpp"
    }

    includedirs
    {
        "Source",
        includeGLFW,
        includeGlad,
        includeGlm,
        includeStb,
        includeImGui,
        includeSpdlog,
        includeEntt,
        includeYaml,
        includeIconFont,
        includeUuidv4,
        includeImGuizmo,
        includeBox2D
    }

    links
    {
        "Box2D",
        "GLFW",
        "Glad",
        "opengl32.lib",
        "ImGui",
        "yaml-cpp"
    }

    filter "files:Includes/ImGuizmo/imguizmo/**.cpp"
	flags { "NoPCH" }

    filter "system:windows"
        cppdialect "C++20"
        systemversion "latest"
        defines { "ENGINE_PLATFORM_WIN" }

    filter "configurations:Debug"
        defines { "ENGINE_DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "ENGINE_RELEASE" }
        optimize "On"
