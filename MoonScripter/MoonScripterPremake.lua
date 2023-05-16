project "MoonScripter"
    kind "SharedLib"
    language "C#"
    dotnetframework "4.7.2"

    targetdir("%{wks.location}/MoonEditor/Resource/Scripts")
    objdir("%{wks.location}/MoonEditor/Resource/Scripts/intermediate")

    defines { "_CRT_SECURE_NO_WARNINGS", "GLFW_INCLUDE_NONE", "STB_IMAGE_IMPLEMENTATION", "YAML_CPP_STATIC_DEFINE" }

    files
    {
        "Source/**.cs",
        "Properties/**.cs",
    }

    filter "configurations:Debug"
        optimize "Off"
        symbols "Default"

    filter "configurations:Release"
        optimize "On"
        symbols "Default"
    
 