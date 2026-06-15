/**
 * 
 */

#pragma once

#include "PWEType.hpp"
#include "PWEObject.hpp"

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include "SDL3_ttf/SDL_ttf.h"
#include <SDL3/SDL.h>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;
using namespace PWEngine::Type;
namespace PWEngine::Core
{
    typedef struct
    {
        SDL_GPUDevice* device;
        SDL_Window* window;
        SDL_Renderer* renderer;
        PWEVec2T<uint> window_resolution;
    } PWEWindowInfo;

    typedef struct
    {
        SDL_Keycode key;
        SDL_Keymod mod;
    } PWEInputEventPackage;
    
    class IPWEScene
    {
        public:
            virtual string loop(SDL_Event event) = 0;
    };
    class PWEFont : public PWERefCountPtr<TTF_Font>
    {
        public:
            PWEFont(PWEFont&& other) noexcept : PWERefCountPtr<TTF_Font>(std::move(other))
            {
                if (other.get()) *this = other;
            }

            PWEFont& operator=(PWEFont&& other) noexcept
            {
                if (this != &other) PWERefCountPtr<TTF_Font>::operator=(other);
                return *this;
            }

            PWEFont(const PWEFont&) = default;
            PWEFont& operator=(const PWEFont&) = default;
            static void Destroy(TTF_Font* font)
            {
                if(font) TTF_CloseFont(font);
            }
            explicit PWEFont(TTF_Font* font) : PWERefCountPtr(font, Destroy){}
            PWEFont() : PWERefCountPtr(nullptr, Destroy) {}
            PWEFont(const string& path, int size) : PWEFont(nullptr)
            {
                TTF_Font* t_font = TTF_OpenFont(path.c_str(), size);
                if(t_font == nullptr) SDL_LogWarn(-1, "Asset Font not found path(%s)", path.c_str());
                else *this = PWEFont(t_font);
            }
            
    };
    
    class PWESurface : public PWERefCountPtr<SDL_Surface>
    {
        public:
            PWESurface() = default;
            explicit PWESurface(SDL_Surface* suf) : PWERefCountPtr(suf, SDL_DestroySurface){}
            
            PWESurface(PWESurface &surface);

            void clearSurface(PWEColor color);
            void drawRect(PWERectSize2 rect, PWEColor color);
            void drawSurface(PWESurface surface, PWEVec2T<uint> pos);
            void drawSurface(PWESurface surface, PWEVec2T<uint> pos, PWERectSize2 src_area);
            
    };
    [[deprecated("use PWESurfaceFactory")]];
    class PWETexture : public PWERefCountPtr<SDL_Texture>
    {    
        public:
            PWETexture() = default;
            explicit PWETexture(SDL_Texture* tex) : PWERefCountPtr(tex, SDL_DestroyTexture){}

            tuple<int, int> getSize();  
            [[deprecated]]
            PWETexture(SDL_Renderer* renderer, string path);
            [[deprecated("Use render surface")]]
            PWETexture(SDL_Renderer* renderer, PWESurface surface);   
    };

    class PWESurfaceFactory
    {
        public:
            static PWESurface createSurfaceByImage(string path);
            static PWESurface createSurfaceByText(PWEFont font, PWEColor color, string text);
            static PWESurface createEmptySurface(PWEVec2T<uint> size, PWEColor color);
            static PWESurface copySurface(PWESurface surface);
    };
    class PWECanvasScene : public IPWEScene
    {
        private:
            vector<SDL_Surface*> surfaces;
            SDL_Renderer* renderer;
        protected:
            void draw(PWESurface surface, PWEVec2T<uint> pos);
            void draw(PWESurface surface, PWEVec2T<uint> pos, PWEVec2T<uint> size);
        public:
            PWECanvasScene(PWEWindowInfo info);
            void clearCanvas(PWEColor color);
            void addSurface(PWESurface surface);
            void submitCanvas();
    }; 
   
    class PWEEvent
    {
        private:
            SDL_Keycode on_pressed_key;
            SDL_Keymod on_pressed_mod;
            uint mouse_x;
            uint mouse_y;
        public: 
            PWEInputEventPackage getPackage();
            tuple<bool, SDL_Event> event(bool is_running);
    };
    class PWEWindow
    {
        typedef struct
        {
            PWEVec2 logic_resolution;
            SDL_RendererLogicalPresentation presentation = SDL_LOGICAL_PRESENTATION_LETTERBOX;
        } PWEWindowDesc;
        private: 
            void startInfo();
            // config
            PWEVec2T<uint> window_resolution = PWEVec2T<uint>(1280, 720);
            PWEVec2T<uint> logical_resolution = PWEVec2T<uint>(1280, 720);
            // basic handle
            SDL_Window* window;
            SDL_GPUDevice* device;
            SDL_Renderer* renderer;
            PWEEvent eventbus;
            // status
            bool thread_is_running = true;
            // scene
            unordered_map<string, IPWEScene*> scenes;
            // render
            void render(IPWEScene* scene);
        public:
            PWEWindow(string name, PWEWindowDesc desc);
            ~PWEWindow();
            // set config
            void configWindowResolution(PWEVec2T<uint> window_resolution){this->window_resolution=window_resolution;}
            // out info
            PWEWindowInfo getInfo(){return PWEWindowInfo {.window=this->window,.device=this->device,.renderer=this->renderer, .window_resolution=this->window_resolution};}
            void registryScene(string unique_name, IPWEScene* scene){scenes[unique_name] = scene;}
            void runMainScene(string name);

            
    };
}