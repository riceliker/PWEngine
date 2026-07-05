#include "PWLibrary.h"
#include "_PWLibrary.h"

int main()
{
        PWLibrary_Engine* engine = PWLibrary_CreateEngine();

        PWLibrary_Window* window = PWLibrary_CreateWindow(engine, &(PWLibrary_WindowInfo) {
                "Test Window", 800, 600, 800, 600, PWLibrary_WINDOWMODE_RESIZABLE
        });

        

        PWLibrary_DestroyWindow(window);
        PWLibrary_DestroyEngine(engine);
        return 0;
}