#include "gpu.hpp"
#include "utils.hpp"
#include "file/tga.hpp"
#include "utils.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <vector>


static const std::string conv_cs_code = R"(
#version 460

}
)";

struct ConvPushConstant
{

}; 


class ConvTask : public PWEngine::GPU::GPUTask
{
private:
    PWEngine::GPU::GPUInstance* p_instance;
    std::unique_ptr<PWEngine::GPU::ComputePipeline> compute_pipeline;
    std::unique_ptr<PWEngine::GPU::BindingLayout> layout;
    std::unique_ptr<PWEngine::GPU::Binding> binding;
public:
    ConvTask(PWEngine::GPU::GPUInstance* instance, ConvPushConstant pc,
             PWEngine::GPU::GPUBindingBuffer* X, PWEngine::GPU::GPUBindingBuffer* Y,
             PWEngine::GPU::GPUBindingBuffer* W, PWEngine::GPU::GPUBindingBuffer* b
            ): PWEngine::GPU::GPUTask(instance)
    {
        this->p_instance = instance;

        std::vector<PWEngine::GPU::BindingInfo> binding_info = {
            {0, 8*16*24, PWEngine::GPU::ShaderType::Compute, PWEngine::GPU::BindingType::storage},
            {1, 8*16*24, PWEngine::GPU::ShaderType::Compute, PWEngine::GPU::BindingType::storage},
            {2, 8*8*3*3, PWEngine::GPU::ShaderType::Compute, PWEngine::GPU::BindingType::storage},
            {3, 8, PWEngine::GPU::ShaderType::Compute, PWEngine::GPU::BindingType::storage},
        };
        this->layout = std::make_unique<PWEngine::GPU::BindingLayout>(instance, binding_info);

        PWEngine::GPU::ComputePipelineInfo pipeline_info = {
            PWEngine::GPU::compilerShader(conv_cs_code, PWEngine::GPU::ShaderType::Compute, "main").value(),
            {this->layout.get()},
            {{0, sizeof(ConvPushConstant), PWEngine::GPU::ShaderType::Compute}}
        };
        this->compute_pipeline = std::make_unique<PWEngine::GPU::ComputePipeline>(instance, pipeline_info);

        auto binding = this->layout->createBinding();
        binding->update(0, {X, Y, W, b});
    }
    void process(PWEngine::GPU::GPURecordCommand* cmd) override
    {
        cmd->bindPipeline(this->compute_pipeline.get());
        cmd->bindBinding(this->compute_pipeline.get(), {this->binding.get()});
        ConvPushConstant pc = {

        };
        cmd->pushConstant(this->compute_pipeline.get(), 0, sizeof(ConvPushConstant), &pc, PWEngine::GPU::ShaderType::Compute);
        cmd->Dispatch({4, 4, 4});
    }
};



int main()
{   
    PWEngine::LogSystem log(true);

    PWEngine::GPU::GPUInstanceInfo instance_info = {
        true, "ASCII-OCR", {1, 0, 0}
    };
    PWEngine::GPU::GPUInstance instance(&log, instance_info);

    auto [image, err] = PWEngine::Tga::TgaFile("./test/ascii-ocr/data/ascii_char.tga");
    std::array<uint32_t, 384> image_mat{};
    size_t pixel_index = 0;
    for (pixel_index = 0; pixel_index < image->data.size(); pixel_index += 4) {
        if (image->data.at(pixel_index + 0) + image->data.at(pixel_index + 1) + 
            image->data.at(pixel_index + 2) + image->data.at(pixel_index + 3) > 512) 
        {
            image_mat.at(pixel_index / 4) = 1;
        } else {
            image_mat.at(pixel_index / 4) = 0;
        }
    }


    int j = 0;
    for (const auto i : image_mat) {
        printf("%s", ((i)?"1":"0"));
        if (j == 15) {
            printf("\n");
            j = -1;
        }
        ++j;
    }
}   

