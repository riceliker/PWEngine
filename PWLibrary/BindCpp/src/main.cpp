#include "PWL.h"

class PWEngine
{
    private:
        PWL_Engine* engine;
    public:
        PWL_Engine* getEngine()
        {
            return engine;
        }
        PWEngine()
        {
            this->engine = PWL_CreateEngine();
        }
        ~PWEngine()
        {
            PWL_DestroyEngine(this->engine);
        }
};

class PWEWindow
{
    private:
        PWL_Window* window;
    public:
        PWEWindow(PWEngine* engine ,PWL_WindowInfo* info)
        {
            this->window = PWL_CreateWindow(engine->getEngine(), info);
        }
};

int main()
{
    PWL_TestEngineSDLRender();
}