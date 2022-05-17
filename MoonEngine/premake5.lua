project "MoonEngine"
	kind "ConsoleApp"
	language "C++"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-obj/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/MoonEngineCore/src",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.glm}",
		"%{IncludeDirs.entt}"
	}   

	links
	{
		"MoonEngineCore"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"
		defines {"ENGINE_PLATFORM_WIN"}
		
		filter "configurations:Debug"
		defines {"ENGINE_DEBUG"}
		symbols "On"

	filter "configurations:Release"
		defines {"ENGINE_RELEASE"}
		optimize "On"
	
	filter "configurations:Final"
		defines {"ENGINE_FINAL"}
		optimize "On"