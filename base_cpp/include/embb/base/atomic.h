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

#ifndef EMBB_BASE_ATOMIC_H_
#define EMBB_BASE_ATOMIC_H_

#include <stdint.h>
#include <cstddef>
#include <cassert>
#include <utility>

#include <embb/base/internal/atomic/atomic_base.h>
#include <embb/base/internal/atomic/atomic_pointer.h>
#include <embb/base/internal/atomic/atomic_integer.h>

namespace embb {
namespace base {
#ifdef DOXYGEN

/**
 * \defgroup CPP_BASE_ATOMIC Atomic
 * %Atomic operations.
 *
 * \ingroup CPP_BASE
 */

/**
 * Class representing atomic variables.
 *
 * The current implementation guarantees sequential consistency (full fences)
 * for all atomic operations. Relaxed memory models may be added in the future.
 *
 * \tparam BaseType Underlying type
 * \ingroup CPP_BASE_ATOMIC
 */
template<typename BaseType>
class Atomic {
 public:
  /**
   * Default constructor.
   *
   * Constructs an atomic variable holding zero.
   *
   * \waitfree
   *
   * \see Atomic(BaseType)
   */
  Atomic();

  /**
   * Valued-based constructor.
   *
   * Constructs an atomic variable holding the passed value.
   *
   * \param val Initial value
   *
   * \waitfree
   *
   * \note There is intentionally no copy constructor, since two different
   *       memory locations cannot be manipulated atomically.
   *
   * \see Atomic()
   */
  explicit Atomic(BaseType val);

  /**
   * Assignment operator.
   *
   * Assigns the passed value to the object.
   *
   * \waitfree
   *
   * \param val The value to assign
   *
   * \return A shallow copy of this object
   */
  BaseType operator=(BaseType val);

  /**
   * Type conversion.
   *
   * Returns the value of the object. Equivalent to Load().
   *
   * \waitfree
   *
   * \return Stored value
   *
   * \see Load()
   */
  operator BaseType() const;

  /**
   * Predicate representing support for arithmetic operations.
   *
   * Returns \c true if type \c BaseType supports arithmetic operations,
   * otherwise \c false. Only integers and non-void pointers support
   * arithmetic operations.
   *
   * \waitfree
   *
   * \return Boolean value indicating support for arithmetic operations
   *
   * \see IsInteger(), IsPointer()
   */
  bool IsArithmetic() const;

  /**
   * Predicate representing integers.
   *
   * Returns \c true if \c BaseType is an integer type, otherwise \c false.
   *
   * \waitfree
   *
   * \return Boolean value indicating whether \c BaseType is an integer
   *
   * \see IsArithmetic(), IsPointer()
   */
  bool IsInteger() const;

  /**
   * Predicate representing pointers.
   *
   * Returns \c true if \c BaseType is a non-void pointer type, otherwise
   * \c false.
   *
   * \waitfree
   *
   * \return Boolean value indicating whether \c BaseType is a non-void pointer
   *         type
   *
   * \see IsArithmetic(), IsInteger()
   */
  bool IsPointer() const;

  /**
   * Store operation.
   *
   * Stores the passed value in the object. Equivalent to assignment operator,
   * except that \c Store does not return anything.
   *
   * \waitfree
   *
   * \param val Value to be stored
   *
   * \see Load()
   */
  void Store(BaseType val);

  /**
   * Load operation.
   *
   * Loads and returns the stored value. Equivalent to type conversion.
   *
   * \waitfree
   *
   * \return Stored value
   *
   * \see Store()
   */
  BaseType Load() const;

  /**
   * Swap operation.
   *
   * Stores the given value in the object and returns the old value.
   *
   * \waitfree
   *
   * \param val New value
   *
   * \return Old value
   *
   * \see CompareAndSwap()
   */
  BaseType Swap(BaseType val);

  /**
   * Compare-and-Swap operation (CAS).
   *
   * Stores \c desired if the current value is equal to \c expected.
   * Otherwise, stores the current value in \c expected.
   *
   * \waitfree
   *
   * \param expected Expected value
   * \param desired Desired value
   *
   * \return \c true if CAS succeeded, otherwise \c false
   *
   * \see Swap()
   */
  bool CompareAndSwap(BaseType& expected, BaseType desired);

  /** @name Arithmetic members
   *
   * The following members are only available if \c BaseType supports arithmetic
   * operations (integer and non-void pointer types).
   *
   */

  /**@{*/

  /**
   * Fetch-and-Add operation.
   *
   * Adds the passed value and returns the old value.
   *
   * \waitfree
   *
   * \param val Addend
   *
   * \return Old value
   *
   * \see FetchAndSub()
   */
  BaseType FetchAndAdd(BaseType val);

  /**
   * Fetch-and-Sub operation.
   *
   * Subtracts the passed value and returns the old value.
   *
   * \waitfree
   *
   * \param val Subtrahend
   *
   * \return Old value
   *
   * \see FetchAndAdd()
   */
  BaseType FetchAndSub(BaseType val);

  /**
   * Post-increment operation.
   *
   * Increments the value and returns the old value.
   *
   * \waitfree
   *
   * \return Old value
   *
   * \see operator++()
   */
  BaseType operator++(int);

  /**
   * Post-decrement operation.
   *
   * Decrements the value and returns the old value.
   *
   * \waitfree
   *
   * \return Old value
   *
   * \see operator--()
   */
  BaseType operator--(int);

  /**
   * Pre-increment operation.
   *
   * Increments the value and returns the new value.
   *
   * \waitfree
   *
   * \return New value
   *
   * \see operator++(int)
   */
  BaseType operator++();

  /**
   * Pre-decrement operation.
   *
   * Decrements the value and returns the new value.
   *
   * \waitfree
   *
   * \return New value
   *
   * \see operator--(int)
   */
  BaseType operator--();

  /**
   * Assignment by sum operation.
   *
   * Adds the passed value and returns the new value.
   *
   * \param val Addend
   * \return New value
   *
   * \waitfree
   *
   * \see operator-=()
   */
  BaseType operator+=(BaseType val);

  /**
   * Assignment by difference operation.
   *
   * Subtracts the passed value and returns the new value.
   *
   * \param val Subtrahend
   * \return New value
   *
   * \waitfree
   *
   * \see operator+=()
   */
  BaseType operator-=(BaseType val);

  /**@}*/

  /** @name Integer members
   *
   * The following members are only available if \c BaseType is an integer type.
   *
   */

  /**@{*/

  /**
   * Assignment by bitwise AND.
   *
   * Stores the result of the bitwise AND in the current object.
   * Does not return anything, since this cannot be implemented
   * atomically on all architectures.
   *
   * \waitfree
   *
   * \param val Second operand of bitwise AND
   *
   * \see operator|=(), operator^=()
   */
  void operator&=(BaseType val);

  /**
   * Assignment by bitwise OR.
   *
   * Stores the result of the bitwise OR in the current object.
   * Does not return anything, since this cannot be implemented
   * atomically on all architectures.
   *
   * \waitfree
   *
   * \param val Second operand of bitwise OR
   *
   * \see operator&=(), operator^=()
   */
  void operator|=(BaseType val);

  /**
   * Assignment by bitwise XOR.
   *
   * Stores the result of the bitwise XOR in the current object.
   * Does not return anything, since this cannot be implemented
   * atomically on all architectures.
   *
   * \param val Second operand of bitwise XOR
   *
   * \waitfree
   *
   * \see operator&=(), operator|=()
   */
  void operator^=(BaseType val);

  /**@}*/

  /** @name Pointer members
   *
   * The following members are only available if \c BaseType is a non-void
   * pointer type.
   *
   */

  /**@{*/

  /**
   * Structure dereference operation.
   *
   * Used to access an element of an instance of a class or a structure
   * pointed to by the stored pointer.
   *
   * \return Stored pointer
   *
   * \waitfree
   *
   * \see operator*()
   */
  BaseType* operator->();

  /**
   * Dereference operation.
   *
   * Used to access the object pointed to by the stored pointer.
   *
   * \return Reference to the object
   *
   * \waitfree
   *
   * \see operator->()
   */
  BaseType& operator*();

  /**@}*/
};

#else

/**
  * Generic implementation that provides basic functionality.
  * See \c embb::base::AtomicBase for more information.
  *
  * \tparam BaseType Underlying type
  */
template<typename BaseType>
class Atomic : public embb::base::internal::atomic::AtomicBase < BaseType > {
 public:
  /**
   * Constructs an atomic variable holding an uninitialized value.
   */
  Atomic() : embb::base::internal::atomic::AtomicBase<BaseType>() {}

  /**
   * Constructs an atomic variable holding the passed value.
   *
   * \param val The value to assign.
   */
  explicit Atomic(BaseType val) : embb::base::internal::atomic::
    AtomicBase<BaseType>(val) {}

  /**
   * Assignment operator.
   *
   * \param val The value to assign.
   *
   * \return  A shallow copy of this object.
   */
  BaseType operator=(BaseType val) {
    return embb::base::internal::atomic::AtomicBase<BaseType>::
    operator=(val);
  }
};

/**
* Specialization for non-void pointer types.
* See \c embb::base::internal::atomic::AtomicPointer for more information.
*
* \tparam BaseType Type of the objects pointed to
*/
template<typename BaseType>
class Atomic<BaseType*> : public embb::base::internal::atomic::
  AtomicPointer < BaseType, ptrdiff_t, sizeof(BaseType*) > {
 public:
  Atomic() : embb::base::internal::atomic::
    AtomicPointer<BaseType, ptrdiff_t, sizeof(BaseType*)>() {}
  explicit Atomic(BaseType* p) : embb::base::internal::atomic::
    AtomicPointer<BaseType, ptrdiff_t, sizeof(BaseType*)>(p) {}

  BaseType* operator=(BaseType* p) {
    return embb::base::internal::atomic::
      AtomicPointer<BaseType, ptrdiff_t, sizeof(BaseType*)>::operator=(p);
  }
};

/**
* Specialization for void pointer types.
* See \c embb::base::internal::atomic::AtomicBase for more information.
* Unlike the specialization for non-void pointer types, this class
* does not permit dereferencing, incrementation, etc.
*/
template<>
class Atomic<void*> : public embb::base::internal::atomic::AtomicBase < void* > {
 public:
  Atomic() : embb::base::internal::atomic::AtomicBase<void*>() {}
  explicit Atomic(void* p) : embb::base::internal::atomic::AtomicBase<void*>(p)
  {}

  void* operator=(void* p) {
    return embb::base::internal::atomic::AtomicBase<void*>::operator=(p);
  }
};

/**
* Specializations for integers.
* See \c embb::base::internal::atomic::AtomicInteger for more information.
*/
#define __EMBB_ATOMIC_INTEGER_SPECIALIZATION(T) \
template<> \
class Atomic<T>: public embb::base::internal::atomic::AtomicInteger<T> { \
 public:  \
  \
  Atomic() : embb::base::internal::atomic::AtomicInteger<T>() {} \
  explicit Atomic(T val) : embb::base::internal::atomic::AtomicInteger<T>(val) \
  {} \
  \
  T operator=(T val) { return embb::base::internal::atomic::AtomicInteger<T>::\
  operator=(val); } \
  \
}

// Specializations for integers
__EMBB_ATOMIC_INTEGER_SPECIALIZATION(signed char);
__EMBB_ATOMIC_INTEGER_SPECIALIZATION(unsigned char);
__EMBB_ATOMIC_INTEGER_SPECIALIZATION(signed short);
__EMBB_ATOMIC_INTEGER_SPECIALIZATION(unsigned short);
__EMBB_ATOMIC_INTEGER_SPECIALIZATION(signed int);
__EMBB_ATOMIC_INTEGER_SPECIALIZATION(unsigned int);

#if defined EMBB_PLATFORM_ARCH_CXX11
__EMBB_ATOMIC_INTEGER_SPECIALIZATION(signed long);
__EMBB_ATOMIC_INTEGER_SPECIALIZATION(unsigned long);
__EMBB_ATOMIC_INTEGER_SPECIALIZATION(signed long long);
__EMBB_ATOMIC_INTEGER_SPECIALIZATION(unsigned long long);
#elif defined EMBB_PLATFORM_ARCH_X86_64
__EMBB_ATOMIC_INTEGER_SPECIALIZATION(size_t);
#endif

#endif
}  // namespace base
}  // namespace embb

#endif  // EMBB_BASE_ATOMIC_H_
