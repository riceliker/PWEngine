add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})
set_languages("c++23")
set_warnings("all")

add_requires("libsdl3", "libsdl3_image", "libsdl3_ttf", "glm", {system = false})

target("PWEngine")
    set_kind("static")
    add_headerfiles("src/**.hpp")
    add_files("src/**.cpp")

    add_includedirs("include")
    add_packages("libsdl3", "libsdl3_image", "libsdl3_ttf", "glm")

    set_targetdir("build/")

    -- only debug
    set_symbols("debug")
    set_optimize("none")
    set_strip("none")


