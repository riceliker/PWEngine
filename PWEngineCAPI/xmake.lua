add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})
set_languages("c23")
set_warnings("all")
add_cflags("-Wunused-result", "-Wall", "-Wextra")

local VULKAN_SDK = os.getenv("HOME") .. "/VulkanSDK/1.4.350.0/macOS"

add_requires("libsdl3", "libsdl3_image", "libsdl3_ttf")

target("PWEngineCAPI")
    set_kind("shared")
    add_headerfiles("include/**.h")
    add_files("src/**.c")

    add_packages("libsdl3", {configs = {vulkan = true}})
    add_packages("libsdl3_image", "libsdl3_ttf")
    add_packages("vulkansdk")
    

    set_targetdir("build/")

    -- only debug
    set_symbols("debug")
    set_optimize("none")
    set_strip("none")

    add_includedirs(path.join(VULKAN_SDK, "include"), "include")
    add_linkdirs(path.join(VULKAN_SDK, "lib"))
    add_links("vulkan")

    add_rpathdirs(path.join(VULKAN_SDK, "lib"))

    if is_plat("macosx") then
        add_links("MoltenVK")
        add_frameworks("CoreVideo", "Metal", "IOKit", "AppKit")
        add_rpathdirs("@loader_path")
        add_rpathdirs("@loader_path/PWEngineCAPI/build")
    end


