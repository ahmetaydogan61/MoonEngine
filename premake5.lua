workspace "MoonEngine"
	architecture "x64"
	startproject "MoonEngine"
	configurations { "Debug", "Release", "Final" }

outputdir = "%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}"

IncludeDirs = {}
IncludeDirs["GLFW"] = "%{wks.location}/MoonEngineCore/Includes/GLFW/include"
IncludeDirs["Glad"] = "%{wks.location}/MoonEngineCore/Includes/Glad/include"
IncludeDirs["ImGui"] = "%{wks.location}/MoonEngineCore/Includes/ImGui"
IncludeDirs["glm"] = "%{wks.location}/MoonEngineCore/Includes/GLM"
IncludeDirs["stb_image"] = "%{wks.location}/MoonEngineCore/Includes/stb_image"
IncludeDirs["entt"] = "%{wks.location}/MoonEngineCore/Includes/ENTT"
IncludeDirs["ImGuizmo"] = "%{wks.location}/MoonEngineCore/Includes/ImGuizmo"

include "MoonEngineCore/Includes/GLFW"
include "MoonEngineCore/Includes/Glad"
include "MoonEngineCore/Includes/ImGui/imgui"
include "MoonEngineCore"
include "MoonEngine"