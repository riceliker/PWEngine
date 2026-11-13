#include "gpu.hpp"
#include "_command.hpp"
#include <memory>
namespace PWEngine::GPU 
{
    ResultImage::ResultImage(): self(std::make_unique<Impl>())
    {

    }
    ResultImage::~ResultImage()
    {

    }
}