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

#pragma once

#include "PWMarco.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// >>>>>| Debug |==================================================================================================== ///

static bool PWL_IsNull(void* ptr, char* name)
{
        if (ptr == NULL)
        {
                printf("\033[33m" "WARN: The ptr(%s) is null" "\033[0m" "\n", name);
                return false;
        }
        else 
        {
                return true;
        }
}

static void* PWL_PrintErrorPtr(char* fmt)
{
        printf("\033[33m" "WARN:%s" "\033[0m" "\n", fmt);
        return NULL;
}

static void PWL_PrintErrorVoid(char* fmt)
{
        printf("\033[33m" "WARN:%s" "\033[0m" "\n", fmt);
        return;
}

/// >>>>>| Vector |==================================================================================================== ///

PWL_STRUCT(PWL_Color)
struct PWL_Color
{
    char r; char g; char b; char a;
};

PWL_STRUCT(PWL_Vec2i)
struct PWL_Vec2i
{
        int32_t x;
        int32_t y;
};

PWL_STRUCT(PWL_Vec2u)
struct PWL_Vec2u
{
        uint32_t x; 
        uint32_t y;
};

PWL_STRUCT(PWL_Vec2f)
struct PWL_Vec2f
{
        float x; 
        float y;
};
/// ===== | PWalloc | ===== ///
#define PWL_New(type, value, out) \
{ \
        type* ptr = malloc(sizeof(type)); \
        *ptr = (value); \
        out = (void*)ptr; \
}

void PWL_Free(void* ptr)
{
        if (ptr != NULL)
                free(ptr);
}

#define PWL_Min(a,b) ((a) < (b) ? (a) : (b))
#define PWL_Max(a,b) ((a) > (b) ? (a) : (b))

/// >>>>>| ArrayList |================================================================================================= ///

/**
 * @brief PWL_ArrayList
 * The PWL_ArrayList object. 
 * @param list_ptr<void**> The start pointer for void* list in heap.
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
 * @param empty_self the stack self
 * @return <PWL_ArrayList*> handle
 */
PWL_ArrayList* PWL_InitArrayList(PWL_ArrayList* self)
{
        self->list_ptr= malloc(8 * sizeof(void*));
        self->count = 0;
        self->memory_count = 8;
        return self;
}
/**
 * @brief PWL_CreateArrayList
 * Create and init ArrayList in heap. It's store in the heap, you should destroy by PWL_DestroyArrayList.
 * @return <PWL_ArrayList*> [need destroy] handle
 */
PWL_ArrayList* PWL_CreateArrayList(void)
{
        PWL_ArrayList* self= (PWL_ArrayList*)malloc(sizeof(PWL_ArrayList));
        if (self == NULL)
        {
                printf("\033[33m" "WARN: Input ArrayList is NULL." "\033[0m" "\n"); 
                return NULL;
        } 
        self->list_ptr= malloc(8 * sizeof(void*));
        self->count = 0;
        self->memory_count = 8;
        return self;
}
/**
 * @brief PWL_DestroyArrayList
 * Destroy the ArrayList. You must do it to avoid the memory leak.
 * @param self<PWL_ArrayList*> [Move owner] ArrayList handle 
 */
void PWL_DestroyArrayList(PWL_ArrayList* self)
{
        if (self == NULL)
        {
                printf("\033[33m" "WARN: Input ArrayList is NULL." "\033[0m" "\n"); 
                return;
        } 
        free(self);
}
/**
 * @brief PWL_CacheInArrayList
 * Add the memory count. When you want to store many data, you can apply the more memory and less apply.
 * @param self<PWL_ArrayList*> [Mut Borrow] ArrayList handle 
 * @param size <size_t> [const] The size of new memory. size * sizeof(void*)
 */
void PWL_CacheInArrayList(PWL_ArrayList* self, const size_t size)
{
        if (self == NULL)
        {
                printf("\033[33m" "WARN: Input ArrayList is NULL." "\033[0m" "\n"); 
                return;
        } 
        size_t new_memory_count = self->memory_count + size;
        void** new_list_ptr= realloc(self->list_ptr, new_memory_count * sizeof(void*));
        if (new_list_ptr == NULL)
        {
                printf("\033[33m" "WARN: Can not create new pointer." "\033[0m" "\n"); 
                return;
        } 
        self->list_ptr= new_list_ptr;
        self->memory_count = new_memory_count;
}
/**
 * @brief PWL_FindInArrayList
 * The find is mix Get and Set.
 * If value equal NULL, mean get function. Return the value in dictionary.
 * Else, Modify the value and return the old value.
 * @param self<PWL_ArrayList*> [Mut Borrow] ArrayList handle
 * @param index <size_t> [const] The index in list
 * @param element <void*> [const] The element self
 * @return <void*> old element 
 */
void* PWL_FindInArrayList(PWL_ArrayList* self, const size_t index, void* element)
{
        if (self == NULL) 
        {
                printf("\033[33m" "WARN: Input ArrayList is NULL." "\033[0m" "\n"); 
                return NULL;
        }
                
        void* pop_obj = NULL;
        if (index <= self->count)
        {
                if (index >= self->count)
                {
                        printf("\033[33m" "WARN:Out of the index" "\033[0m" "\n");    
                        return NULL;
                }
                pop_obj =  self->list_ptr[index];
                if (element == NULL)
                {
                        /* get mode*/
                        return pop_obj;
                }
                else
                {
                        /* set mode*/
                        self->list_ptr[index] = element;
                }
        }
        else
        {
                if (index >= self->memory_count)
                        PWL_CacheInArrayList(self, (index + 8) * sizeof(void*));

                self->list_ptr[index] = element;
                self->count = index + 1;
        }
        return pop_obj;
}
/**
 * @brief PWL_InsertInArrayList
 * Insert element in list, and other element in behind will be moved.
 * @param self<PWL_ArrayList*> [Mut Borrow] ArrayList handle
 * @param index <size_t> [const] The index in list
 * @param element <void*> [const] The element self
 */
void PWL_InsertInArrayList(PWL_ArrayList* self, size_t index, void* element)
{
        if (self == NULL)
                return;
        if (index >= self->count)
        {
                PWL_FindInArrayList(self, index, element);
                return;
        }
        else 
        {
                if(self->count == self->memory_count)
                {
                        PWL_CacheInArrayList(self, (self->memory_count + 8) * sizeof(void*));
                }
                size_t head = self->count - index;
                if (head > 0)
                        memmove(&self->list_ptr[index + 1], &self->list_ptr[index], head * sizeof(void*));
                self->list_ptr[index] = element;
                self->count += 1;
        }
}
/**
 * @brief PWL_RemoveInArrayList
 * Remove the element in the list, and other element in behind will be moved.
* It will return the old value and help you delete it.  
 * @param self<PWL_ArrayList*> [Mut Borrow] ArrayList handle
 * @param index <size_t> [const] The index in list
 */
void* PWL_RemoveInArrayList(PWL_ArrayList* self, size_t index)
{
        if (self == NULL)
        {
                printf("\033[33m" "WARN: Input ArrayList is NULL." "\033[0m" "\n"); 
                return NULL;
        }     
        if (index >= self->count)
        {
                printf("\033[33m" "WARN: The index can not out of range." "\033[0m" "\n");    
                return NULL;
        }
        void* old_self= self->list_ptr[index];
        size_t tail = self->count - index - 1;
        if (tail > 0)
                memmove(&self->list_ptr[index], &self->list_ptr[index + 1],tail * sizeof(void*));
        self->count -= 1;
        return old_self;
}
/**
 * @brief PWL_PushInArrayList
 * Push element in the end of list.
 * @param self<PWL_ArrayList*> [Mut Borrow] ArrayList handle
 * @param element <void*> [const] The element self
 */
void PWL_PushInArrayList(PWL_ArrayList* self, void* element)
{
        if (self == NULL)
        {
                printf("\033[33m" "WARN: Input ArrayList is NULL." "\033[0m" "\n"); 
                return;
        } 
        if(self->count == self->memory_count)
        {
                PWL_CacheInArrayList(self, (self->memory_count + 8) * sizeof(void*));
        }
        self->list_ptr[self->count++] = element;
}
/**
 * @brief PWL_PopInArrayList
 * Pop element in the end of list.
 * @param self<PWL_ArrayList*> [Mut Borrow] ArrayList handle 
 * @return void* 
 */
void* PWL_PopInArrayList(PWL_ArrayList* self)
{
        if (self == NULL)
        {
                printf("\033[33m" "WARN: Input ArrayList is NULL." "\033[0m" "\n"); 
                return NULL;
        } 
        if (self->count == 0)
        {
                printf("\033[33m" "WARN:The array list is empty. Can not pop continue." "\033[0m" "\n");    
                return NULL;
        }
        return self->list_ptr[--self->count];
}
/**
 * @brief PWL_PeekInArrayList
 * Peek the top of the list
 * @param self <PWL_ArrayList*> [Mut Borrow] ArrayList handle 
 */
void* PWL_PeekInArrayList(PWL_ArrayList* self)
{
        if (self == NULL)
        {
                printf("\033[33m" "WARN: Input ArrayList is NULL." "\033[0m" "\n"); 
                return NULL;
        }
        return self->list_ptr[self->count-1];
}
/**
 * @brief PWL_FreeInArrayList
 * If your list need free every pointer, use it to free self. If it's null, it will be passed.
 * It will not delete the pointer. And the count will be zero, but memory count is still.
 * Notice: Any element are must in the heap.
 * @param self<PWL_ArrayList*> [Mut Borrow] ArrayList handle 
 */
void PWL_FreeArrayList(PWL_ArrayList* self)
{
        for (size_t index = 0; index < self->count; ++index)
        {
                void* object = PWL_FindInArrayList(self, index, NULL);
                if (object == NULL) 
                        continue;
                free(object);
        }
}
/**
 * @brief 
 * Use function pointer to support callback code.
 * @param self <PWL_ArrayList*> [Mut Borrow] ArrayList handle 
 * @param callback <function *(size_t index, void* object)->void> call back function
 */
void PWL_ForeachInArrayList(PWL_ArrayList* self, void (*callback)(size_t index, void* object))
{
        if (self == NULL)
        {
                printf("\033[33m" "WARN: Input ArrayList is NULL." "\033[0m" "\n"); 
                return;
        }
        for (size_t index = 0; index < self->count; ++index)
        {
                void* object = PWL_FindInArrayList(self, index, NULL);
                callback(index, object);
        }
}
/**
 * @brief PWL_ForeachMacroInArrayList
 * Use macro to support callback code. The stack frame have `(size_t)index` and `(void*)object` to use
 * Example:
 * PWL_ForeachInArrayList(self, {
 *      printf("index:%d, element:%s", index, (char*)object);
 * });
 */
#define PWL_ForeachMacroInArrayList(self, code) do{ \
        for (size_t index = 0; index < self->count; ++index){ \
                void* object = PWL_FindInArrayList(self, index, NULL); \
                code \
        }\
}while(0)
/// >>>>>| Dictionary |================================================================================================ ///

#define STD_FNV64_PRIME     1099511628211ULL
#define STD_FNV64_OFFSET    14695981039346656037ULL

static inline uint64_t PWL_FNV1aHashFuction(const uint8_t* data, size_t len)
{
        uint64_t h = STD_FNV64_OFFSET;
        for (size_t i = 0; i < len; ++i)
        {
                h ^= data[i];
                h *= STD_FNV64_PRIME;
        }
        return h;
}

static inline uint64_t PWL_FNV1aHashFuctionChar(const char* str)
{
        if (!str)
                return 0;
        return PWL_FNV1aHashFuction((const uint8_t*)str, strlen(str));
}

/**
 * @brief PWL_DictionaryEntry
 * The basic element in buckets.
 * @param hash <uint64_t> The cache of hash
 * @param key <char*> The key 
 * @param next <PWL_DictionaryEntry*> The next pointer in LinkList.
 */
PWL_STRUCT(PWL_DictionaryEntry)
struct PWL_DictionaryEntry
{
        uint64_t hash;
        char* key;
        void* value;
        PWL_DictionaryEntry* next;
};
/**
 * @brief PWL_Dictionary
 * The Dictionary is a special hashmap which key is string, not other value.
 * @param buckets <PWL_DictionaryEntry**> The bucket store the pointer, which point the header of LinkList.
 * @param bucket_cap <size_t> The number of bucket in memory, bucket_cap = 2 ^ n.
 * @param entry_count <size_t> The number of entry in dictionary.
 * @param load_threshold <float 0-1> The limit to add bucket.
 */
PWL_STRUCT(PWL_Dictionary)
struct PWL_Dictionary
{
        PWL_DictionaryEntry** buckets;
        size_t bucket_count;
        size_t entry_count;
        float load_threshold;
        PWL_ArrayList* string_list;
};
/**
 * @brief PWL_CreateDictionary
 * Create and init Dictionary in heap. It's store in the heap, you should destroy by PWL_DestroyDictionary.
 * @return <PWL_Dictionary*> [need destroy] handle
 */
PWL_Dictionary* PWL_CreateDictionary()
{
        PWL_Dictionary* self = (PWL_Dictionary*)malloc(sizeof(PWL_Dictionary));
        if (self == NULL)
        {
                printf("\033[33m" "WARN: Create Dictionary is failed" "\033[0m" "\n"); 
                return NULL;
        }

        self->bucket_count = 64;
        self->entry_count = 0;
        self->load_threshold = 0.75f;

        self->string_list = PWL_CreateArrayList();

        self->buckets = (PWL_DictionaryEntry**)calloc(self->bucket_count, sizeof(PWL_DictionaryEntry*));
        if (self->buckets == NULL)
        {
                printf("\033[33m" "WARN: Create Dictionary Entry is failed" "\033[0m" "\n"); 
        }
        return self;
}
/**
 * @brief PWL_DestroyDictionary
 * Destroy the Dictionary. You must do it to avoid the memory leak.
 * @param self<PWL_Dictionary*> [Move owner] ArrayList handle 
 */
void PWL_DestroyDictionary(PWL_Dictionary* self)
{
        if (self == NULL)
        {
                printf("\033[33m" "WARN: Create Dictionary is failed" "\033[0m" "\n"); 
                return;
        }
        PWL_FreeArrayList(self->string_list);
        PWL_DestroyArrayList(self->string_list);
        free(self->buckets);
        free(self);
}
/**
 * @brief PWL_ResizeInDictionary
 * Add the memory count as double. When you want to store many data, you can apply the more memory and less apply.
 * @param self<PWL_Dictionary*> [Mut Borrow] Dictionary handle 
 */
static void PWL_ResizeInDictionary(PWL_Dictionary* self)
{
        size_t new_size = 2 * self->bucket_count;

        PWL_DictionaryEntry** new_bucket = (PWL_DictionaryEntry**)calloc(new_size, sizeof(PWL_DictionaryEntry*));
        if (new_bucket == NULL)
        {
                printf("\033[33m" "WARN: Create Dictionary Bucket is failed" "\033[0m" "\n"); 
                return;
        }

        for (size_t i = 0; i < self->bucket_count; ++i)
        {
                PWL_DictionaryEntry* currect_entry = self->buckets[i];
                while (currect_entry == NULL) 
                {
                        PWL_DictionaryEntry* next_node = currect_entry->next;
                        size_t new_index = currect_entry->hash & (new_size - 1);
                        currect_entry->next = new_bucket[new_index];
                        new_bucket[new_index] = currect_entry;
                        currect_entry = next_node;
                }
        }

        free(self->buckets);
        self->buckets = new_bucket;
        self->bucket_count = new_size;
}
/**
 * @brief PWL_FindDictionary
 * The find is mix Get and Set.
 * If value equal NULL, mean get function. Return the value in dictionary.
 * Else, Modify the value and return the old value.
 */
void* PWL_FindInDictionary(PWL_Dictionary* self, const char* key, void* value)
{
        if (self == NULL)
        {
                printf("\033[33m" "WARN: Input Dictionary is NULL." "\033[0m" "\n"); 
                return NULL;
        }

        void* old_value;
        /* resize */
        float resize_level = (float)self->entry_count / (float)self->bucket_count; 
        if (resize_level >= self->load_threshold)
        {
                PWL_ResizeInDictionary(self);
        }

        /* find bucket */
        uint64_t hash_code = PWL_FNV1aHashFuctionChar(key);
        size_t index_of_bucket = hash_code & (self->bucket_count - 1);
        PWL_DictionaryEntry* current_bucket = self->buckets[index_of_bucket];
        while (current_bucket != NULL) 
        {
                /* if find it*/
                if (current_bucket->hash == hash_code && strcmp(current_bucket->key, key) == 0)
                {
                        if (value == NULL)
                        {
                                /* get mode*/
                                return current_bucket->value;
                        }
                        else 
                        {
                                /* set mode*/
                                old_value = current_bucket->value;
                                current_bucket->value = value;
                                char* key_name = (char*)calloc(strlen(key), sizeof(char*));
                                if (key_name == NULL) 
                                {
                                        printf("\033[33m" "Error: Can not malloc memory for store window name." "\033[0m" "\n");
                                        return NULL;
                                }
                                strcpy(key_name, key);
                                return old_value;
                        }
                        
                        
                }
                current_bucket = current_bucket->next;
        }
        /* if no found, create new entry and store it. */
        if (value == NULL)
        {
                printf("\033[33m" "WARN: Not found in Dictionary." "\033[0m" "\n"); 
                return NULL;
        }
        PWL_DictionaryEntry* new_entry = (PWL_DictionaryEntry*)malloc(sizeof(PWL_DictionaryEntry*));
        if (new_entry == NULL)
        {
                printf("\033[33m" "WARN: Create PWL_DictionaryEntry is failed." "\033[0m" "\n"); 
                return NULL;
        }

        size_t key_text_length = strlen(key);
        new_entry->key = (char*)malloc(key_text_length + 1);
        if (new_entry->key == NULL)
        {
                printf("\033[33m" "WARN: Create string in PWL_DictionaryEntry is failed." "\033[0m" "\n"); 
                free(new_entry);
                return NULL;
        }
        memcpy(new_entry->key, key, key_text_length + 1);

        new_entry->hash = hash_code;
        new_entry->value = value;
        /* link new entry */
        new_entry->next = self->buckets[index_of_bucket];
        self->buckets[index_of_bucket] = new_entry;

        self->entry_count += 1;
        
        return old_value;
}

void* PWL_RemoveInDictionary(PWL_Dictionary* self, const char* key)
{
        if (self == NULL)
        {
                printf("\033[33m" "WARN: Input Dictionary is NULL." "\033[0m" "\n"); 
                return NULL;
        }
        if (key == NULL)
        {
                printf("\033[33m" "WARN: Input key is NULL." "\033[0m" "\n");
                return NULL;
        }

        uint64_t hash_code = PWL_FNV1aHashFuctionChar(key);
        size_t index_of_bucket = hash_code & (self->bucket_count - 1);

        PWL_DictionaryEntry** pre_currect = &self->buckets[index_of_bucket];
        while (*pre_currect != NULL) 
        {
                PWL_DictionaryEntry* currect = *pre_currect;
                if (currect->hash == hash_code && strcmp(currect->key, key) == 0)
                {
                        void* old_value = currect->value;
                        *pre_currect = currect->next;
                        free(currect->key);
                        free(currect);
                        self->entry_count -= 1;
                        return old_value;
                }
                pre_currect = &currect->next;
        }
        printf("\033[33m" "WARN: The key not found in Dictionary." "\033[0m" "\n");
        return NULL;
}
/**
 * @brief PWL_FreeDictionary
 * If your dictionary need free every pointer, use it to free self. If it's null, it will be passed.
 * It will not delete the pointer. And the count will be zero, but memory count is still.
 * Notice: Any element are must in the heap.
 * @param self<PWL_ArrayList*> [Mut Borrow] ArrayList handle 
 */
void PWL_FreeDictionary(PWL_Dictionary* self)
{
        if (self == NULL)
        {
                printf("\033[33m" "WARN: Input Dictionary is NULL." "\033[0m" "\n"); 
                return;
        }

        for (size_t i = 0; i < self->bucket_count; ++i)
        {
                PWL_DictionaryEntry* currect_entry = self->buckets[i];
                while (currect_entry != NULL) 
                {
                        PWL_DictionaryEntry* next_entry = currect_entry->next;
                        free(currect_entry->key);
                        free(currect_entry);
                        currect_entry = next_entry;
                }
                self->buckets[i] = NULL;
        }
        self->entry_count = 0;
}

/**
 * @brief 
 * Use function pointer to support callback code.
 * @param self <PWL_Dictionary*> [Mut Borrow] ArrayList handle 
 * @param callback <function *(size_t index, void* object)->void> call back function
 */
void PWL_ForeachInDictionary(PWL_Dictionary* self, void (*callback)(char* key, void* value))
{
        if (self == NULL)
        {
                printf("\033[33m" "WARN: Input ArrayList is NULL." "\033[0m" "\n"); 
                return;
        }
        for (size_t i = 0; i < self->bucket_count; ++i)
        {
                PWL_DictionaryEntry* currect_entry = self->buckets[i];
                while (currect_entry != NULL) 
                {
                        PWL_DictionaryEntry* next_entry = currect_entry->next;
                        callback(currect_entry->key, currect_entry->value);
                }
        }
}