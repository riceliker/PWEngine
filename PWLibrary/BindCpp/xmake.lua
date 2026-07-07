add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})
set_languages("c++23")
set_warnings("all")


target("PWEngineCpp")
    set_kind("binary")
    set_languages("c++23")

    add_includedirs("./lib", "./include")
    add_includedirs("/opt/homebrew/include")
    add_linkdirs("/opt/homebrew/lib")
    
    add_headerfiles("src/**.h", "lib/**.h")
    add_files("src/**.cpp")

    add_linkdirs("./lib")
    add_links("PWLibrary")

    set_targetdir("build/")

    if is_plat("macosx") then
        add_rpathdirs("@executable_path/../lib")
    end