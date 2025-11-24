add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "build"}) -- generate compile commands
add_rules("lex", "yacc")

set_policy("build.sanitizer.address", true)
set_policy("build.sanitizer.leak", true)

add_requires("fmt", {external=false})
add_requires("bison", "catch2", "mapbox_eternal", "tl_expected") -- libs
set_warnings("all") -- warns
set_languages("c++20")

add_includedirs("src") -- includes for all targets

target("hcc_core")
	set_kind("static")
	add_files(
		"src/*.cpp",
		"src/ast/*.cpp",
		"src/backend/*/*.cpp",
		"src/backend/*.cpp",
		"src/value/*.cpp",
		"src/driver/*.cpp",
		"src/ir/*.cpp", "src/ir/optimizations/*.cpp",
		"src/regallocator/*.cpp",
		"src/lexer/*.cpp",
		"src/parser/*.yy"
	)

	add_packages("fmt", "mapbox_eternal", "tl_expected")
target_end()


target("hcc")
	set_kind("binary")
	add_files("src/main/main.cpp")

	add_deps("hcc_core")
	add_packages("fmt", "mapbox_eternal", "tl_expected")
target_end()


target("hcc_test")
	set_kind("binary")
	add_files(
		"tests/*.cpp"
	)

	add_deps("hcc_core")
	add_packages("catch2", "fmt", "mapbox_eternal", "tl_expected")

	set_default(false)
target_end()
