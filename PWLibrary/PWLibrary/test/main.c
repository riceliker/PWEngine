#include "PWL.h"
#include "core/core.h"

int main()
{
        PWL_Engine* engine = PWL_CreateEngine();

        PWL_Window* window = PWL_CreateWindow(engine, &(PWL_WindowInfo) {
                "Test Window", 800, 600, 800, 600, PWL_WINDOWMODE_RESIZABLE
        });

        

        PWL_DestroyWindow(window);
        PWL_DestroyEngine(engine);
        return 0;
}