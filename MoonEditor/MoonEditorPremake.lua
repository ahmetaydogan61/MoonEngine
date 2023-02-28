project "MoonEditor"
    kind "ConsoleApp"
    language "C++"
    staticruntime "On"

    targetdir(dirTarget)
    objdir(dirObj)

    defines { "_CRT_SECURE_NO_WARNINGS", "GLFW_INCLUDE_NONE", "STB_IMAGE_IMPLEMENTATION" }

    files 
    {
        "**.h",
        "**.hpp",
        "**.cpp"
    }

    includedirs
    {
        "Source",
        includeEntt,
        includeGlm,
        includeImGui,
        includeMoonEngine,
        includeSpdlog,
        includeYaml,
        includeIconFont,
        includeUuidv4,
        includeImGuizmo
    }

    links
    {
        "MoonEngine"
    }

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
