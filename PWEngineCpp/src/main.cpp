#include "PWEngineCAPI.h"

class PWEngine
{
    private:
        PWEngineCAPI_Engine* engine;
    public:
        PWEngineCAPI_Engine* getEngine()
        {
            return engine;
        }
        PWEngine()
        {
            this->engine = PWEngineCAPI_CreateEngine();
        }
        ~PWEngine()
        {
            PWEngineCAPI_DestroyEngine(this->engine);
        }
};

class PWEWindow
{
    private:
        PWEngineCAPI_Window* window;
    public:
        PWEWindow(PWEngine* engine ,PWEngineCAPI_WindowInfo* info)
        {
            this->window = PWEngineCAPI_CreateWindow(engine->getEngine(), info);
        }
};

int main()
{
    PWEngineCAPI_TestEngine();
}