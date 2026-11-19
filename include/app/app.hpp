// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2026 riceliker and all contributors

#include "format.hpp"
#include "gpu.hpp"
#include "math.hpp"
#include "utils.hpp"
#include <cstddef>
#include <cstdint>
#include <format>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace PWEngine::App 
{
    struct GUIObject
    {
        Vec2<uint32_t> position;
        Vec2<uint32_t> size;
        bool is_visible;
        GUIObject(Vec2<uint32_t> position, Vec2<uint32_t> size)
            : position(position), size(size) {}
        virtual void draw() = 0;
        virtual ~GUIObject() = default;
    };

    struct RectColor : public GUIObject
    {
        Vec4<uint8_t> color;
        void draw() override;
        RectColor(Vec2<uint32_t> position, Vec2<uint32_t> size, Vec4<uint8_t> color) 
            :GUIObject(position, size), color(color) {}
        virtual ~RectColor() = default;
    };

    struct RectImage : public GUIObject
    {
        Format::Image* image;
        void draw() override;
        RectImage(Vec2<uint32_t> position, Vec2<uint32_t> size, Format::Image* image)
            :GUIObject(position, size), image(image) {}
        virtual ~RectImage() = default;
    };

    template<typename COLOR, typename  IMAGE>
    inline void matchGUIObject(GUIObject* object,COLOR&& if_color, IMAGE&& )
    {

    }

    class Scene
    {
    private:
        LogSystem* log;
        std::string scene_name;
        std::unordered_map<std::string, std::unique_ptr<GUIObject>> gui_objects;
    protected:
        template<typename ... Args, typename T> requires std::is_base_of_v<GUIObject, T>
        void createGUIObject(std::string name, Args ... args)
        {
            auto iter = this->gui_objects.find(name);
            std::unique_ptr<GUIObject> object = std::make_unique<T>(std::forward<decltype(args)>(args)...);
            this->gui_objects.insert({name, std::move(object)});
        }
        void eraseGUIObject(std::string name)
        {
            auto iter = this->gui_objects.find(name);
            if (iter == this->gui_objects.end()) {
                sendLog(log, LogType::Warn, LogFrom::App, std::format("The key name(%s) is not found. Stop erase. In Scene: %s", name, scene_name));
            }
            this->gui_objects.erase(iter);
        }
        std::optional<GUIObject*> findGUIObject(std::string name)
        {

        }
    public:
        virtual void ready() = 0;
        virtual void process(double delta) = 0;
        virtual void destroy() = 0;
    };

    class App
    {
    private:
        std::unique_ptr<GPU::GPUInstance> instance;
    public:
        App(std::vector<Scene>); 
        void run(std::string project_file);
    };

    
}