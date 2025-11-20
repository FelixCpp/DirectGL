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

    group("Utilities")
        include("Utilities/Logging/Build-LogForge.lua")
        include("Utilities/Premake/Build-Premake.lua")

    group("Libraries")
        include("Libraries/Freetype/Build-FreeType.lua")
        include("Libraries/Glad/Build-Glad.lua")
        include("Libraries/Stb/Build-Stb.lua")
        include("Libraries/Build-Libtess2.lua")

    group("DirectGL")
        include("DirectGL/DirectGL-Core/Build-Core.lua")
        include("DirectGL/DirectGL-Input/Build-Input.lua")
        include("DirectGL/DirectGL-Logging/Build-Logging.lua")
        include("DirectGL/DirectGL-Math/Build-Math.lua")

        -- Operating System API's
        include("DirectGL/DirectGL-Context/Build-Context.lua")
        include("DirectGL/DirectGL-Monitor/Build-Monitor.lua")
        include("DirectGL/DirectGL-Window/Build-Window.lua")
        include("DirectGL/DirectGL-DPI/Build-DPI.lua")

        -- Utility Libraries
        include("DirectGL/DirectGL-Startup/Build-Startup.lua")
        include("DirectGL/DirectGL-ControlFlow/Build-ControlFlow.lua")

    group("") -- Root group
        include("App/Build-App.lua")