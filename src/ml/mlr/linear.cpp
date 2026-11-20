#include "gpu.hpp"
#include "tensor/mlp.hpp"
#include <cstdint>
#include <format>
#include <string>

namespace PWEngine::ML 
{
    LinearForwordTask::LinearForwordTask(GPU::GPUInstance* instance, uint32_t input_dimension, uint16_t output_dimension) : GPU::GPUTask(instance)
    {
        this->p_instance = instance;

        std::vector<PWEngine::GPU::BindingInfo> binding_info = {
            {0, input_dimension, PWEngine::GPU::ShaderType::Compute, PWEngine::GPU::BindingType::storage},
            {1, output_dimension, PWEngine::GPU::ShaderType::Compute, PWEngine::GPU::BindingType::storage},
            {2, input_dimension * output_dimension, PWEngine::GPU::ShaderType::Compute, PWEngine::GPU::BindingType::storage},
            {3, output_dimension, PWEngine::GPU::ShaderType::Compute, PWEngine::GPU::BindingType::storage},
        };
        this->layout = std::make_unique<PWEngine::GPU::BindingLayout>(instance, binding_info);

        std::string shader_code = R"(
            "#version 450
            layout(set = 0, binding = 0) buffer Tensor { float data[]; } x;
            layout(set = 0, binding = 1) buffer Tensor { float data[]; } y;
            layout(set = 0, binding = 2) buffer Tensor { float data[]; } w;
            layout(set = 0, binding = 3) buffer Tensor { float data[]; } b;

            layout(push_constant) uniform PushConstant {
                uint input_size;
            } pc;
        )";
        shader_code += std::format("layout(local_size_x = {}, local_size_y = {}) in;", output_dimension % 256, output_dimension / 256);
        shader_code += R"(
            void main() {
                uint workgroup_index = gl_GlobalInvocationID.y * gl_WorkGroupSize.x + gl_GlobalInvocationID.x;
        )";
        shader_code += std::format("if(workgroup_index >= {}) return;", output_dimension);
        shader_code += R"(
                float sum = b.data[workgroup_index];
                for(uint i = 0; i < input_size; ++i) {
                    sum += x.data[i] * w.data[workgroup_index * input_size + i];
                }
                y.data[workgroup_index] = sum;
            }
        )";

        PWEngine::GPU::ComputePipelineInfo pipeline_info = {
            PWEngine::GPU::compilerShader(shader_code, PWEngine::GPU::ShaderType::Compute, "main").value(),
            {this->layout.get()},
            {{0, sizeof(LinearForwordPushConstant), PWEngine::GPU::ShaderType::Compute}}
        };
        this->pipeline = std::make_unique<PWEngine::GPU::ComputePipeline>(instance, pipeline_info);

        auto binding = this->layout->createBinding();
    }
    
    void LinearForwordTask::push(Tensor1 *x, Tensor1 *y, Tensor1 *w, Tensor1 *b)
    {
        binding->update(0, {x->buffer.get(), y->buffer.get(), w->buffer.get(), b->buffer.get()});
    }

     
}