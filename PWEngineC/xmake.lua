add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})
set_languages("c23", "c++23")
set_warnings("all")

local VULKAN_SDK = os.getenv("HOME") .. "/VulkanSDK/1.4.350.0/macOS"

add_requires("libsdl3", "libsdl3_image", "libsdl3_ttf", "glm", {system = false})

target("PWEngine")
    set_kind("binary")
    add_headerfiles("src/**.hpp", "src/**.h")
    add_files("src/**.cpp", "src/**.c")

    add_packages("libsdl3", {configs = {vulkan = true}})
    add_packages("libsdl3_image", "libsdl3_ttf", "glm")
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
    end


