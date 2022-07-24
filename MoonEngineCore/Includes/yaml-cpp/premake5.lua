project "yaml-cpp"
	kind "StaticLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin/_obj/" .. outputdir .. "/%{prj.name}")

	defines { "YAML_CPP_STATIC_DEFINE" }

	files
	{
		"src/**.h",
		"src/**.cpp",

		"include/**.h"
	}

	includedirs
	{
		"include"
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"
		staticruntime "on"

	filter "system:linux"
		pic "On"
		systemversion "latest"
		cppdialect "C++17"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

    filter "configurations:Final"
		runtime "Release"
		optimize "on"
