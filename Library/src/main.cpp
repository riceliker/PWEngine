#include "Core/Foundation/Type.hpp"
#include "Core/Surface/Surface.hpp"
#include "Core/Core.hpp"
#include "Core/Foundation/PWERect.hpp"
#include "Core/Foundation/PWEColor.hpp"
#include "Core/Foundation/PWEVecMat.hpp"
#include "Core/Scene/Scene.hpp"
#include "Core/Window/Window.hpp"
#include <cstddef>


class TestScene : PWECanvasScene
{
    public:
        PWESurface draw_surface;
        PWESurface icon_surface;
        PWEFont jetbrains_mono;
        PWESurface text_surface;
        TestScene(PWEWindowInfo info) : PWECanvasScene(info)
        {   
            jetbrains_mono = PWEFont("jbm.ttf", 32);
            icon_surface = PWESurfaceFactory::createSurfaceByImage("icon.png");
            string msg = "hello";
            text_surface = PWESurfaceFactory::createSurfaceByText(jetbrains_mono, PWEColor{0,0,0,255}, msg);
            draw_surface = PWESurfaceFactory::createEmptySurface(PWEVec2T<uint>(100, 100), PWEColor{.r=0, .g=0, .b=0, .a=0});
            draw_surface.drawRect(PWERectSize2{0, 0, 100, 100}, PWEColor{0, 0, 255, 255});
            draw_surface.drawRect(PWERectSize2{8 , 8, 84, 84}, PWEColor{0,255, 0, 255});
        }
        string loop() override
        {
            clearCanvas(PWEColor{255, 255, 255, 255});
        
            draw(icon_surface, PWEVec2T<uint>(1,1), PWEVec2T<uint>(64,64));

            draw(text_surface, PWEVec2T<uint>(100, 100));

            draw(draw_surface, PWEVec2T<uint>(300, 300));

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
