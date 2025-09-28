workspace("DirectGL")
    architecture("x64")
    startproject("App")
    staticruntime("Off")
    configurations({
        "Debug",
        "Release"
    })

    flags({
        "MultiProcessorCompile",
    })

    OutputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    group("System")
        include("System/Monitor/Build-Monitor.lua")

    group("Utilities")
        include("Logging/Build-Logging.lua")
        include("Startup/Build-Startup.lua")
        include("Math/Build-Math.lua")

    group("Build-Tools")
        include("Premake/Build-Premake.lua")

    group("Graphics")
        include("Glad/Build-Glad.lua")
        include("Stb/Build-Stb.lua")

    group("") -- Root group
        include("DirectGL/Build-DirectGL.lua")
        include("App/Build-App.lua")