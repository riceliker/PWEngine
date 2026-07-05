add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})
set_languages("c23")
set_warnings("all")
add_cflags("-Wunused-result", "-Wall", "-Wextra")

add_requires("libsdl3", "libsdl3_image", "libsdl3_ttf")
add_includedirs("thirdparty/stc", {public = true})

target("PWLibrary")
    set_kind("binary")
    add_headerfiles("include/**.h")
    add_files("src/**.c", "test/main.c")

    add_includedirs("thirdparty/stc/include/stc")
    add_headerfiles("thirdparty/stc/include/stc/**.h")
    add_files("thirdparty/stc/src/**.c")

    add_packages("libsdl3", {configs = {vulkan = true}})
    add_packages("libsdl3_image", "libsdl3_ttf")
    add_packages("vulkansdk")

    set_targetdir("build/")

    -- only debug
    set_symbols("debug")
    set_optimize("none")
    set_strip("none")

    local VULKAN_SDK = os.getenv("HOME") .. "/VulkanSDK/1.4.350.0/macOS"
    add_includedirs(path.join(VULKAN_SDK, "include"), "include")
    add_linkdirs(path.join(VULKAN_SDK, "lib"))
    add_links("vulkan")

    add_rpathdirs(path.join(VULKAN_SDK, "lib"))

    if is_plat("macosx") then
        add_links("MoltenVK")
        add_frameworks("CoreVideo", "Metal", "IOKit", "AppKit")
        add_rpathdirs("@loader_path/PWLibrary")
        add_rpathdirs("@loader_path/PWLibrary/PWLibrary/build")
    end


