/**
 * @file Collections.h
 * @author riceliker 
 * @brief 
 * @version 0.1
 * @date 2026-07-09 
 * 
 * @copyright Copyright (c) 2026 riceliker. Open Source License: LGPL3.0
 * 
 */
/*
 * Copyright (C) 2026 riceliker
 *
 * This file is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser GPL along
 * with this file. If not, see <https://www.gnu.org/licenses/>.
 */


/// ----- ArrayList Document ------------------------------------------------------------------------------------------ ///
//|
//|     ArrayList is a simply dynamic list in PWLibrary. It only support the pointer.
//|     It not only the common list, but also is a stack or query.
//|     ----------------
//|     ```C
//|     PWL_ArrayList* list;
//|     PWL_InitArrayList(list)
//|     /* Add element in the end */
//|     PWL_PushInArrayList(list, "a");
//|     PWL_PushInArrayList(list, "b");
//|     PWL_PushInArrayList(list, "c");
//|     /* Get element in the end and remove last element*/
//|     PWL_PopInArrayList(list);
//|     /* Append element in the end*/
//|     PWL_PushInArrayList(list, "d");
//|     /* Set the element in the list and return the old element. It maybe will free or do other thing. */
//|     void* get = PWL_SetInArrayList(list, 2, "e");
//|     printf("pop:%s except: d\n", (char*)get);
//|     /* Remove the element in the list and return the old element. It maybe will free or do other thing. */
//|     void* get = PWL_RemoveInArrayList(list, 1);
//|     /* Insert the elemnet in list. The element will stay in index. Other behind will be move. */
//|     PWL_InsertInArrayList(list, 0, "f");
//|     /* Out of list, same as PWL_SetInArrayList() */
//|     PWL_InsertInArrayList(list, 4, "i");
//|     /* The macro to foreach the list, you will get two variable `index` and `object`*/
//|     PWL_ForeachInArrayList(list, {
//|             printf("%d:%s,", index, (char*)object);
//|     });
//|     /* Use `list->count` to get length of list*/
//|     printf(" except: a,e, count: %u\n", (unsigned int)list->count);
//|     ```
//|     ----------------
//|     It support create in heap, remember delete.
//|     ----------------
//|     ```C
//|     PWL_ArrayList* list = PWL_CreateArrayList();
//|     ...
//|     /*If the element is in the heap. It will help you free then, don't worry about NULL pointer. */
//|     PWL_FreeArrayList(list);
//|     PWL_DestroyArrayList(list);
//|     ```
//|     ----------------
//|
/// -------------------------------------------------------------------------------------------------------------------- ///
/// ------ Dictionary Document ----------------------------------------------------------------------------------------- ///
//|
//|   
/// -------------------------------------------------------------------------------------------------------------------- ///
/// ------ HashMap Document -------------------------------------------------------------------------------------------- ///

#pragma once

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// >>>>>| Log System |=============================================================================================== ///

static inline void PWL_LogWarn(const char *fmt)
{
        printf("\033[33m" "WARN:%s" "\033[0m" "\n", fmt);     
}


static inline void* PWL_CheckNull(void* ptr, const char* name)
{
        if(ptr == NULL)
        {
                printf("\033[33m" "WARN:The pointer(%s) is null." "\033[0m" "\n", name); 
        }
        return ptr;
}

#define PWL_ReturnNull(ptr, name, obj) \
do{ \
        PWL_CheckNull((ptr), name); \
        if (ptr == NULL) return obj; \
}while(0);

#define PWL_ReturnNullVoid(ptr, name) \
do{ \
        PWL_CheckNull((ptr), name); \
        if (ptr == NULL) return; \
}while(0);



/// >>>>>| Global Macro |=============================================================================================== ///

/* Extern C macro template */

#ifdef __cplusplus
#  define PW_EXTERN_C_BEGIN extern "C" {
#  define PW_EXTERN_C_END   }
#else
#  define PW_EXTERN_C_BEGIN
#  define PW_EXTERN_C_END
#endif

/* Public struct define */
#ifdef __cplusplus
#  define PWL_STRUCT(Name) struct Name;
#else
#  define PWL_STRUCT(Name) typedef struct Name Name; struct Name;
#endif

/* Public enum define */
#ifdef __cplusplus
#  define PWL_ENUM(Name) enum Name
#else
#  define PWL_ENUM(Name) typedef enum Name Name; enum Name
#endif

/// >>>>>| ArrayList |================================================================================================= ///

/**
 * @brief PWL_ArrayList
 * The PWL_ArrayList object. 
 * @param list_ptr <void**> The start pointer for void* list in heap.
 * @param count <size_t> The count of the list.
 * @param memory_count <size_t> The count of the memory.
 */
PWL_STRUCT(PWL_ArrayList)
struct PWL_ArrayList
{
        void** list_ptr;
        size_t count;
        size_t memory_count;
};
/**
 * @brief PWL_InitArrayList
 * Create and init ArrayList in Stack.
 * @param empty_ptr the stack ptr
 * @return <PWL_ArrayList*> handle
 */
PWL_ArrayList* PWL_InitArrayList(PWL_ArrayList* empty_ptr)
{
        empty_ptr->list_ptr = malloc(8 * sizeof(void*));
        empty_ptr->count = 0;
        empty_ptr->memory_count = 8;
        return empty_ptr;
}

/**
 * @brief PWL_CreateArrayList
 * Create and init ArrayList in heap. It's store in the heap, you should destroy by PWL_DestroyArrayList.
 * @return <PWL_ArrayList*> [need destroy] handle
 */
PWL_ArrayList* PWL_CreateArrayList(void)
{
        void* void_ptr = malloc(sizeof(PWL_ArrayList));
        PWL_ArrayList* array_list_ptr = (PWL_ArrayList*)PWL_CheckNull(void_ptr, "PW_LArrayList");
        array_list_ptr->list_ptr = malloc(8 * sizeof(void*));
        array_list_ptr->count = 0;
        array_list_ptr->memory_count = 8;
        return array_list_ptr;
}
/**
 * @brief PWL_DestroyArrayList
 * Destroy the ArrayList. You must do it to avoid the memory leak.
 * @param ptr <PWL_ArrayList*> [Move owner] ArrayList handle 
 */
void PWL_DestroyArrayList(PWL_ArrayList* ptr)
{
        PWL_ReturnNullVoid(ptr, "PWL_ArrayList");
        free(ptr);
}
/**
 * @brief PWL_CacheInArrayList
 * Add the memory count. When you want to store many data, you can apply the more memory and less apply.
 * @param ptr <PWL_ArrayList*> [Mut Borrow] ArrayList handle 
 * @param size <size_t> [const] The size of new memory. size * sizeof(void*)
 */
void PWL_CacheInArrayList(PWL_ArrayList* ptr, const size_t size)
{
        PWL_CheckNull(ptr, "PWL_ArrayList");
        size_t new_memory_count = ptr->memory_count + size;
        void** new_list_ptr = realloc(ptr->list_ptr, new_memory_count * sizeof(void*));
        PWL_CheckNull(new_list_ptr, "PW_ArrayList->list_ptr");
        ptr->list_ptr = new_list_ptr;
        ptr->memory_count = new_memory_count;
}
/**
 * @brief PWL_GetInArrayList
 * Get object by index in ArrayList. It will check the index less than count. If not, return NULL.
 * @param ptr <PWL_ArrayList*> [Mut Borrow] ArrayList handle
 * @param index <size_t> [const] The index in list
 */
void* PWL_GetInArrayList(PWL_ArrayList* ptr, const size_t index)
{
        PWL_CheckNull(ptr, "PWL_ArrayList");
        if (index >= ptr->count)
        {
                PWL_LogWarn("Out of the index");
                return NULL;
        }
        return ptr->list_ptr[index];
}
/**
 * @brief PWL_SetInArrayList
 * Set element data in element from ArrayList.
 * It will return the old value and help you delete it.  
 * @param ptr <PWL_ArrayList*> [Mut Borrow] ArrayList handle
 * @param index <size_t> [const] The index in list
 * @param element <void*> [const] The element ptr
 * @return <void*> old element 
 */
void* PWL_SetInArrayList(PWL_ArrayList* ptr, const size_t index, void* element)
{
        PWL_CheckNull(ptr, "PWL_ArrayList");
        void* pop_obj = NULL;
        if (index <= ptr->count)
        {
                pop_obj = PWL_GetInArrayList(ptr, index);
                ptr->list_ptr[index] = element;
        }
        else
        {
                if (index >= ptr->memory_count)
                        PWL_CacheInArrayList(ptr, (index + 8) * sizeof(void*));

                ptr->list_ptr[index] = element;
                ptr->count = index + 1;
        }
        return pop_obj;
}
/**
 * @brief PWL_InsertInArrayList
 * Insert element in list, and other element in behind will be moved.
 * @param ptr <PWL_ArrayList*> [Mut Borrow] ArrayList handle
 * @param index <size_t> [const] The index in list
 * @param element <void*> [const] The element ptr
 */
void PWL_InsertInArrayList(PWL_ArrayList* ptr, size_t index, void* element)
{
        PWL_CheckNull(ptr, "PWL_ArrayList");
        if (index >= ptr->count)
        {
                PWL_SetInArrayList(ptr, index, element);
                return;
        }
        else 
        {
                if(ptr->count == ptr->memory_count)
                {
                        PWL_CacheInArrayList(ptr, (ptr->memory_count + 8) * sizeof(void*));
                }
                size_t head = ptr->count - index;
                if (head > 0)
                        memmove(&ptr->list_ptr[index + 1], &ptr->list_ptr[index], head * sizeof(void*));
                ptr->list_ptr[index] = element;
                ++(ptr->count);
        }
}
/**
 * @brief PWL_RemoveInArrayList
 * Remove the element in the list, and other element in behind will be moved.
* It will return the old value and help you delete it.  
 * @param ptr <PWL_ArrayList*> [Mut Borrow] ArrayList handle
 * @param index <size_t> [const] The index in list
 */
void* PWL_RemoveInArrayList(PWL_ArrayList* ptr, size_t index)
{
        PWL_CheckNull(ptr, "PWL_ArrayList");
        if (index >= ptr->count)
        {
                PWL_LogWarn("The index can not out of range.");
                return NULL;
        }
        void* old_ptr = ptr->list_ptr[index];
        size_t tail = ptr->count - index - 1;
        if (tail > 0)
                memmove(&ptr->list_ptr[index], &ptr->list_ptr[index + 1],tail * sizeof(void*));
        --(ptr->count);
        return old_ptr;
}

/**
 * @brief PWL_PushInArrayList
 * Push element in the end of list.
 * @param ptr <PWL_ArrayList*> [Mut Borrow] ArrayList handle
 * @param element <void*> [const] The element ptr
 */
void PWL_PushInArrayList(PWL_ArrayList* ptr, void* element)
{
        PWL_CheckNull(ptr, "PWL_ArrayList");
        if(ptr->count == ptr->memory_count)
        {
                PWL_CacheInArrayList(ptr, (ptr->memory_count + 8) * sizeof(void*));
        }
        ptr->list_ptr[ptr->count++] = element;
}
/**
 * @brief PWL_PopInArrayList
 * Pop element in the end of list.
 * @param ptr <PWL_ArrayList*> [Mut Borrow] ArrayList handle 
 * @return void* 
 */
void* PWL_PopInArrayList(PWL_ArrayList* ptr)
{
        PWL_CheckNull(ptr, "PWL_ArrayList");
        if (ptr->count == 0)
        {
                PWL_LogWarn("The array list is empty. Can not pop continue.");
                return NULL;
        }
        return ptr->list_ptr[--ptr->count];
}
/**
 * @brief PWL_PeekInArrayList
 * Peek the top of the list
 * @param ptr <PWL_ArrayList*> [Mut Borrow] ArrayList handle 
 */
void* PWL_PeekInArrayList(PWL_ArrayList* ptr)
{
        PWL_CheckNull(ptr, "PWL_ArrayList");
        return ptr->list_ptr[ptr->count-1];
}
/**
 * @brief PWL_ForeachInArrayList
 * Use macro to support callback code. The stack frame have `(size_t)index` and `(void*)object` to use
 * Example:
 * PWL_ForeachInArrayList(ptr, {
 *      printf("index:%d, element:%s", index, (char*)object);
 * });
 */
#define PWL_ForeachInArrayList(ptr, code) do{ \
        for (size_t index = 0; index < ptr->count; ++index){ \
                void* object = PWL_GetInArrayList(ptr, index); \
                code \
        }\
}while(0)
/**
 * @brief PWL_FreeArrayList
 * If your list need free every pointer, use it to free ptr. If it's null, it will be passed.
 * It will not delete the pointer. And the count will be zero, but memory count is still.
 * Notice: Any element are must in the heap.
 * @param ptr <PWL_ArrayList*> [Mut Borrow] ArrayList handle 
 */
void PWL_FreeArrayList(PWL_ArrayList* ptr)
{
        PWL_ForeachInArrayList(ptr, {
                if (object == NULL) 
                        continue;
                free(object);
        });
}
/// >>>>>| Dictionary |================================================================================================ ///

static inline uint32_t PWL_FNV1aHashFuction(const uint8_t* data, size_t len)
{
        const uint32_t FNV_OFFSET = 2166136261UL;
        const uint32_t FNV_PRIME  = 16777619UL;

        uint32_t h = FNV_OFFSET;
        for (size_t i = 0; i < len; ++i)
        {
                h ^= data[i];
                h *= FNV_PRIME;
        }
        return h;
}

#define PWL_HashFuction PWL_FNV1aHashFuction
