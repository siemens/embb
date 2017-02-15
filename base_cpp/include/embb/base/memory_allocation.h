/*
 * Copyright (c) 2014-2017, Siemens AG. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef EMBB_BASE_MEMORY_ALLOCATION_H_
#define EMBB_BASE_MEMORY_ALLOCATION_H_

#include <embb/base/exceptions.h>
#include <embb/base/internal/config.h>
#include <embb/base/c/internal/unused.h>
#include <limits>
#include <cstddef>

#include <new>
#include <embb/base/c/memory_allocation.h>

/**
 * \defgroup CPP_BASE_MEMORY_ALLOCATION Memory Allocation
 * Functions, classes, and allocators for dynamic memory allocation.
 *
 * \ingroup CPP_BASE
 */

namespace embb {
namespace base {
/**
 * Common (static) functionality for unaligned and aligned memory allocation.
 * This class is a wrapper for the functions in
 * \ref embb/base/c/memory_allocation.h
 *
 * \ingroup CPP_BASE_MEMORY_ALLOCATION
 */
class Allocation {
 public:
  /**
   * Allocates memory for an instance of type \c Type and default-initializes
   * it.
   *
   * Keeps track of allocated memory in debug mode.
   *
   * \return Pointer to new instance of type \c Type
   *
   * \throws embb::base::NoMemoryException if not enough memory is available
   *         for the given type.
   *
   * \see Delete()
   *
   * \memory <tt>size+3*sizeof(size_t)</tt> bytes in debug mode, otherwise \c
   *         size bytes
   *
   * \threadsafe
   *
   * \tparam Type %Type of the object to be allocated
   */
  template<typename Type>
  static Type* New() {
    void* memory = embb_alloc(sizeof(Type));
    if (memory == NULL) EMBB_THROW(NoMemoryException, "When allocating memory");
    return new(memory) Type();
  }

  #ifdef DOXYGEN

  /**
   * Allocates memory unaligned for an instance of type \c Type and initializes
   * it with the specified arguments.
   *
   * Keeps track of allocated memory in debug mode.
   *
   * \return Pointer to new instance of type \c Type
   *
   * \throws embb::base::NoMemoryException if not enough memory is available
   *         for the given type.
   *
   * \see Delete()
   *
   * \memory <tt>size+3*sizeof(size_t)</tt> bytes in debug mode, otherwise \c
   *         size bytes
   *
   * \threadsafe
   *
   * \tparam Type %Type of the instance to be allocated
   * \tparam Arg1 %Type of (first) constructor argument
   */
  template<typename Type, typename Arg1, ...>
  static Type* New(
    Arg1 argument1,
    /**< [IN] (First) argument for constructor of \c Type */
    ...
    );

#else // DOXYGEN

  /**
   * See Doxygen documentation dummy above
   */
  template<typename Type, typename Arg1>
  static Type* New(Arg1 arg1) {
    void* memory = embb_alloc(sizeof(Type));
    if (memory == NULL) EMBB_THROW(NoMemoryException, "When allocating memory");
    return new(memory) Type(arg1);
  }

  /**
   * See Doxygen documentation dummy above
   */
  template<typename Type, typename Arg1, typename Arg2>
  static Type* New(Arg1 arg1, Arg2 arg2) {
    void* memory = embb_alloc(sizeof(Type));
    if (memory == NULL) EMBB_THROW(NoMemoryException, "When allocating memory");
    return new(memory) Type(arg1, arg2);
  }

  /**
   * See Doxygen documentation dummy above
   */
  template<typename Type, typename Arg1, typename Arg2, typename Arg3>
  static Type* New(Arg1 arg1, Arg2 arg2, Arg3 arg3) {
    void* memory = embb_alloc(sizeof(Type));
    if (memory == NULL) EMBB_THROW(NoMemoryException, "When allocating memory");
    return new(memory) Type(arg1, arg2, arg3);
  }

  /**
   * See Doxygen documentation dummy above
   */
  template<typename Type, typename Arg1, typename Arg2, typename Arg3,
           typename Arg4>
  static Type* New(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4) {
    void* memory = embb_alloc(sizeof(Type));
    if (memory == NULL) EMBB_THROW(NoMemoryException, "When allocating memory");
    return new(memory) Type(arg1, arg2, arg3, arg4);
  }

  /**
   * See Doxygen documentation dummy above
   */
  template<typename Type, typename Arg1, typename Arg2, typename Arg3,
           typename Arg4, typename Arg5>
  static Type* New(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5) {
    void* memory = embb_alloc(sizeof(Type));
    if (memory == NULL) EMBB_THROW(NoMemoryException, "When allocating memory");
    return new(memory) Type(arg1, arg2, arg3, arg4, arg5);
  }

#endif // else DOXYGEN

  /**
   * Destructs an instance of type \c Type and frees the allocated memory.
   *
   * \tparam Type %Type of instance to be deleted
   */
  template<typename Type>
  static void Delete(
    Type* to_delete
    /**< [IN,OUT] Instance to be deleted */
    ) {
    to_delete->~Type();
    embb_free(static_cast<void*>(to_delete));
  }

  /**
   * Returns the total number of bytes currently allocated.
   *
   * Wrapper for C function embb_get_bytes_allocated().
   *
   * \return Number of currently allocated bytes in debug mode, otherwise 0.
   *
   * \waitfree
   */
  static size_t AllocatedBytes();

  /**
   * Allocates \p size bytes of memory (unaligned).
   *
   * Wrapper for C function embb_allocate().
   *
   * Keeps track of allocated memory in debug mode.
   *
   * \return NULL in case of failure, otherwise address of allocated memory
   * block.
   *
   * \throws embb::base::NoMemoryException if not enough memory is available.
   *
   * \memory <tt>size+3*sizeof(size_t)</tt> bytes in debug mode, otherwise \c
   *         size bytes
   *
   * \threadsafe
   *
   * \note Memory allocated using this function must be freed using
   *       Allocation::Free().
   *
   * \see AllocateAligned(), AllocateCacheAligned(), Free()
   */
  static void* Allocate(
    size_t size
    /**< [IN] Size of memory block to be allocated in bytes */
    );

  /**
   * Frees memory that has been allocated by Allocation::Allocate() for
   * some pointer \p ptr.
   *
   * Wrapper for C function embb_free().
   *
   * Keeps track of freed memory in debug mode.
   *
   * \threadsafe
   *
   * \see Allocate()
   */
  static void Free(
    void * ptr
    /**< [IN,OUT] Pointer to memory block to be freed */
    );

  /**
   * Allocates \p size bytes of memory with alignment \p alignment.
   *
   * Wrapper for C function embb_alloc_aligned().
   *
   * This function can be used to align objects to certain boundaries such as
   * cache lines, memory pages, etc.
   *
   * Keeps track of allocated memory in debug mode.
   *
   * It is not required that \p size is a multiple of \p alignment as, e.g.,
   * for the \c aligned\_alloc function of the C11 Standard.
   *
   * \pre The alignment has to be power of 2 and a multiple of
   *      <tt>size(void*)</tt>.
   * \post The returned pointer is a multiple of \p alignment.
   *
   * \return NULL in case of failure, otherwise address of allocated memory
   * block.
   *
   * \throws embb::base::NoMemoryException if not enough memory is available.
   *
   * \memory Debug mode: Let \c n be the number of aligned cells necessary to
   *         fit the payload. Then, <tt>(n+1)*alignment+3*size_of(size_t)-1</tt>
   *         bytes are allocated.<br> Release mode: \c size bytes are requested
   *         using the functions provided by the operating systems.
   *
   * \threadsafe
   *
   * \note Memory allocated using this function must be freed using
   *       Allocation::FreeAligned().
   *
   * \see Allocate(), AllocateCacheAligned(), FreeAligned()
   */
  static void* AllocateAligned(
    size_t alignment,
    /**< [IN] Alignment in bytes */
    size_t size
    /**< [IN] Size of memory block to be allocated in bytes */
    );

  /**
   * Frees memory that has been allocated by Allocation::AllocateAligned() or
   * Allocation::AllocateCacheAligned() for some pointer \p ptr.
   *
   * Wrapper for C function embb_free_aligned().
   *
   * Keeps track of freed memory in debug mode.
   *
   * \threadsafe
   *
   * \see AllocateAligned(), AllocateCacheAligned()
   */
  static void FreeAligned(
    void * ptr
    /**< [IN,OUT] Pointer to memory block to be freed */
    );

  /**
   * Allocates \p size bytes of cache-aligned memory.
   *
   * Wrapper for C function embb_alloc_cache_aligned().
   *
   * Specialized version of Allocation::AllocateAligned(). The alignment is
   * chosen automatically (usually 64 bytes).
   *
   * Keeps track of allocated memory in debug mode.
   *
   * \post The returned pointer is a multiple of the cache line size.
   *
   * \return NULL in case of failure, otherwise address of allocated memory
   *         block.
   *
   * \throws embb::base::NoMemoryException if not enough memory is available.
   *
   * \memory See Allocation::AllocateAligned()
   *
   * \threadsafe
   *
   * \note Memory allocated using this function must be freed using
   *       Allocation::FreeAligned().
   *
   * \see Allocate(), AllocateAligned(), FreeAligned()
   */
  static void* AllocateCacheAligned(
    size_t size
    /**< [IN] Size of memory block to be allocated in bytes */
    );
};

/**
 * Overloaded new/delete operators.
 *
 * Classes that derive from this class will use the EMBB methods for dynamic
 * allocation and deallocation of memory (Allocation::Allocate() and
 * Allocation::Free()). In debug mode, memory consumption is tracked in order to
 * detect memory leaks.
 *
 * \see CacheAlignedAllocatable
 *
 * \ingroup CPP_BASE_MEMORY_ALLOCATION
 */
class Allocatable {
 public:
  /**
   * New operator.
   *
   * Allocates \c size bytes of memory. Must not be called directly!
   *
   * \return Pointer to allocated block of memory
   *
   * \throws embb::base::NoMemoryException if not enough memory is available.
   *
   * \threadsafe
   *
   * \memory See Allocation::Allocate()
   *
   * \see operator delete()
   */
  static void* operator new(
    size_t size
    /**< [IN] Size of the memory block in bytes */
    );

  /**
   * Delete operator.
   *
   * Deletes \c size bytes of memory pointed to by \c ptr. Must not be called
   * directly!
   *
   * \threadsafe
   *
   * \see operator new()
   */
  static void operator delete(
    void* ptr,
    /**< [IN,OUT] Pointer to memory block to be freed */
    size_t size
    /**< [IN] Size of the memory block in bytes */
    );

  /**
   * Array new operator.
   *
   * Allocates an array of \c size bytes. Must not be called directly!
   *
   * \remark  Note that the global new[], calling this function, might
   *          return a different address. This is stated in the standard
   *          (5.3.4 New [expr.new]):
   * \remark  "A new-expression passes the amount of space requested to the
   *          allocation function as the first argument of type std::size_t.
   *          That argument shall be no less than the size of the object being
   *          created; it may be greater than the size of the object being
   *          created only if the object is an array."
   * \remark  So, even if the returned pointer of this function is aligned,
   *          the pointer to the array returned by global new[] need not be.
   *          For example, when using GCC 4.8.3 (64 bit), the size of the array
   *          is kept in the first 8 bytes of the allocated memory.
   *
   * \return Pointer to allocated block of memory
   *
   * \threadsafe
   *
   * \memory See Allocation::Allocate()
   *
   * \throws embb::base::NoMemoryException if not enough memory is available.
   *
   * \see operator delete[]()
   */
  static void* operator new[](
    size_t size
    /**< [IN] Size of the array in bytes*/
    );

  /**
   * Array delete operator.
   *
   * Deletes array of \c size bytes pointed to by \c ptr. Must not be called
   * directly!
   *
   * \threadsafe
   *
   * \see operator new[]()
   */
  static void operator delete[](
    void* ptr,
    /**< [IN,OUT] Pointer to the array to be freed */
    size_t size
    /**< [IN] Size of the array in bytes */
    );
};

/**
 * Overloaded new/delete operators.
 *
 * Classes that derive from this class will use the EMBB methods for dynamic,
 * cache-aligned allocation and deallocation of memory
 * (Allocation::AllocateCacheAligned() and Allocation::FreeAligned()).
 * In debug mode, memory consumption is tracked in order to detect memory leaks.
 *
 * \note When using the new[] operator, not each object in the array is aligned,
 * but only the constructed array as a whole.
 *
 * \see Allocatable
 *
 * \ingroup CPP_BASE_MEMORY_ALLOCATION
 */
class CacheAlignedAllocatable {
 public:
  /**
   * New operator.
   *
   * Allocates \c size bytes of memory. Must not be called directly!
   *
   * \return Pointer to allocated block of memory
   *
   * \throws embb::base::NoMemoryException if not enough memory is available.
   *
   * \threadsafe
   *
   * \memory See Allocation::AllocateCacheAligned()
   *
   * \see operator delete()
   */
  static void* operator new(
    size_t size
    /**< [IN] Size of the memory block in bytes */
    );

  /**
   * Delete operator.
   *
   * Deletes \c size bytes of memory pointed to by \c ptr. Must not be called
   * directly!
   *
   * \threadsafe
   */
  static void operator delete(
    void* ptr,
    /**< [IN,OUT] Pointer to memory block to be freed */
    size_t size
    /**< [IN] Size of the memory block in bytes */
    );

  /**
   * Array new operator.
   *
   * Allocates an array of \c size bytes. Must not be called directly!
   *
   * \return Pointer to allocated block of memory
   *
   * \throws embb::base::NoMemoryException if not enough memory is available.
   *
   * \memory See Allocation::AllocateCacheAligned()
   *
   * \threadsafe
   *
   * \see operator delete[]()
   */
  static void* operator new[](
    size_t size
    /**< [IN] size of bytes to allocate for the array*/
    );

  /**
   * Array delete operator.
   *
   * Deletes array of \c size bytes pointed to by \c ptr. Must not be called
   * directly!
   *
   * \threadsafe
   *
   * \see operator new[]()
   */
  static void operator delete[](
    void* ptr,
    /**< [IN,OUT] Pointer to the array to be freed */
    size_t size
    /**< [IN] Size of the array in bytes */
    );
};

/*
 * Forward declaration
 */
template <typename Type>
class Allocator;

/*
 * Specialization for void
 */
template <>
class Allocator < void > {
 public:
  typedef void*       pointer;
  typedef const void* const_pointer;
  typedef void        value_type;

  template <typename OtherType> struct rebind {
    typedef Allocator<OtherType> other;
  };
};

/**
 * %Allocator according to the C++ standard.
 *
 * For memory allocation and deallocation,
 * embb::base::Allocation::Allocate() and
 * embb::base::Allocation::Free() are used, respectively.
 *
 * In debug mode, leak checking is active. The function
 * embb::base::Allocation::AllocatedBytes() returns the number of
 * currently allocated bytes.
 *
 * \ingroup CPP_BASE_MEMORY_ALLOCATION
 */
template <typename Type>
class Allocator {
 public:
  /** Quantity of elements type */
  typedef size_t      size_type;

  /** Difference between two pointers type */
  typedef ptrdiff_t   difference_type;

  /** Pointer to element type */
  typedef Type*       pointer;

  /** Pointer to constant element type */
  typedef const Type* const_pointer;

  /** Reference to element type */
  typedef Type&       reference;

  /** Reference to constant element type */
  typedef const Type& const_reference;

  /** Element type */
  typedef Type        value_type;

  /**
   * Rebind allocator to type OtherType
   */
  template <typename OtherType> struct rebind {
    /** Type to rebind to */
    typedef Allocator<OtherType> other;
  };

  /**
   * Constructs allocator object
   */
  Allocator() throw() {}

  /**
   * Copies allocator object
   */
  Allocator(
    const Allocator&
    /**< [IN] Other allocator object */
    ) throw() {}

  /**
   * Constructs allocator object
   *
   * Allows construction from allocators for different types (rebind)
   */
  template <typename OtherType> Allocator(
    const Allocator<OtherType>&
    /**< [IN] Other allocator object*/
    )
    throw() {}

  /**
   * Destructs allocator object
   */
  ~Allocator() throw() {}

  /**
   * Gets address of an object
   *
   * \return Address of object
   *
   * \waitfree
   */
  pointer address(
    reference x
    /**< [IN] Reference to object */
    ) const {
    return &x;
  }

  /**
   * Gets address of a constant object
   *
   * \return Address of object
   *
   * \waitfree
   */
  const_pointer address(
    const_reference x
    /**< [IN] Reference to constant object */
    ) const {
    return &x;
  }

  /**
   * Allocates but doesn't initialize storage for elements of type Type
   *
   * \threadsafe
   *
   * \return Pointer to allocated storage
   *
   * \memory See Allocation::Allocate()
   */
  pointer allocate(
    size_type n,
    /**< [IN] Number of elements to allocate */
    const void* = 0
    /**< [IN] Optional pointer previously obtained from allocate */
    ) {
    if (n > max_size())
      EMBB_THROW(embb::base::NoMemoryException,
      "Amount of requested memory too high");
    return reinterpret_cast<pointer>
      (embb::base::Allocation::Allocate(n*sizeof(value_type)));
  }

  /**
   * Deallocates storage of destroyed elements.
   *
   * \threadsafe
   */
  void deallocate(
    pointer p,
    /**< [IN,OUT] Pointer to allocated storage */
    size_type
    ) {
    embb::base::Allocation::Free(p);
  }

  /**
   * %Allocation maximum
   *
   * \return Maximum number of elements that can be allocated
   *
   *\waitfree
   */
  size_type max_size() const throw() {
    return std::numeric_limits<size_type>::max() / sizeof(value_type);
  }

  /**
   * Initializes elements of allocated storage with specified value.
   *
   * \threadsafe
   */
  void construct(
    pointer p,
    /**< [IN,OUT] Pointer to allocated storage */
    const value_type& val
    /**< [IN] Value */
    ) {
    new(p)value_type(val);
  }

  /**
   * Destroys elements of initialized storage.
   *
   * \threadsafe
   */
  void destroy(
    pointer p
    /**< [IN,OUT] Pointer to allocated storage*/
    ) {
    EMBB_UNUSED(p);
    p->~value_type();
  }

 private:
  /*
   * \threadsafe
   */
  Allocator& operator=(const Allocator&);
};

/*
 * Forward declaration
 */
template <typename Type>
class AllocatorCacheAligned;

/*
 * Specialization for void
 */
template <>
class AllocatorCacheAligned < void > {
 public:
  typedef void*       pointer;
  typedef const void* const_pointer;
  typedef void        value_type;

  template <typename OtherType> struct rebind {
    typedef AllocatorCacheAligned<OtherType> other;
  };
};

/**
 * %Allocator according to the C++ standard. Allocates memory cache-aligned.
 *
 * For memory allocation and deallocation,
 * embb::base::Allocation::AllocateCacheAligned() and
 * embb::base::Allocation::FreeAligned() are used, respectively.
 *
 * In debug mode, leak checking is active. The function
 * embb::base::Allocation::AllocatedBytes() returns the number of
 * currently allocated bytes.
 *
 * \ingroup CPP_BASE_MEMORY_ALLOCATION
 */
template< typename Type >
class AllocatorCacheAligned : public Allocator < Type > {
 public:
  /** Quantity of elements type */
  typedef size_t      size_type;

  /** Difference between two pointers type */
  typedef ptrdiff_t   difference_type;

  /** Pointer to element type */
  typedef Type*       pointer;

  /** Pointer to constant element type */
  typedef const Type* const_pointer;

  /** Reference to element type */
  typedef Type&       reference;

  /** Reference to constant element type */
  typedef const Type& const_reference;

  /** Element type */
  typedef Type        value_type;

  /**
   * Rebind allocator to type OtherType
   */
  template <typename OtherType> struct rebind {
    /** Type to rebind to */
    typedef Allocator<OtherType> other;
  };

  /**
   * Constructs allocator object
   */
  AllocatorCacheAligned() throw() { }

  /**
   * Copies allocator object
   */
  AllocatorCacheAligned(
    const AllocatorCacheAligned& a
    /**< [IN] Other allocator object */
    ) throw()
    : Allocator < Type >(a) { }

  /**
   * Constructs allocator object
   *
   * Allows construction from allocators for different types (rebind)
   */
  template<typename OtherType>
  AllocatorCacheAligned(
    const AllocatorCacheAligned<OtherType>&
    /**< [IN] Other allocator object*/
    ) throw() { }

  /**
   * Destructs allocator object
   */
  ~AllocatorCacheAligned() throw() { }

  /**
   * Allocates but doesn't initialize storage for elements of type Type
   *
   * \threadsafe
   *
   * \return Pointer to allocated storage
   *
   * \memory see Allocation::Allocate()
   */
  pointer allocate(
    size_type n,
    /**< [IN] Number of elements to allocate */
    const void* = 0
    /**< [IN] Optional pointer previously obtained from allocate */
    ) {
    if (n > this->max_size())
      EMBB_THROW(embb::base::NoMemoryException,
      "Amount of requested memory too high");
    return reinterpret_cast<pointer>
      (embb::base::Allocation::AllocateCacheAligned(n*sizeof(value_type)));
  }

  /**
   * Deallocates storage of destroyed elements.
   *
   * \threadsafe
   */
  void deallocate(
    pointer p,
    /**< [IN,OUT] Pointer to allocated storage */
    size_type
    ) {
    embb::base::Allocation::FreeAligned(p);
  }
  // Inherit everything else.
};

/*
 * Comparison operator for Allocator objects.
 * Not for manual use.
 */
template<typename O>
inline bool operator==(const Allocator<O>&, const Allocator<O>&) {
  return true;
}

/*
 * Comparison operator for Allocator objects.
 * Not for manual use.
 */
template<typename T, typename U>
inline bool operator==(const Allocator<T>&, const Allocator<U>&) {
  return true;
}

/*
 * Comparison operator for Allocator objects.
 * Not for manual use.
 */
template<typename O>
inline bool operator!=(const Allocator<O>&, const Allocator<O>&) {
  return false;
}

/*
 * Comparison operator for Allocator objects.
 * Not for manual use.
 */
template<typename T, typename U>
inline bool
operator!=(const Allocator<T>&, const Allocator<U>&) {
  return false;
}

/*
 * Comparison operator for AllocatorCacheAligned objects.
 * Not for manual use.
 */
template<typename O>
inline bool operator==(
  const AllocatorCacheAligned<O>&, const AllocatorCacheAligned<O>&) {
  return true;
}

/*
 * Comparison operator for AllocatorCacheAligned objects.
 * Not for manual use.
 */
template<typename T, typename U>
inline bool operator==(
  const AllocatorCacheAligned<T>&, const AllocatorCacheAligned<U>&) {
  return true;
}

/*
 * Comparison operator for AllocatorCacheAligned objects.
 * Not for manual use.
 */
template<typename O>
inline bool operator!=(
  const AllocatorCacheAligned<O>&, const AllocatorCacheAligned<O>&) {
  return false;
}

/*
 * Comparison operator for AllocatorCacheAligned objects.
 * Not for manual use.
 */
template<typename T, typename U>
inline bool operator!=(
  const AllocatorCacheAligned<T>&, const AllocatorCacheAligned<U>&) {
  return false;
}
} // namespace base
} // namespace embb

#include <embb/base/internal/memory_allocation-inl.h>

#endif //  EMBB_BASE_MEMORY_ALLOCATION_H_
