#pragma once

#include "core/font.hpp"
#include "core/surface.hpp"

#include <optional>
#include <string>
#include <unordered_map>

using namespace std;
namespace PWL::Core
{
    class GlobalAsset
    {
    private:
        unordered_map<string, Font*> font_dict;
        unordered_map<string, SurfacePool*> pool_dict;
    public:
        GlobalAsset()
        {
            this->font_dict = unordered_map<string, Font*>();
            this->pool_dict = unordered_map<string, SurfacePool*>();
        }
        ~GlobalAsset()
        {
            for (const auto& obj: this->font_dict)
            {
                Font* font = obj.second;
                font->Destroy();
                delete font;
            }
            for (const auto& obj: this->pool_dict)
            {
                SurfacePool* pool = obj.second;
                pool->Destroy();
                delete pool;
            }
        }
        /* preload */
        const optional<const Font* const> AddFont(string name, string path)
        {
            TTF_Font* _font = TTF_OpenFont(path.c_str(), 16);
            Font* font = new Font(_font);
            this->font_dict.insert({name, font});
            return font;
        }
        /* preload */
        const optional<const Font* const> GetFont(string name)
        {
            if (this->font_dict.find(name) != this->font_dict.end())
            {
                return this->font_dict[name];
            }
            return nullopt;
        }
        /* preload */
        const optional<const SurfacePool* const> RegistrySurfacePool(string name)
        {
            SurfacePool* pool = new SurfacePool();
            this->pool_dict[name] = pool;
            return pool; 
        }

        /* preload */
        const optional<const SurfacePool* const> GetSurfacePool(string name)
        {
            if (this->pool_dict.find(name) != this->pool_dict.end())
            {
                return this->pool_dict[name];
            }
            return nullopt; 
        }
    };
}