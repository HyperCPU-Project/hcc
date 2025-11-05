add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "build"}) -- generate compile commands
add_rules("lex", "yacc")

add_requires("fmt", "bison", "gtest") -- libs
set_warnings("all") -- warns
set_languages("c++20")

add_includedirs("src") -- includes for all targets

target("hcc_core")
	set_kind("static")
	add_files(
		"src/*.cc",
		"src/ast/*.cc",
		"src/backend/*/*.cc",
		"src/backend/*.cc",
		"src/value/*.cc",
		"src/ir/*.cc"
	)
	add_files("src/ir/optimizations/*.cc")

	add_files("src/bison/*.ll", "src/bison/*.yy")

	set_pcxxheader("global_pch.hpp")

	add_packages("fmt")
target_end()


target("hcc")
	set_kind("binary")
	add_files("src/main/main.cc")

	set_pcxxheader("global_pch.hpp")

	add_deps("hcc_core")
	add_packages("fmt")
target_end()


target("hcctest")
	set_kind("binary")
	add_files(
		"tests/*.cc"
	)

	add_deps("hcc_core")
	add_packages("gtest", "fmt")

	set_default(false)
target_end()
