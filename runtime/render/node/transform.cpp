#include "render.hpp"
#include <cstddef>
#include <memory>

namespace PWEngine::Render
{
    std::shared_ptr<Transform3D> RenderContext::createTransform3D()
    {
        auto obj = std::make_shared<Transform3D>();
        obj->m_uniform = this->createUniform(sizeof(Mesh3DNodeTransform));
        return obj;
    }

    Transform3D::Transform3D()
    {
        this->position = {0, 0, 0};
        this->rotation = {0, 0, 0, 1};
        this->scale = {1, 1, 1};
    }
    Utils::Vec3<float> Transform3D::getPosition()
    {
        return this->position;
    }
    void Transform3D::setPosition(Utils::Vec3<float> new_position)
    {
        this->position = new_position;
    }
    void Transform3D::addPosition(Utils::Vec3<float> delta_postion)
    {
        this->position = this->position + delta_postion;
    }
    Utils::Vec3<float> Transform3D::getRotation()
    {
        return Utils::quat2euler(this->rotation);
    }
    void Transform3D::setRotation(Utils::Vec3<float> new_rotation)
    {
        this->rotation = Utils::euler2quat(new_rotation);
    }
    void Transform3D::addRotation(Utils::Vec3<float> delta_rotation)
    {
        this->rotation = Utils::quat_normalize(Utils::quat_mul(this->rotation, Utils::euler2quat(delta_rotation)));
    }
    Utils::Vec3<float> Transform3D::getScale()
    {
        return this->scale;
    }
    void Transform3D::setScale(Utils::Vec3<float> new_scale)
    {
        this->scale = new_scale;
    }
    void Transform3D::addScale(Utils::Vec3<float> delta_scale)
    {
        this->scale = this->scale + delta_scale;
    }
    void Transform3D::calculateNodeMatrix(Command* command)
    {
        auto transform_matrix = Utils::transform(this->position, this->scale, this->rotation);
        Mesh3DNodeTransform transform_data = { transform_matrix };
        this->m_uniform->updateData(command, &transform_data, sizeof(transform_data));
    }
}