#include "Core/Asset/Asset.hpp"
#include "Core/Core.hpp"
#include "Core/Foundation/PWEAutoRect.hpp"
#include "Core/Foundation/PWEColor.hpp"
#include "Core/Foundation/PWEVecMat.hpp"
#include "Core/Scene/Scene.hpp"
#include "Core/Window/Window.hpp"
#include <cstddef>


class TestScene : PWEUIScene
{
    public:
        PWETexture icon_texture;
        TestScene(PWEWindowInfo info) : PWEUIScene(info)
        {

        }
        void init() override
        {
            icon_texture = PWETexture::createTextureByPath(this->renderer, "icon.png");
        }
        string loop() override
        {
            clearCanvas(PWEColor{255, 255, 255, 255});
        
            staticDrawRect(PWEStaticRect {
                .is_full = true,
                .rect = PWEAutoRect( true,
                    10, 10, 20, 20),
                .color=PWEColor(0, 255, 0,255),
            });

            staticDrawTexture(PWEStaticTexture {
                .texture = icon_texture,
                .pos = PWEAutoRect( true,
                    100, 100, 256, 256),
            });
            submitCanvas();
            return "";
        }
};


int main(int argc, char** argv) 
{
    PWEngine::Core::PWEWindow win = PWEngine::Core::PWEWindow("Test", {
        .logic_resolution = PWEVec2(1280, 720)
    });
    
    TestScene ts = TestScene(win.getInfo());

    win.registryScene("test", (IPWEScene*)&ts);
    win.runMainScene("test");
    
    return 0;
}
