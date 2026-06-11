#include "PWECore.hpp"

using namespace PWEngine::Core;
class TestScene : PWECanvasScene
{
    public:
        PWESurface draw_surface;
        PWEFont jetbrains_mono;
        PWESurface text_surface;
        PWESurface icon_surface;
        
        TestScene(PWEWindowInfo info) : PWECanvasScene(info)
        {   
            draw_surface = PWESurfaceFactory::createEmptySurface(PWEVec2T<uint>(800, 500), PWEColor{.r=0, .g=0, .b=0, .a=0});
            draw_surface.drawRect(PWERectSize2{0, 0, 800, 200}, PWEColor{0, 0, 0, 255});
            draw_surface.drawRect(PWERectSize2{2 , 2, 796, 196}, PWEColor{255,255, 0, 127});
            jetbrains_mono = PWEFont("jbm.ttf", 32);
            icon_surface = PWESurfaceFactory::createSurfaceByImage("icon512.png");   
        }
        string loop() override
        {
            clearCanvas(PWEColor{255, 255, 255, 255});
            draw(draw_surface, PWEVec2T<uint>(16, 16));
            draw(icon_surface, PWEVec2T<uint>(18,18), PWEVec2T<uint>(196,196));

            text_surface = PWESurfaceFactory::createSurfaceByText(jetbrains_mono, PWEColor{0,0,0,255}, "hello world");
            draw(text_surface, PWEVec2T<uint>(250, 18));

            submitCanvas();
            return "";
        }
};


int main(int argc, char** argv) 
{
    PWEWindow win = PWEWindow("Test", {
        .logic_resolution = PWEVec2(1280, 720)
    });
    
    TestScene ts = TestScene(win.getInfo());

    win.registryScene("test", (IPWEScene*)&ts);
    win.runMainScene("test");
    
    return 0;
}
