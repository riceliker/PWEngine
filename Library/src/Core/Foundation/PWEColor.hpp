#pragma once

namespace PWEngine::Core 
{  
    class PWEColor
    {
        public:
            int r; int g; int b; int a;
            PWEColor(int r, int g, int b, int a)
            {
                this->r = to255(r);
                this->g = to255(g);
                this->b = to255(b);
                this->a = to255(a);
            }
            int to255(int x)
            {
                if(0 > x) return 0;
                if(x > 255) return 255;
                return x;
            }
    };
}