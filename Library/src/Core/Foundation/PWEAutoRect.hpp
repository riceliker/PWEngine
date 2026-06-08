#pragma once

#include "Core/Foundation/PWEVecMat.hpp"
namespace PWEngine::Core 
{
    class PWEAutoRect
    {
        private:
            PWEVec2 _start = PWEVec2(0,0);
            PWEVec2 _end = PWEVec2(0,0);
            PWEVec2 _size = PWEVec2(0,0);
        public:
            PWEVec2 start() const {return _start;}
            PWEVec2 end() const {return _end;}
            PWEVec2 size() const {return _size;}
            void start(PWEVec2 start)
            {
                _start = start;
                _end = _start + _size;
            }
            void size(PWEVec2 size)
            {
                _size = size;
                _end = _start + _size;
            }
            void end(PWEVec2 end)
            {
                _end = end;
                _size = _end - _start;
            }
            PWEAutoRect(bool sizeT_endF, float a, float b, float c, float d)
            {
                _start = PWEVec2(a, b);
                if(sizeT_endF)
                {
                    this->size(PWEVec2(c, d));
                }
                else 
                {
                    this->end(PWEVec2(c, d));
                }
            }
    };
}