project ("Freetype")
	kind("StaticLib")
	language("C")
	cdialect("C11")
	targetdir("%{wks.location}/build/bin/" .. OutputDir .. "/%{prj.name}")
	objdir("%{wks.location}/build/bin-int/" .. OutputDir .. "/%{prj.name}")
	location("%{wks.location}/Libraries/FreeType")

	defines({
		"FT2_BUILD_LIBRARY",
	--	"FT_CONFIG_OPTION_SYSTEM_ZLIB=1",
	--	"FT_CONFIG_OPTION_USE_LARGE_FORMAT=1"
	})

	files({
		"%{prj.location}/include/ft2build.h",
		"%{prj.location}/include/freetype/*.h",
		"%{prj.location}/include/freetype/config/*.h",
		"%{prj.location}/include/freetype/internal/*.h",
	
		"%{prj.location}/src/autofit/autofit.c",
		"%{prj.location}/src/base/ftbase.c",
		"%{prj.location}/src/base/ftbbox.c",
		"%{prj.location}/src/base/ftbdf.c",
		"%{prj.location}/src/base/ftbitmap.c",
		"%{prj.location}/src/base/ftcid.c",
		"%{prj.location}/src/base/ftfstype.c",
		"%{prj.location}/src/base/ftgasp.c",
		"%{prj.location}/src/base/ftglyph.c",
		"%{prj.location}/src/base/ftgxval.c",
		"%{prj.location}/src/base/ftinit.c",
		"%{prj.location}/src/base/ftmm.c",
		"%{prj.location}/src/base/ftotval.c",
		"%{prj.location}/src/base/ftpatent.c",
		"%{prj.location}/src/base/ftpfr.c",
		"%{prj.location}/src/base/ftstroke.c",
		"%{prj.location}/src/base/ftsynth.c",
		"%{prj.location}/src/base/fttype1.c",
		"%{prj.location}/src/base/ftwinfnt.c",
		"%{prj.location}/src/bdf/bdf.c",
		"%{prj.location}/src/bzip2/ftbzip2.c",
		"%{prj.location}/src/cache/ftcache.c",
		"%{prj.location}/src/cff/cff.c",
		"%{prj.location}/src/cid/type1cid.c",
		"%{prj.location}/src/gzip/ftgzip.c",
		"%{prj.location}/src/lzw/ftlzw.c",
		"%{prj.location}/src/pcf/pcf.c",
		"%{prj.location}/src/pfr/pfr.c",
		"%{prj.location}/src/psaux/psaux.c",
		"%{prj.location}/src/pshinter/pshinter.c",
		"%{prj.location}/src/psnames/psnames.c",
		"%{prj.location}/src/raster/raster.c",
		"%{prj.location}/src/sdf/sdf.c",
		"%{prj.location}/src/sfnt/sfnt.c",
		"%{prj.location}/src/smooth/smooth.c",
		"%{prj.location}/src/svg/svg.c",
		"%{prj.location}/src/truetype/truetype.c",
		"%{prj.location}/src/type1/type1.c",
		"%{prj.location}/src/type42/type42.c",
		"%{prj.location}/src/winfonts/winfnt.c",
	})
	
	includedirs({
		"%{prj.location}/include",
		"%{prj.location}/include/freetype/config",
		"%{prj.location}/src",
	})

	filter "system:windows"
		systemversion "latest"
		defines({ "_CRT_SECURE_NO_WARNINGS", "_CRT_NONSTD_NO_WARNINGS" })
		files({
			"%{prj.location}/builds/windows/ftsystem.c",
			"%{prj.location}/builds/windows/ftdebug.c",
		})

	filter("configurations:Debug")
		runtime("Debug")
		symbols("On")
	filter("configurations:Release")
		runtime("Release")
		optimize("On")