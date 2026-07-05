#include "PWLibrary.h"

class PWEngine
{
    private:
        PWLibrary_Engine* engine;
    public:
        PWLibrary_Engine* getEngine()
        {
            return engine;
        }
        PWEngine()
        {
            this->engine = PWLibrary_CreateEngine();
        }
        ~PWEngine()
        {
            PWLibrary_DestroyEngine(this->engine);
        }
};

class PWEWindow
{
    private:
        PWLibrary_Window* window;
    public:
        PWEWindow(PWEngine* engine ,PWLibrary_WindowInfo* info)
        {
            this->window = PWLibrary_CreateWindow(engine->getEngine(), info);
        }
};

int main()
{
    PWLibrary_TestEngineSDLRender();
}