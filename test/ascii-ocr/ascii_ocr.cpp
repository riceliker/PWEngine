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


/*
kernel: (3, 3)
stride = (1, 1)

[in_c, in_h, in_w] -> [8, 16, 24]
```
out_h = (in_h - kernel_h) / stride_y + 1
out_w = (in_w - kernel_w) / stride_x + 1
```
[out_c, out_h, out_w] -> [8, 12, 22]


*/

/* n * 16 * 24 */

static const std::string conv_cs_code = R"(
#version 460

layout(std430, set = 0, binding = 0) buffer FloatBuffer {
    float data[];
} X;

layout(std430, set = 1, binding = 0) buffer FloatBuffer {
    float data[];
} Y;

layout(std430, set = 2, binding = 0) buffer FloatBuffer {
    float data[];
} W;

layout(std430, set = 3, binding = 0) buffer FloatBuffer {
    float data[];
} b;

layout(push_constant) uniform Const {
    uint in_c;
    uint in_h;
    uint in_w;

    uint out_c;
    uint out_h;
    uint out_w;

    uint kh;
    uint kw;
    uint stride_y;
    uint stride_x;
    uint pad_y;
    uint pad_x;
} pc;

// X[in_ch, y, x]
uint getXIdx(uint c, uint y, uint x) {
    return c * (pc.in_h * pc.in_w) + y * pc.in_w + x;
}
// W[out_ch, in_ch, dy, dx]
uint getWIdx(uint oc, uint ic, uint dy, uint dx) {
    return oc * (pc.in_c * pc.kh * pc.kw)
         + ic * (pc.kh * pc.kw)
         + dy * pc.kw
         + dx;
}
// Y[out_ch, y, x]
uint getYIdx(uint oc, uint y, uint x) {
    return oc * (pc.out_h * pc.out_w) + y * pc.out_w + x;
}

layout(local_size_x = 4, local_size_y = 4, local_size_z = 4) in;
void main() {
    uint oc = gl_GlobalInvocationID.z;
    uint yo = gl_GlobalInvocationID.y;
    uint xo = gl_GlobalInvocationID.x;

    if (oc >= pc.out_c || yo >= pc.out_h || xo >= pc.out_w) {
        return;
    }

    float sum = b.data[oc];

    for (uint ic = 0; ic < pc.in_c; ic++) {
        for (uint dy = 0; dy < pc.kh; dy++) {
            for (uint dx = 0; dx < pc.kw; dx++) {
                uint yi = yo * pc.stride_y - pc.pad_y + dy;
                uint xi = xo * pc.stride_x - pc.pad_x + dx;

                float val = 0.0;
                if (yi >= 0 && yi < pc.in_h && xi >=0 && xi < pc.in_w) {
                    val = X.data[getXIdx(ic, yi, xi)];
                }

                uint wIdx = getWIdx(oc, ic, dy, dx);
                sum += val * W.data[wIdx];
            }
        }
    }

    uint yIdx = getYIdx(oc, yo, xo);
    Y.data[yIdx] = sum;

    uint i = oc + yo + xo;
    if(i >= len) return;
    Y.data[i] = max(Y.data[i], 0.0);
}
)";

struct ConvPushConstant
{
    uint32_t in_c;
    uint32_t in_h;
    uint32_t in_w;

    uint32_t out_c;
    uint32_t out_h;
    uint32_t out_w;

    uint32_t kh;
    uint32_t kw;
    uint32_t stride_y;
    uint32_t stride_x;
    uint32_t pad_y;
    uint32_t pad_x;

    uint32_t len;
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

    {
        ConvPushConstant pc {
            .in_c = 1,
            .in_h = 16,
            .in_w = 24,

            .out_c = 8,
            .out_h = 12,
            .out_w = 22,

            .kh = 3,
            .kw = 3,
            .stride_y = 1,
            .stride_x = 1,
            .pad_y = 0,
            .pad_x = 0,

            .len = 0
        };

        auto X = std::make_unique<PWEngine::GPU::GPUBindingBuffer>(&instance, PWEngine::GPU::BufferUsage::storage, sizeof(uint32_t) * image_mat.size(), image_mat.data());
        uint32_t* Y_data = (uint32_t*)calloc(8 * 12 * 22, sizeof(uint32_t));
        auto Y = std::make_unique<PWEngine::GPU::GPUBindingBuffer>(&instance, PWEngine::GPU::BufferUsage::storage, sizeof(uint32_t) * 8 * 12 * 22, Y_data);

        ConvTask task(&instance, &pc, X.get(), Y.get(), );

        free(Y_data);
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

