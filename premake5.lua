workspace "fractal-generator"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "fractal-generator"
    location ""
    kind "ConsoleApp"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "fpch.h"
    pchsource "src/fpch.cpp"

    files
    {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs
    {
        "vendor/spdlog/include",
        "vendor/olcPixelEngine",
        "src/",
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

    filter "configurations:Debug"
        defines "FRACTAL_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "FRACTAL_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "FRACTAL_DIST"
        optimize "On"
