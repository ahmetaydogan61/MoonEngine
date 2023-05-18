project "MoonScripter"
    kind "SharedLib"
    language "C#"
    dotnetframework "4.7.2"

    targetdir("../MoonEditor/Resource/Scripts")
    objdir("../MoonEditor/Resource/Scripts/intermediate")

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
    
 