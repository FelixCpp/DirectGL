project("Premake")
	kind("Utility")
	targetdir("%{wks.location}/build/bin/" .. OutputDir .. "/%{prj.name}")
	objdir("%{wks.location}/build/bin-int/" .. OutputDir .. "/%{prj.name}")

	files({
		"%{wks.location}/**/Build-*.lua",
	})

	postbuildmessage("Regenerating project files with Premake ...")
	postbuildcommands({
		-- "%{wks.location}\\Scripts\\Clean-VS2022",
		-- "%{wks.location}\\Scripts\\Build-VS2022"
	})