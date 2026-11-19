#pragma once

#include "gpu.hpp"
#include "tensor/tensor.hpp"
#include <cstddef>
#include <string>

namespace PWEngine::ML 
{
    struct LinearForwordPushConstant
    {
        uint32_t input_size;
    };

    class LinearForwordTask : public GPU::GPUTask
    {
    private:
        PWEngine::GPU::GPUInstance* p_instance;
        std::unique_ptr<PWEngine::GPU::ComputePipeline> pipeline;
        std::unique_ptr<PWEngine::GPU::BindingLayout> layout;
        std::unique_ptr<PWEngine::GPU::Binding> binding;
    public:
        LinearForwordTask(GPU::GPUInstance* instance, uint32_t input_dimension, uint16_t output_dimension);
        void push(Tensor1* x, Tensor1* y, Tensor1* w, Tensor1* b);
    };
}