#include "PWL.h"
#include "PWLCollections.h"

struct PWL_Scene
{
        void (*Ready)(PWL_Scene* self);
        char* (*Loop)(PWL_Scene* self, float delta);
        void (*Leave)(PWL_Scene* self);
        PWL_Dictionary texture_layers;
};

void PWL_SubmitSurfaceInScene(PWL_Scene* scene, PWL_SurfacePool* pool, char* name)
{

}
