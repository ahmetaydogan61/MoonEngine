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
includeUuidv4 = "%{wks.location}/Externals/uuid_v4"
includeImGuizmo = "%{wks.location}/Externals/ImGuizmo"

include "Externals/GLFW/GLFWPremake.lua"
include "Externals/Glad/GladPremake.lua"
include "Externals/ImGui/ImGuiPremake.lua"
include "Externals/yaml-cpp/YamlPremake.lua"

include "MoonEngine/MoonEnginePremake.lua"
includeMoonEngine = "%{wks.location}/MoonEngine/Source"

include "MoonEditor/MoonEditorPremake.lua"
includeMoonEditor = "%{wks.location}/MoonEditor/Source"