project("DirectGL-Core")
    kind("StaticLib")
    language("C++")
    cppdialect("C++23")
    targetdir("%{wks.location}/build/bin/" .. OutputDir .. "/%{prj.name}")
    objdir("%{wks.location}/build/bin-int/" .. OutputDir .. "/%{prj.name}")

    files({
        "internal/**.ixx",
        "internal/**.cpp",
        "private/**.ixx",
        "private/**.cpp",
        "public/**.ixx",
    })

    links({
        -- DirectGL
        "DirectGL-Renderer",
        "DirectGL-ShapeRenderer",
        "DirectGL-TextureRenderer",
        "DirectGL-Logging",
        "DirectGL-Math",

        -- Utilities
        "LogForge",
        "Startup",
        "Preconditions",

        -- System
        "Context",
        "Monitor",
        "Window",

        -- Third-Party
        "Glad",
        "Stb"
    })

    includedirs({
        "%{wks.location}/Libraries/Glad/include",
        "%{wks.location}/Libraries/Stb/include"
    })

    filter("files:**.ixx")
        compileas("Module")

    filter("system:windows")
        systemversion("latest")
        links({ "opengl32" })

    filter("configurations:Debug")
        runtime("Debug")
        symbols("On")

    filter("configurations:Release")
        runtime("Release")
        optimize("On")