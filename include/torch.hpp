// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2026 riceliker and all contributors

/*
    Although PWEngine is a game engine. It also have ability to support the 
    generic GPU compute. It's make use PWEngine to realize is possible.
    The Deep Learning or Machine Learning by PWEngine is possible.

    No cuda, not private environment. PWEngine help we can load model in anywhere.
    PWEngine will try to realize simple learning, Computer Vision and even LLM.
*/

/* -------------------------------------------------------------
 | ▒███████▓   ▓████████  ████████ ▒██  ███     ██  █████████ |
 | ▒██    ███  ▓██        ██▓      ▒██  ████▓   ██  ███       |
 | ▒██     ███ ▓████████  ██▓▒▒▒▒  ▒██  ██████  ██  ████████  |
 | ▒██     ███ ▓██        ███████  ▒██  ███ ▓██ ██  ███       |
 | ▒██    ███  ▓██        ██▓      ▒██  ███   ████  ███       |
 | ▒████████   ▓████████  ██▓      ▒██  ███    ███  █████████ |
---------------------------------------------------------------*/

#include "math.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
namespace PWEngine::Torch
{
    class Tensor;
    class MathAST;
    class NeuralNetwork;

}

/*------------------------------------------
 | ░██  ████     ████  ████████   ██▓      |
 | ░██  █████   █████  ███    ██  ██▓      |
 | ░██  ██████  ██▓██  ███   ░██  ██▓      |
 | ░██  ███ ██ ██ ▓██  ████████   ██▓      |
 | ░██  ███ ░████ ▓██  ███        ██▓      |
 | ░██  ███  ███  ▓██  ███        ████████ |
-------------------------------------------*/
namespace PWEngine::Torch
{
    class Tensor
    {
    private:
        std::vector<float> data;
        std::vector<uint64_t> shape;
    public:
        Tensor();
        void arange(uint64_t size);
        void reshape(std::vector<uint64_t> shape);
        void sum(std::vector<uint64_t> axis, bool key_shape);
        void mean(std::vector<uint64_t> axis, bool key_shape);
        float normal();
        void abs();
        Tensor operator*(const Tensor &A);
        Tensor operator*(const float lambda);
        static Tensor dot(const Tensor &A, const Tensor &B);
        static Tensor mv(const Tensor &A, const Tensor &B);
        static Tensor mm(const Tensor &A, const Tensor &B);
        
    };
}