#pragma once

#include "PWEType.hpp"
#include "PWEObject.hpp"

#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_video.h"
#include "SDL3_ttf/SDL_ttf.h"
#include <SDL3/SDL.h>
#include <string>
#include <unordered_map>
using namespace std;
using namespace PWEngine::Type;
namespace PWEngine::Core
{
    /*
     
    
    
     */
    typedef struct
    {
        SDL_GPUDevice* device;
        SDL_Window* window;
        SDL_Renderer* renderer;
    } PWEWindowInfo;
    
    class IPWEScene
    {
        public:
            virtual string loop() = 0;
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
        private:
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
        protected:
            SDL_Renderer* renderer;
        public:
            PWECanvasScene(PWEWindowInfo info)
            {
                this->renderer = info.renderer;

                int w, h;
                SDL_GetWindowSizeInPixels(info.window, &w, &h);
            }
            void clearCanvas(PWEColor color)
            {
                SDL_SetRenderDrawColor(this->renderer, color.r, color.g, color.b, color.a);
                SDL_RenderClear(this->renderer);
            }
            void submitCanvas()
            {
                SDL_RenderPresent(this->renderer);   
            }

            void draw(PWESurface surface, PWEVec2T<uint> pos);
            void draw(PWESurface surface, PWEVec2T<uint> pos, PWEVec2T<uint> size);

    };
    class PWEEvent
    {
        private:
        
        public: 
            void event(bool is_running);
            void onKeyPressOnce(SDL_Keycode key, SDL_Keymod mod);
            void onKeyHoldRepeat(SDL_Keycode key, SDL_Keymod mod);
            void onKeyRelease(SDL_Keycode key, SDL_Keymod mod);
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
            PWEWindowInfo getInfo(){return PWEWindowInfo {.window=this->window,.device=this->device,.renderer=this->renderer};}
            void registryScene(string unique_name, IPWEScene* scene){scenes[unique_name] = scene;}
            void runMainScene(string name);

            
    };
}