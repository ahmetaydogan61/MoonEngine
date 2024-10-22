project "MoonEngine"
    kind "StaticLib"
    language "C++"
    staticruntime "Off"

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
        includeImGuizmo,
        includeBox2D,
        includeMono,
        includeFileWatch
    }

    links
    {
        "Box2D",
        "GLFW",
        "Glad",
        "opengl32.lib",
        "ImGui",
        "yaml-cpp",

        "%{Library.mono}"
    }

    filter "files:Includes/ImGuizmo/imguizmo/**.cpp"
	flags { "NoPCH" }

    filter "system:windows"
        cppdialect "C++20"
        systemversion "latest"
        defines { "ENGINE_PLATFORM_WIN" }

        links
        {
            "%{Library.WinSock}",
            "%{Library.Winmm}",
            "%{Library.WinVer}",
            "%{Library.Bcrypt}"
        }

    filter "configurations:Debug"
        defines { "ENGINE_DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "ENGINE_RELEASE" }
        optimize "On"
