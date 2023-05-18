workspace "TemplateProject"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "Template"
    
project "Template"
    kind "SharedLib"
    language "C#"
    dotnetframework "4.7.2"

    targetdir("Build")
    objdir("Build/intermediate")

    files
    {
        "Source/**.cs",
        "Properties/**.cs",
    }

    links
    {
       "MoonScripter"
    }

    filter "configurations:Debug"
        optimize "Off"
        symbols "Default"

    filter "configurations:Release"
        optimize "On"
        symbols "Default"
    
group "MoonEngine"

include "../../MoonScripter/MoonScripterPremake.lua"

group ""
 