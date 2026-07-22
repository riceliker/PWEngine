
#include "public/core.hpp"
#include "SDL3/SDL_surface.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <filesystem>
#include <algorithm>

using namespace std;
namespace PWL::Core
{
    struct Image::Impl
    {
    public:
        SDL_Surface* surface;
    };
    Image::Image(void* surface)
    {
        this->impl->surface = (SDL_Surface*)surface;
    }

    struct ImageGroup::Impl
    {
    public:
        unordered_map<string, SDL_Surface*> surface;
        unordered_map<string, string> path;
        vector<string> name;
        bool is_load;
    };
    ImageGroup::ImageGroup(string folder_path)
    {
        this->impl->surface = unordered_map<string, SDL_Surface*>();
        this->impl->path = unordered_map<string, string>();
        this->impl->is_load = false;
        for (const auto& file: filesystem::directory_iterator(folder_path))
        {
            if (file.is_regular_file())
                continue;
            string file_path = file.path().string();
            string file_extension = file.path().extension().string();
            string file_name = file.path().filename().string();
            if (file_extension == ".png")
            {
                this->impl->name.push_back(file_name);
                this->impl->path.insert({file_name, file_path});
            }
        }
    }
    ImageGroup::~ImageGroup()
    {
        this->Sleep();
    }
    void ImageGroup::Load()
    {
        if (this->is_load == true)
            return;
        for (const auto& name: this->name)
        {
            SDL_Surface* temp = SDL_LoadPNG(this->path[name].c_str());
            this->surface.insert({name, temp});
        }
    }
        void ImageGroup::Sleep()
        {
            if (this->is_load == false)
                return;
            for (const auto& name: this->name)
            {
                SDL_Surface* temp = this->surface[name];
                SDL_DestroySurface(temp);
            }
            this->surface = unordered_map<string, SDL_Surface*>();
        }
        optional<shared_ptr<Image>> ImageGroup::GetImage(string name)
        {
            if (find(this->name.begin(), this->name.end(), name) != this->name.end())
            {
                SDL_Surface* temp_surface = this->surface[name];
                shared_ptr<Image> temp(new Image(temp_surface));
                return temp;
            }
            return nullopt;
        }
}