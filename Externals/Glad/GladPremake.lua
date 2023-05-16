project "Glad"
	kind "StaticLib"
	language "C"
	staticruntime "off"
	systemversion "latest"

    targetdir(dirTarget)
    objdir(dirObj)

	files
	{
		"include/glad/glad.h",
		"include/KHR/khrplatform.h",
		"src/glad.c"
	}

	includedirs
	{
		"include",
	}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
