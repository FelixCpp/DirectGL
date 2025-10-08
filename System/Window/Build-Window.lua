project("Window")
	kind("StaticLib")
	language("C++")
	cppdialect("C++23")
	targetdir("%{wks.location}/build/bin/" .. OutputDir .. "/%{prj.name}")
	objdir("%{wks.location}/build/bin-int/" .. OutputDir .. "/%{prj.name}")

	files({
		"public/Window.ixx",
		"public/Window-NativeWindowHandle.ixx",
		"public/Window-Window.ixx",
		"public/Window-WindowEvent.ixx",
		"private/Window-Window.cpp",
	})

	links({
		"DirectGL-Input",
		"Math"
	})

	filter("system:windows")
		systemversion("latest")
		defines({ "WINDOW_PLATFORM_WINDOWS" })
		files({
			"public/Win32/Window-Win32Window.ixx",
			"private/Win32/Window-Win32Window.cpp",
		})

	filter("configurations:Debug")
		runtime("Debug")
		symbols("On")

	filter("configurations:Release")
		runtime("Release")
		optimize("On")