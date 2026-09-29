# Details

Date : 2026-09-29 12:16:28

Directory /Users/riceliker/Documents/PWFSP/PWEngine

Total : 56 files,  3757 codes, 453 comments, 638 blanks, all 4848 lines

[Summary](results.md) / Details / [Diff Summary](diff.md) / [Diff Details](diff-details.md)

## Files
| filename | language | code | comment | blank | total |
| :--- | :--- | ---: | ---: | ---: | ---: |
| [AGENT.md](/AGENT.md) | Markdown | 36 | 0 | 10 | 46 |
| [CONTRIBUTE.md](/CONTRIBUTE.md) | Markdown | 11 | 0 | 2 | 13 |
| [README.md](/README.md) | Markdown | 103 | 0 | 28 | 131 |
| [assets/engine_config.json](/assets/engine_config.json) | JSON | 5 | 0 | 0 | 5 |
| [build.pwm](/build.pwm) | PWMake | 31 | 0 | 5 | 36 |
| [build.sh](/build.sh) | Shell Script | 2 | 0 | 0 | 2 |
| [config.pwm](/config.pwm) | PWMake | 6 | 3 | 1 | 10 |
| [engine_pld/engine.hpp](/engine_pld/engine.hpp) | C++ | 53 | 0 | 10 | 63 |
| [engine_pld/event.cpp](/engine_pld/event.cpp) | C++ | 5 | 0 | 3 | 8 |
| [engine_pld/export.hpp](/engine_pld/export.hpp) | C++ | 66 | 6 | 11 | 83 |
| [engine_pld/init.cpp](/engine_pld/init.cpp) | C++ | 40 | 6 | 10 | 56 |
| [engine_pld/log.cpp](/engine_pld/log.cpp) | C++ | 76 | 0 | 8 | 84 |
| [include/api.hpp](/include/api.hpp) | C++ | 33 | 0 | 6 | 39 |
| [include/asset.hpp](/include/asset.hpp) | C++ | 4 | 5 | 5 | 14 |
| [include/engine.hpp](/include/engine.hpp) | C++ | 0 | 0 | 1 | 1 |
| [include/file.hpp](/include/file.hpp) | C++ | 13 | 0 | 2 | 15 |
| [include/math.hpp](/include/math.hpp) | C++ | 245 | 5 | 40 | 290 |
| [include/render.hpp](/include/render.hpp) | C++ | 323 | 104 | 27 | 454 |
| [include/stream.hpp](/include/stream.hpp) | C++ | 73 | 4 | 6 | 83 |
| [runtime/boot/boot.hpp](/runtime/boot/boot.hpp) | C++ | 0 | 0 | 1 | 1 |
| [runtime/file/_utils.hpp](/runtime/file/_utils.hpp) | C++ | 71 | 0 | 11 | 82 |
| [runtime/file/obj.cpp](/runtime/file/obj.cpp) | C++ | 91 | 0 | 4 | 95 |
| [runtime/file/shader.cpp](/runtime/file/shader.cpp) | C++ | 22 | 0 | 7 | 29 |
| [runtime/file/tga.cpp](/runtime/file/tga.cpp) | C++ | 97 | 1 | 5 | 103 |
| [runtime/render/_render.hpp](/runtime/render/_render.hpp) | C++ | 117 | 41 | 26 | 184 |
| [runtime/render/_utils.hpp](/runtime/render/_utils.hpp) | C++ | 77 | 37 | 13 | 127 |
| [runtime/render/context/buffer.cpp](/runtime/render/context/buffer.cpp) | C++ | 158 | 0 | 37 | 195 |
| [runtime/render/context/check.cpp](/runtime/render/context/check.cpp) | C++ | 88 | 0 | 23 | 111 |
| [runtime/render/context/context.cpp](/runtime/render/context/context.cpp) | C++ | 172 | 6 | 41 | 219 |
| [runtime/render/context/loop.cpp](/runtime/render/context/loop.cpp) | C++ | 32 | 0 | 8 | 40 |
| [runtime/render/context/swapchain.cpp](/runtime/render/context/swapchain.cpp) | C++ | 127 | 1 | 26 | 154 |
| [runtime/render/context/window.cpp](/runtime/render/context/window.cpp) | C++ | 13 | 0 | 2 | 15 |
| [runtime/render/input/keyborad.cpp](/runtime/render/input/keyborad.cpp) | C++ | 80 | 0 | 9 | 89 |
| [runtime/render/input/mouse.cpp](/runtime/render/input/mouse.cpp) | C++ | 31 | 0 | 1 | 32 |
| [runtime/render/instance/instance.cpp](/runtime/render/instance/instance.cpp) | C++ | 130 | 5 | 23 | 158 |
| [runtime/render/instance/validlayer.cpp](/runtime/render/instance/validlayer.cpp) | C++ | 20 | 0 | 1 | 21 |
| [runtime/render/loop/command.cpp](/runtime/render/loop/command.cpp) | C++ | 57 | 3 | 10 | 70 |
| [runtime/render/loop/frame.cpp](/runtime/render/loop/frame.cpp) | C++ | 37 | 3 | 7 | 47 |
| [runtime/render/loop/rendering.cpp](/runtime/render/loop/rendering.cpp) | C++ | 111 | 2 | 11 | 124 |
| [runtime/render/loop/singletimer.cpp](/runtime/render/loop/singletimer.cpp) | C++ | 145 | 0 | 27 | 172 |
| [runtime/render/node3D/camera.cpp](/runtime/render/node3D/camera.cpp) | C++ | 76 | 0 | 17 | 93 |
| [runtime/render/node3D/material.cpp](/runtime/render/node3D/material.cpp) | C++ | 50 | 0 | 9 | 59 |
| [runtime/render/node3D/mesh.cpp](/runtime/render/node3D/mesh.cpp) | C++ | 88 | 4 | 20 | 112 |
| [runtime/render/node3D/node.cpp](/runtime/render/node3D/node.cpp) | C++ | 45 | 0 | 10 | 55 |
| [runtime/render/node3D/texture.cpp](/runtime/render/node3D/texture.cpp) | C++ | 93 | 0 | 16 | 109 |
| [runtime/render/node3D/transform.cpp](/runtime/render/node3D/transform.cpp) | C++ | 87 | 0 | 5 | 92 |
| [runtime/render/node3D/uniform.cpp](/runtime/render/node3D/uniform.cpp) | C++ | 49 | 0 | 6 | 55 |
| [runtime/render/pipeline/pipeline.cpp](/runtime/render/pipeline/pipeline.cpp) | C++ | 218 | 13 | 38 | 269 |
| [runtime/stream/config.cpp](/runtime/stream/config.cpp) | C++ | 0 | 203 | 3 | 206 |
| [runtime/stream/log.cpp](/runtime/stream/log.cpp) | C++ | 110 | 0 | 5 | 115 |
| [shaders/pipeline2D/shader.frag](/shaders/pipeline2D/shader.frag) | glsl | 8 | 0 | 4 | 12 |
| [shaders/pipeline2D/shader.vert](/shaders/pipeline2D/shader.vert) | glsl | 14 | 0 | 4 | 18 |
| [shaders/pipeline3D/shader.frag](/shaders/pipeline3D/shader.frag) | glsl | 8 | 0 | 4 | 12 |
| [shaders/pipeline3D/shader.vert](/shaders/pipeline3D/shader.vert) | glsl | 18 | 0 | 5 | 23 |
| [test/ImGui/main.cpp](/test/ImGui/main.cpp) | C++ | 0 | 0 | 1 | 1 |
| [test/VikingRoom/main.cpp](/test/VikingRoom/main.cpp) | C++ | 92 | 1 | 23 | 116 |

[Summary](results.md) / Details / [Diff Summary](diff.md) / [Diff Details](diff-details.md)