workspace "MoonEngine2D"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "MoonEditor"
    
dirTarget = "Build/%{cfg.system}/%{cfg.buildcfg}/%{cfg.architecture}"
dirObj = "Build/intermediate/%{cfg.system}/%{cfg.buildcfg}/%{cfg.architecture}"

includeGLFW = "%{wks.location}/Externals/GLFW/include"
includeGlad = "%{wks.location}/Externals/Glad/include"
includeGlm = "%{wks.location}/Externals/GLM"
includeStb = "%{wks.location}/Externals/stb"
includeImGui = "%{wks.location}/Externals/ImGui"
includeSpdlog = "%{wks.location}/Externals/spdlog"
includeEntt = "%{wks.location}/Externals/entt"
includeYaml = "%{wks.location}/Externals/yaml-cpp/include"
includeIconFont = "%{wks.location}/Externals/IconFontCppHeaders"
includeImGuizmo = "%{wks.location}/Externals/ImGuizmo"
includeBox2D = "%{wks.location}/Externals/Box2D/include"
includeMono = "%{wks.location}/Externals/mono/include"
includeFileWatch = "%{wks.location}/Externals/filewatch"

LibraryDir = {}
LibraryDir["mono"] = "%{wks.location}/Externals/mono/lib/%{cfg.buildcfg}"

Library = {}
Library["mono"] = "%{LibraryDir.mono}/libmono-static-sgen.lib"

--Include if on windows
Library["WinSock"] = "Ws2_32.lib"
Library["Winmm"] = "Winmm.lib"
Library["WinVer"] = "Version.lib"
Library["Bcrypt"] = "Bcrypt.lib"

group "Externals"
    include "Externals/GLFW/GLFWPremake.lua"
    include "Externals/Glad/GladPremake.lua"
    include "Externals/ImGui/ImGuiPremake.lua"
    include "Externals/yaml-cpp/YamlPremake.lua"
    include "Externals/Box2D/Box2DPremake.lua"
group ""

group "MoonEngine"
    include "MoonEngine/MoonEnginePremake.lua"
    includeMoonEngine = "%{wks.location}/MoonEngine/Source"
    include "MoonScripter/MoonScripterPremake.lua"
group ""

group "MoonEditor"
    include "MoonEditor/MoonEditorPremake.lua"
    includeMoonEditor = "%{wks.location}/MoonEditor/Source"
group ""
