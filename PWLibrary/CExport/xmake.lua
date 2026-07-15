add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})
set_languages("gnu23")

local make_binary = true
local VULKAN_SDK = "/Users/Riceliker/VulkanSDK/1.4.350.0/macOS"


add_requires("libsdl3", "libsdl3_image", "libsdl3_ttf")

target("PWL")
    if make_binary then
        set_kind("binary")  
    else
        set_kind("shared")
    end
    -- for export
    add_headerfiles("include/**.h")
    -- for private
    add_headerfiles("src/**/**.h")
    add_includedirs("src", "include")
    add_files("src/**/**.c")
    -- for test 
    if make_binary then
        add_files("test/main.c")
    end

    add_packages("libsdl3", {configs = {vulkan = true}})
    add_packages("libsdl3_image", "libsdl3_ttf")
    add_packages("vulkansdk")

    set_targetdir("build/")

    -- only debug
    set_symbols("debug")
    set_optimize("none")
    set_strip("none")

    add_includedirs(path.join(VULKAN_SDK, "include"))
    add_linkdirs(path.join(VULKAN_SDK, "lib"))
    add_links("vulkan")

    add_rpathdirs(path.join(VULKAN_SDK, "lib"))
    if is_plat("macosx") then
        add_links("MoltenVK")
        add_frameworks("CoreVideo", "Metal", "IOKit", "AppKit")
        add_rpathdirs("@loader_path/PWLibrary")
        add_rpathdirs("@loader_path/PWLibrary/PWLibrary/build")
    end


