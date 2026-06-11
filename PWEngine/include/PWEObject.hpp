#pragma once

#include <memory>

namespace PWEngine::Type
{   
    template<typename T>
    class PWERefCountPtr
    {
        protected:
            std::shared_ptr<T> ptr;
            void setPtr(T* ptr) {this->ptr.reset(ptr);}
        public:
            // new 
            PWERefCountPtr<T>() = default;
            // move old ptr to refcount ptr
            PWERefCountPtr(T* raw_ptr) : ptr(raw_ptr){}
            // move old ptr to refcount ptr, who have special delete method
            PWERefCountPtr(T* raw_ptr, void (*destroy_function)(T*)) : ptr(raw_ptr, destroy_function){}
            // ptr leave stack, count--
            virtual ~PWERefCountPtr() = default;
            // copy object, count++
            PWERefCountPtr(const PWERefCountPtr&) = default;
            PWERefCountPtr& operator=(const PWERefCountPtr&) = default;
            // move object, count
            PWERefCountPtr(PWERefCountPtr&&) = default;
            PWERefCountPtr& operator=(PWERefCountPtr&&) = default;
            static void* operator new(size_t, void*) = delete;
            // ptr info
            bool isNull() const {return !ptr;}
            // switch native
            T* get() {return ptr.get();}
            const T* get() const {return ptr.get();}

            T* operator->() { return ptr.get(); }
            const T* operator->() const { return ptr.get(); }
            T& operator*() { return *ptr; }
            const T& operator*() const { return *ptr; }
            // must use get()
            operator T*() const = delete;
            // get count
            long getRefCount() const {return ptr.use_count();}

    };
}