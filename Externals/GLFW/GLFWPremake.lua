project "GLFW"
    kind "StaticLib"
    language "C"
    staticruntime "On"
    systemversion "latest"

    targetdir(dirTarget)
    objdir(dirObj)

    defines { "_CRT_SECURE_NO_WARNINGS" }

    files
	{
		"src/glfw_config.h",
        "include/GLFW/glfw3.h",
		"include/GLFW/glfw3native.h",
		"src/context.c",
		"src/init.c",
		"src/input.c",
		"src/monitor.c",
		"src/vulkan.c",
		"src/window.c"
	}

    filter "system:windows"
        defines { "_GLFW_WIN32" }

        files
        {
            "src/win32_init.c",
            "src/win32_joystick.c",
            "src/win32_monitor.c",
            "src/win32_time.c",
            "src/win32_thread.c",
            "src/win32_window.c",
            "src/wgl_context.c",
            "src/egl_context.c",
            "src/osmesa_context.c"
        }

    filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"