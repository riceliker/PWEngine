#include "PWLibraryTest.h"
#include "Collections/Collections.h"
#include <stdio.h>

int main()
{
        PWL_ArrayList* list;
        PWL_InitArrayList(list);
        PWL_PushInArrayList(list, "a");
        PWL_PushInArrayList(list, "b");
        PWL_PushInArrayList(list, "c");

        PWL_PopInArrayList(list);
        PWL_PushInArrayList(list, "d");


        void* get = PWL_SetInArrayList(list, 2, "e");
        printf("pop:%s except: d\n", (char*)get);

        PWL_RemoveInArrayList(list, 1);

        PWL_InsertInArrayList(list, 0, "f");
        PWL_InsertInArrayList(list, 4, "i");
        
        PWL_ForeachInArrayList(list, {
                printf("%s,", (char*)object);
        });
        printf(" except: a,e, count: %u\n", (unsigned int)list->count);

        return 0;
}