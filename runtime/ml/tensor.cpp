#include "tensor/tensor.hpp"
#include "gpu.hpp"
#include "utils.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace PWEngine::ML
{
    Tensor1::Tensor1(GPU::GPUInstance* instance, std::vector<float> data)
    {
        this->p_instance = instance;
        if (data.size() > 65536) 
            sendLog(instance->log, LogType::Warn, LogFrom::Tensor, "The max size of a Tensor1 is 65536. If more than, try to use Tensor2.");
        
        this->buffer = std::make_unique<GPU::GPULocalBuffer>(instance, GPU::BufferUsage::storage, sizeof(float) * data.size(), data.data());
        this->shape = {(uint16_t)data.size()};
    }

    void Tensor1::reshape(std::vector<uint16_t> shape)
    {
        if (shape.size() > 64)
            sendLog(this->p_instance->log, LogType::Warn, LogFrom::Tensor, "The max size of a Tensor1 is 65536. If more than, try to use Tensor2.");
     
        this->shape = shape;
    }
    
}