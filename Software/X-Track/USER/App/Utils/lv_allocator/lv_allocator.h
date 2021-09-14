/**
  * Reference link: http://www.josuttis.com/cppcode/myalloc.hpp.html
  * The following code example is taken from the book
  * The C++ Standard Library - A Tutorial and Reference
  * by Nicolai M. Josuttis, Addison-Wesley, 1999
  * © Copyright Nicolai M. Josuttis 1999
  */
#ifndef __LV_ALLOCATOR_H
#define __LV_ALLOCATOR_H

#include "lvgl/lvgl.h"
#include <limits.h>
#include <cstddef>

template <class T>
class lv_allocator
{
public:
    // type definitions
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef std::size_t    size_type;
    typedef std::ptrdiff_t difference_type;

    // rebind allocator to type U
    template <class U>
    struct rebind
    {
        typedef lv_allocator<U> other;
    };

    // return address of values
    pointer address(reference value) const
    {
        return &value;
    }
    const_pointer address(const_reference value) const
    {
        return &value;
    }

    /* constructors and destructor
        * - nothing to do because the allocator has no state
        */
    lv_allocator() throw()
    {
    }
    lv_allocator(const lv_allocator&) throw()
    {
    }
    template <class U>
    lv_allocator(const lv_allocator<U>&) throw()
    {
    }
    ~lv_allocator() throw()
    {
    }

    // return maximum number of elements that can be allocated
    size_type max_size() const throw()
    {
        // return std::numeric_limits<std::size_t>::max() / sizeof(T);
        return UINT_MAX / sizeof(T);
    }

    // allocate but don't initialize num elements of type T
    pointer allocate(size_type num, const void* = 0)
    {
        if(!lv_is_initialized())
        {
            lv_init();
        }
        
        // print message and allocate memory with global new
        //
        //std::cerr << "allocate " << num << " element(s)" << " of size " << sizeof(T) << std::endl;
        //pointer ret = (pointer)(::operator new(num * sizeof(T)));
        pointer ret = (pointer)lv_mem_alloc(num * sizeof(T));
        // std::cerr << " allocated at: " << (void*)ret << std::endl;
        return ret;
    }

    // initialize elements of allocated storage p with value value
    void construct(pointer p, const T& value)
    {
        // initialize memory with placement new
        new((void*)p)T(value);
    }

    // destroy elements of initialized storage p
    void destroy(pointer p)
    {
        // destroy objects by calling their destructor
        p->~T();
    }

    // deallocate storage p of deleted elements
    void deallocate(pointer p, size_type num)
    {
        // print message and deallocate memory with global delete
        //
        //std::cerr << "deallocate " << num << " element(s)" << " of size " << sizeof(T) << " at: " << (void*)p << std::endl;
        //::operator delete((void*)p);
        lv_mem_free((void*)p);
    }
};

// return that all specializations of this allocator are interchangeable
template <class T1, class T2>
bool operator== (const lv_allocator<T1>&,
    const lv_allocator<T2>&) throw() {
    return true;
}
template <class T1, class T2>
bool operator!= (const lv_allocator<T1>&,
    const lv_allocator<T2>&) throw() {
    return false;
}

#endif
