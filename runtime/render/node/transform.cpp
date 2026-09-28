#include "render.hpp"
#include "utils.hpp"
#include <cstddef>
#include <memory>
#include <vector>

namespace PWEngine::Render
{
    std::shared_ptr<Transform3D> RenderContext::createTransform3D()
    {
        auto obj = std::make_shared<Transform3D>();
        obj->m_uniform = this->createUniform(sizeof(Mesh3DNodeTransform), DescriptorType::Storage);
        return obj;
    }

    Transform3D::Transform3D()
    {
        
    }
    void Transform3D::resizeTransform(size_t size)
    {
        this->length = size;
        this->positions = std::vector<Utils::Vec3<float>>(size, {0, 0, 0});
        this->rotations = std::vector<Utils::Vec4<float>>(size, {0, 0, 0, 1});
        this->scales = std::vector<Utils::Vec3<float>>(size, {1, 1, 1});
    }

    void Transform3D::addTransform()
    {
        this->length++;
        this->positions.push_back(Utils::Vec3<float>(0, 0, 0));
        this->rotations.push_back(Utils::Vec4<float>(0, 0, 0, 1));
        this->scales.push_back(Utils::Vec3<float>(1, 1, 1));
    }

    Utils::Vec3<float> Transform3D::getPosition(size_t index)
    {
        if (this->length < index) return {0, 0, 0};
        return this->positions.at(index);
    }
    void Transform3D::setPosition(size_t index, Utils::Vec3<float> new_position)
    {
        if (this->length < index) return;
        this->positions[index] = new_position;
    }
    void Transform3D::addPosition(size_t index, Utils::Vec3<float> delta_postion)
    {
        if (this->length < index) return;
        this->positions[index] = this->positions[index] + delta_postion;
    }
    Utils::Vec3<float> Transform3D::getRotation(size_t index)
    {
        if (this->length < index) return {0, 0, 0};
        return Utils::quat2euler(this->rotations.at(index));
    }
    void Transform3D::setRotation(size_t index, Utils::Vec3<float> new_rotation)
    {
        if (this->length < index) return;
        this->rotations[index] = Utils::euler2quat(new_rotation);
    }
    void Transform3D::addRotation(size_t index, Utils::Vec3<float> delta_rotation)
    {
        if (this->length < index) return;
        this->rotations[index] = Utils::quat_normalize(Utils::quat_mul(this->rotations[index], Utils::euler2quat(delta_rotation)));
    }
    Utils::Vec3<float> Transform3D::getScale(size_t index)
    {
        if (this->length < index) return {0, 0 ,0};
        return this->scales.at(index);
    }
    void Transform3D::setScale(size_t index, Utils::Vec3<float> new_scale)
    {
        if (this->length < index) return;
        this->scales[index] = new_scale;
    }
    void Transform3D::addScale(size_t index, Utils::Vec3<float> delta_scale)
    {
        if (this->length < index) return;
        this->scales[index] = this->scales[index] + delta_scale;
    }
    void Transform3D::calculateNodeMatrix(Command* command)
    {
        std::vector<Mesh3DNodeTransform> transform_data;
        transform_data.reserve(this->length);
        for (size_t i = 0; i < this->length; ++i)
        {
            auto transform_matrix = Utils::transform(this->positions[i], this->scales[i], this->rotations[i]);
            transform_data.push_back({transform_matrix});
        }
        size_t byte_size = transform_data.size() * sizeof(Mesh3DNodeTransform);
        this->m_uniform->updateData(command, transform_data.data(), byte_size);
    }
}