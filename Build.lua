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
        include("System/Context/Build-Context.lua")
        include("System/Monitor/Build-Monitor.lua")
        include("System/Window/Build-Window.lua")

    group("Utilities")
        include("Utilities/Logging/Build-LogForge.lua")
        include("Utilities/Math/Build-Math.lua")
        include("Utilities/Preconditions/Build-Preconditions.lua")
        include("Utilities/Premake/Build-Premake.lua")
        include("Utilities/Startup/Build-Startup.lua")

    group("Libraries")
        include("Libraries/Glad/Build-Glad.lua")
        include("Libraries/Stb/Build-Stb.lua")

    group("DirectGL")
        include("DirectGL/DirectGL-Core/Build-Core.lua")
        include("DirectGL/DirectGL-Renderer/Build-Renderer.lua")

    group("") -- Root group
        include("App/Build-App.lua")