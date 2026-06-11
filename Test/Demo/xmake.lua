add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})
set_languages("c++23")

add_requires("libsdl3", "libsdl3_image", "libsdl3_ttf", "glm", {system = false})


target("Demo")
    set_kind("binary")
    add_files("src/**.cpp")
    add_includedirs("src", "include")

    add_packages("libsdl3", "libsdl3_image", "libsdl3_ttf", "glm")

    add_linkdirs("lib")
    add_links("PWEngine")

    set_targetdir("build/")

    set_symbols("debug")
    set_optimize("none")
    set_strip("none")

    -- MacOS
    local vulkan_sdk = os.getenv("VULKAN_SDK")
        if vulkan_sdk then
            add_includedirs(vulkan_sdk.."/include")
            add_linkdirs(vulkan_sdk.."/lib")
            add_links("vulkan")
            add_links("MoltenVK")
            add_frameworks("Metal","MetalKit","Foundation","AppKit")
        end
    if is_plat("macosx") or is_plat("darwin") then
        add_shflags("-install_name", "@rpath/PWEngineCpp.dylib")
        add_rpathdirs("@loader_path")             
        add_rpathdirs(vulkan_sdk.."/lib")       
        set_policy("install.rpath", false) 
        add_links("MoltenVK")
        add_frameworks("MetalKit","Foundation","AppKit")     
        add_frameworks("Cocoa", "IOKit", "CoreVideo", "CoreFoundation", "Metal", "QuartzCore")
    end

