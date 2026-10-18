# PWEngine
The game engine for game Physic World.
Welcome to use PWEngine. It is Lite, Easy, Editable.
PWEngine support 2D(Main) and 3D Game Developing.

## Different
What the different of other game engine:
1. PWEngine is both Engine and Library.
2. PWEngine is modularization.
3. PWEngine is self-select. Choose just you want.

## File Support
1. Image
tga(native) png(stb) jpg(stb) 
2. Audio
ogg(stb) wav(midi-audio) midi(midi-audio)

## Platform
No DRM Prison:
1. PWEngine is **never** support for mobile device.  
Such as Android and iOS.  
2. PWEngine is **never** support for Game Terminal.  
Such as Xbox, PlayStation and Switch.  

| platform | Windows | macOS | Linux |
|:---:|:---:|:---:|:---:|
| API | Vulkan | Vulkan(MoltenVK) | Vulkan |
| Arch | amd64,arm64 | arm64 | amd64,arm64 |

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

