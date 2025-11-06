add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "build"}) -- generate compile commands
add_rules("lex", "yacc")

add_requires("fmt", "bison", "gtest") -- libs
set_warnings("all") -- warns
set_languages("c++20")

add_includedirs("src/Core", "src/Common", "src") -- includes for all targets

target("hcc_common")
	set_kind("static")
	add_files(
		"src/Common/*.cc"
	)

	set_pcxxheader("global_pch.hpp")

	add_packages("fmt")
target_end()

target("hcc_core")
	set_kind("static")
	add_files(
		"src/Core/*.cc",
		"src/Core/ast/*.cc",
		"src/Core/backend/*/*.cc",
		"src/Core/backend/*.cc",
		"src/Core/ir/*.cc"
	)
	add_files("src/Core/ir/optimizations/*.cc")

	add_files("src/Core/bison/*.ll", "src/Core/bison/*.yy")

	set_pcxxheader("global_pch.hpp")

	add_deps("hcc_common")
	add_packages("fmt")
target_end()


target("hcc")
	set_kind("binary")
	add_files("src/Main/main.cc")

	set_pcxxheader("global_pch.hpp")

	add_deps("hcc_core")
	add_packages("fmt")
target_end()


target("hcc_test")
	set_kind("binary")
	add_files(
		"tests/*.cc"
	)

	add_deps("hcc_core")
	add_packages("gtest", "fmt")

	set_default(false)
target_end()

