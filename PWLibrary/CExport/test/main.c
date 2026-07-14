#include "PWCollections.h"
#include <stdio.h>

int main()
{
        PWL_Dictionary* dict = PWL_CreateDictionary();
        PWL_FindInDictionary(dict, "a", "1");
        PWL_FindInDictionary(dict, "b", "2");

        printf("%s\n", (char*)PWL_FindInDictionary(dict, "a", NULL));
        printf("%s\n", (char*)PWL_FindInDictionary(dict, "b", NULL));

        return 0;
}