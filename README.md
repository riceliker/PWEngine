# PWEngine
Welcome to use PWEngine. It is Lite, No-black-box and freedom.  
PWEngine is a libraries group(Like LLVM). and the core is game engine.  
PWEngine support 2D(Main) and 3D Game Developing.  
PWEngine will support common GPU compute and machine learning.  

## Different
What the different of other game engine:
1. PWEngine is both Engine and Library.
2. PWEngine is modularization.
3. PWEngine is self-select. Choose just you want.

## How to start
Read PWEngine Document for help.

## Platform
No DRM Prison:
1. PWEngine is **never** support for mobile device.  
Such as Android and iOS.  
Android is not open! see [https://keepandroidopen.org](https://keepandroidopen.org).  
2. PWEngine is **never** support for Game Terminal.  
Such as Xbox, PlayStation and Switch.  

| platform | Windows | macOS | Linux | FreeBSD |
|:---:|:---:|:---:|:---:|:---: |
| API | Vulkan | Vulkan(MoltenVK) | Vulkan | Planning |
| Arch | amd64,arm64 | arm64 | amd64,arm64 | Planning |
| ENV | MinGW | Native | Native | Planning |

## License
Simply Mean:
Use header file in include folder to create game. -> Allow Close Source;
Modify engine or create plug-in. -> Should Open Source.

PWEngine use LGPL3.0 + GPL3.0 license.  

You can allocate you code freely in that cases:  
1. Include only **public** header file and link the dynamic library.  
2. Include only **public** header file and link the static library with object file list. 

You should make you program as GPL3.0 too.
1. Include the **private** header file. The all file `_xxx.hpp` is private header file.  
2. Modify the engine source code. Write the engine plug-in.  

