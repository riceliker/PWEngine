#include "render.hpp"
#include "utils.hpp"

namespace PWEngine::Render
{
    Utils::Vec3<float> Node3D::getPosition()
    {
        return this->position;
    }
    void Node3D::setPosition(Utils::Vec3<float> new_position)
    {
        this->position = new_position;
    }
    void Node3D::addPosition(Utils::Vec3<float> delta_postion)
    {
        this->position = this->position + delta_postion;
    }
    Utils::Vec3<float> Node3D::getRotation()
    {
        return Utils::quat2euler(this->rotation);
    }
    void Node3D::setRotation(Utils::Vec3<float> new_rotation)
    {
        this->rotation = Utils::euler2quat(new_rotation);
    }
    void Node3D::addRotation(Utils::Vec3<float> delta_rotation)
    {
        this->rotation = Utils::quat_normalize(Utils::quat_mul(this->rotation, Utils::euler2quat(delta_rotation)));
    }
    Utils::Vec3<float> Node3D::getScale()
    {
        return this->scale;
    }
    void Node3D::setScale(Utils::Vec3<float> new_scale)
    {
        this->scale = new_scale;
    }
    void Node3D::addScale(Utils::Vec3<float> delta_scale)
    {
        this->scale = this->scale + delta_scale;
    }
}