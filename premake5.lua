workspace "DenisDimitrovCoursework"
    architecture "x64"
    configurations { "Debug", "Release"}

project "DenisDimitrovCoursework"
    kind "ConsoleApp"
    language "C++"

    targetdir ("bin/%{cfg.buildcfg}")
    objdir ("bin-int/%{cfg.buildcfg}")

    files { 
        "src/**.h", 
        "src/**.cpp",
    }

    includedirs { "vendor/SFML/include" }

    libdirs { "vendor/SFML/lib" }

    cppdialect "C++20"
    systemversion "latest"

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
        links
        {
            "sfml-graphics-d",
            "sfml-audio-d",
            "sfml-main",
            "sfml-system-d",
            "sfml-network-d",
            "sfml-window-d",
        }

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
        links
        {
            "sfml-graphics",
            "sfml-audio",
            "sfml-main",
            "sfml-system",
            "sfml-network",
            "sfml-window",
        }

    filter "configurations:Debug or Release"
    postbuildcommands {
        "{COPY} %{wks.location}/vendor/SFML/bin/* %{cfg.targetdir}"
    }