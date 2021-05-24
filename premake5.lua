workspace "fractal-generator"
architecture "x64"

configurations {"Debug", "Release", "Dist"}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "fractal-generator"
location ""
kind "ConsoleApp"
language "C++"
vectorextensions "avx2"

targetdir("bin/" .. outputdir .. "/%{prj.name}")
objdir("bin-int/" .. outputdir .. "/%{prj.name}")

pchheader "fpch.h"
pchsource "src/fpch.cpp"

defines {"GLEW_STATIC"}

files {"src/**.cpp", "vendor/imgui/**.cpp", "vendor/imgui/backends/**.cpp"}

includedirs {"src", "vendor/spdlog/include", "vendor/glfw/include", "vendor/glew/include", "vendor/imgui"}

libdirs {"vendor/glfw/lib", "vendor/glew/lib"}

links {"opengl32.lib", "glew32s.lib", "glfw3.lib", "msvcrt.lib", "msvcmrt.lib"}

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
