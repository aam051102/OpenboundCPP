include "conanbuildinfo.premake.lua"

workspace "OpenboundCPP"
    conan_basic_setup()
    architecture "x64"
    startproject "openbound"

    configurations
    {
        "Debug",
        "Release"
    }

    project "openbound"
        location "openbound"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++17"
        staticruntime "on"

        targetdir "build/bin/%{prj.name}-%{cfg.buildcfg}"
        objdir "build/obj/%{prj.name}-%{cfg.buildcfg}"

        files
        {
            "%{prj.name}/includes/**.h",
            "%{prj.name}/src/**.cpp"
        }

        includedirs
        {
            "%{prj.name}/includes",
            "includes/",
            "lib/"
        }

		filter "system:windows"
			postbuildcommands { "{COPY} %{conan_rootpath_openal}/bin/OpenAL32.dll %{cfg.targetdir}" }

        filter "configurations:Debug"
            defines "SBURB_DEBUG"
            runtime "Debug"
            symbols "on"
        
        filter "configurations:Release"
            defines "SBURB_RELEASE"
            runtime "Release"
            optimize "on"