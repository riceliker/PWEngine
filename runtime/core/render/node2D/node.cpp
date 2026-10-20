#include "core/format.hpp"
#include "core/math.hpp"
#include "core/render.hpp"
#include "core/render/_render.hpp"
#include <optional>
#include <vector>

namespace PWEngine::Render
{
    Layer2D::Layer2D(Pipeline2D* pipeline) : m_descriptor_set(std::make_unique<DescriptorSet>())
    {
        this->p_pipeline = pipeline;
        this->vertices = std::vector<Vertex2D>();
        this->texture = std::nullopt;
    }   
        
    void Layer2D::addRect(Vec4<Vertex2D> rect)
    {
        this->vertices.push_back(rect.x);
        this->vertices.push_back(rect.y);
        this->vertices.push_back(rect.z);
        this->vertices.push_back(rect.w);
    }

    void Layer2D::setTexture2D(std::shared_ptr<Texture2D> texture)
    {
        this->texture = texture;
    }
    

    Layer2D::~Layer2D()
    {

    }
}