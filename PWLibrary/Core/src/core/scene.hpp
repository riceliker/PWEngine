
#include "core/surface.hpp"

#include <string>
#include <vector>

using namespace std;
namespace PWL::Core
{ 
    class IScene
    {
    public:
        SurfacePool* pool;


        virtual void Ready() = 0;
        virtual bool Loop() = 0;
        virtual string Leave() = 0;

    };
}

