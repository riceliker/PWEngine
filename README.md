# PWEngine

![License](https://img.shields.io/badge/License-LGPL3.0-purple)
![C](https://img.shields.io/badge/C-23-green)
![C++](https://img.shields.io/badge/C++-23-blue)
![Rust](https://img.shields.io/badge/Rust-24-orange)
![Version](https://img.shields.io/badge/Version-0.1.0-green)


**PWEngine is licensed under the GNU Lesser General Public License v3.0 Only(LGPLv3).**  
**See COPYING (GPLv3 base) and COPYING.LESSER (LGPLv3 additional terms) for details.**  

Each contributor retains copyright of their respective contributions.  
No copyright assignment is required to contribute.  
This project intentionally adopts distributed copyright model to prevent full acquisition and privatization.  

## What is PWEngine.
PWEngine is a SDK include:   
1. A Lite Simple 2D Game/GUI Library: PWL.   
2. A Modules 3D Runtime: PWEngineRuntime. 
3. The Editor Utils.  

### PWL 
PWL is a simple library and base of PWEngine. Basic by SDL.  
PWL is write by C language. Because it can be use for C++, Rust, Zig.  
*C is a simple language which can let me focus on underlying logical.*
PWL have bind layer to support for C++, Rust, Zig.  
*Use C++, Rust, Zig focus on RAII, refcount and business design.*

### PWEngineRuntime
PWEngineRuntime is a Vulkan runtime. Not independent the SDL API.
PWEngineRuntime use C++ only. Not support Rust.

### PWEngine Editor Utils
PWEngine Editor Utils is a group of editor. Write by PWL.
We don't like a huge editor. The different module need in different editor.

## Programming Language & Framework
We both like C++ & Rust. Do not talk about who is better.  
Official Support Language: C, C++, Rust, Zig. 
Notice: PWEngine dependent on clang/gcc. Not support by MSVC.
*MSVC is a terrible C/C++ compiler. And it is not open source.*
If you are a windows developer, you should install MSYS2 and MinGW.

### PWL 
PWL basic by C only, bind C++, Rust, Zig.
### PWEngineRuntime
PWEngineRuntime basic by C++ only.
### PWEngine Editors Utils
PWEngine Editors Utils support three official language C++, Rust, Zig.

## Standard Assets Format
PWEngine support developer use Standard Format.
1. Image: `.png` `.bmp` File -> GIMP Krita
2. 3D Model: `.glb` `.obj` File -> Blender
3. Audio: `.ogg` File
4. Font: `.ttf` `.otf` File
5. Shader: `.glsl` File
6. Video: `.webm` File (If have) -> Kdenlive, FFmpeg

## Use Open Source Developing Environment
You can developing PWEngine in GNU/Linux
System: GNU/Linux
StandardLibrary: glibc
C/C++ Compiler: gcc/clang
C/C++ Make Program: xmake(will be abandon),PWMake(ninjia)
Code Editor: codium(have plug-in), emacs, (neo)vim
ImageEditor: GIMP/Krita
3DModel: Blender
...

## NO DRM (Digital Restrictions Management)
![DRM-FREE](./Assets/DRM_FREE.png)

PWEngine NEVER allow any code for DRM, include:
1. DRM Platform/Device: such as iOS, XBox, PlayStation, Switch etc.
2. DRM Tool: Anti-cheating, Code obfuscation.
3. DRM MEDIA: The media format must not have any DRM.

PWEngine never support this code, do not pull your PR which include them.

## End
We want to create a free game for everyone. PWEngine is the base of it.
Help PWEngine, we need more developers who know Vulkan, Render Engine and Physics Engine.
Write you Open Source Game by PWEngine. It's the best way for help us.

Enjoy the developing.
