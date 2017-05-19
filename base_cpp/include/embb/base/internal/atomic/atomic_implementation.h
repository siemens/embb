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

#ifndef EMBB_BASE_INTERNAL_ATOMIC_ATOMIC_IMPLEMENTATION_H_
#define EMBB_BASE_INTERNAL_ATOMIC_ATOMIC_IMPLEMENTATION_H_

#ifdef DOXYGEN
// Omit this file from Doxygen generation.
#else

#include <embb/base/c/atomic.h>
#include <embb/base/c/internal/macro_helper.h>

/**
 * The macros below are used to generate calls to the base_c implementation. We
 * do not implement a macro for each atomic method, as many methods are equal in
 * method signature. Therefore, we only define macros for different method
 * signatures, which actually define template specializations. The signature is
 * reflected in the macro name. For example the prefix
 * RET_VAL_PAR1_POINTER_PAR2_VALUE means that the signature of the atomic has
 * return type value, the first parameter is a pointer and the second one a
 * value. For each set of template specializations, a master template has to be
 * defined. This can be done with the EMBB_ATOMIC_GENERAL_TEMPLATE macro.
 * Example:
 *
 * This code...
 * ====
 * EMBB_ATOMIC_GENERAL_TEMPLATE(load_implementation)
 * EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VAL_PAR1_POINTER (1,
 * load_implementation, Load, embb_internal__atomic_load_
 * ====
 *
 * ...will generate: 
 * ====
 * template< typename AtomicType, typename NativeType > class load_implementation {};
 * 
 * template<> class load_implementation< EMBB_BASE_BASIC_TYPE_ATOMIC_1, EMBB_BASE_BASIC_TYPE_SIZE_1 > {
 *  public: 
 *   static inline EMBB_BASE_BASIC_TYPE_SIZE_1 Load
 *   ( EMBB_BASE_BASIC_TYPE_ATOMIC_1 * par1) { 
 *     return embb_internal__atomic_load_1(par1); 
 *   } 
 * };
 * ====
 */

/**
* \def EMBB_ATOMIC_GENERAL_TEMPLATE
* See general comment on top of file
* \param [in] IMPLEMENTATION_CLASS The wrapper class to be defined in the C++
*             lib (here)
*/
#define EMBB_ATOMIC_GENERAL_TEMPLATE(IMPLEMENTATION_CLASS) \
  template< typename AtomicType, typename NativeType > \
class IMPLEMENTATION_CLASS \
{};

/**
 * \def EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VAL_PAR1_POINTER
 * See general comment on top of file
 * \param [in] SIZE The size in bytes of the implementation that shall be called
 * \param [in] IMPLEMENTATION_CLASS The wrapper class to be defined in the C++
 *             lib (here)
 * \param [in] IMPLEMENTATION_METHOD The wrapper method that should be defined
 *             in IMPLEMENTATION_CLASS
 * \param [in] IMPLEMENTATION_METHOD The prefix of the method name, as it can be
 *             found in the base_c implementation.
 */
#define EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VAL_PAR1_POINTER\
(SIZE, IMPLEMENTATION_CLASS, IMPLEMENTATION_METHOD, NATIVE_PREFIX) \
template<> \
class IMPLEMENTATION_CLASS< EMBB_CAT2(EMBB_BASE_BASIC_TYPE_ATOMIC_, SIZE), \
EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, SIZE) > \
{ \
public: \
    static inline EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, SIZE) \
    IMPLEMENTATION_METHOD(\
    EMBB_CAT2(EMBB_BASE_BASIC_TYPE_ATOMIC_, SIZE) * par1) \
    { \
    return EMBB_CAT2(NATIVE_PREFIX, SIZE)(par1); \
    } \
};

/**
 * \def EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VAL_PAR1_POINTER_PAR2_VALUE
 * See general comment on top of file
 * \param [in] SIZE The size in bytes of the implementation that shall be called
 * \param [in] IMPLEMENTATION_CLASS The wrapper class to be defined in the C++
 *             lib (here)
 * \param [in] IMPLEMENTATION_METHOD The wrapper method that should be defined
 *             in IMPLEMENTATION_CLASS
 * \param [in] IMPLEMENTATION_METHOD The prefix of the method name, as it can be
 *             found in the base_c implementation.
 */
#define EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VAL_PAR1_POINTER_PAR2_VALUE\
(SIZE, IMPLEMENTATION_CLASS, IMPLEMENTATION_METHOD, NATIVE_PREFIX) \
template<> \
class IMPLEMENTATION_CLASS< EMBB_CAT2(EMBB_BASE_BASIC_TYPE_ATOMIC_, SIZE), \
EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, SIZE) > \
{ \
public: \
    static inline EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, SIZE) \
    IMPLEMENTATION_METHOD(\
    EMBB_CAT2(EMBB_BASE_BASIC_TYPE_ATOMIC_, SIZE) * par1, \
    EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, SIZE) par2) \
    { \
    return EMBB_CAT2(NATIVE_PREFIX, SIZE)(par1, par2); \
    } \
};

/**
 * \def EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_INT_PAR1_POINTER_PAR2_POINTER_PAR3_VAL
 * See general comment on top of file
 * \param [in] SIZE The size in bytes of the implementation that shall be called
 * \param [in] IMPLEMENTATION_CLASS The wrapper class to be defined in the C++
 *             lib (here)
 * \param [in] IMPLEMENTATION_METHOD The wrapper method that should be defined
 *             in IMPLEMENTATION_CLASS
 * \param [in] IMPLEMENTATION_METHOD The prefix of the method name, as it can be
 *             found in the base_c implementation.
 */
#define EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_INT_PAR1_POINTER_PAR2_POINTER_PAR3_VAL\
(SIZE, IMPLEMENTATION_CLASS, IMPLEMENTATION_METHOD, NATIVE_PREFIX) \
template<> \
class IMPLEMENTATION_CLASS< EMBB_CAT2(EMBB_BASE_BASIC_TYPE_ATOMIC_, SIZE), \
EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, SIZE) > \
{ \
public: \
    static inline int \
    IMPLEMENTATION_METHOD(\
    EMBB_CAT2(EMBB_BASE_BASIC_TYPE_ATOMIC_, SIZE) * par1, \
    EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, SIZE) * par2, \
    EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, SIZE) par3) \
    { \
      return EMBB_CAT2(NATIVE_PREFIX, SIZE)(par1, par2, par3); \
    } \
};

/**
 * \def EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VOID_PAR1_POINTER_PAR2_VAL
 * See general comment on top of file
 * \param [in] SIZE The size in bytes of the implementation that shall be called
 * \param [in] IMPLEMENTATION_CLASS The wrapper class to be defined in the C++
 *             lib (here)
 * \param [in] IMPLEMENTATION_METHOD The wrapper method that should be defined
 *             in IMPLEMENTATION_CLASS
 * \param [in] IMPLEMENTATION_METHOD The prefix of the method name, as it can be
 *             found in the base_c implementation.
 */
#define EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VOID_PAR1_POINTER_PAR2_VAL\
(SIZE, IMPLEMENTATION_CLASS, IMPLEMENTATION_METHOD, NATIVE_PREFIX) \
template<> \
class IMPLEMENTATION_CLASS< EMBB_CAT2(EMBB_BASE_BASIC_TYPE_ATOMIC_, SIZE), \
EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, SIZE) > \
{ \
public: \
    static inline void \
    IMPLEMENTATION_METHOD(\
    EMBB_CAT2(EMBB_BASE_BASIC_TYPE_ATOMIC_, SIZE) * par1, \
    EMBB_CAT2(EMBB_BASE_BASIC_TYPE_SIZE_, SIZE) par2) \
    { \
    EMBB_CAT2(NATIVE_PREFIX, SIZE)(par1, par2); \
    } \
};

namespace embb {
namespace base {
namespace internal {
namespace atomic {

/**
 * The macro calls below actually define the atomic implementations using the
 * macros defined in this file, as documented above.
 */

// load_implementation
EMBB_ATOMIC_GENERAL_TEMPLATE(load_implementation)
EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VAL_PAR1_POINTER
(1, load_implementation, Load, embb_internal__atomic_load_)
EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VAL_PAR1_POINTER
(2, load_implementation, Load, embb_internal__atomic_load_)
EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VAL_PAR1_POINTER
(4, load_implementation, Load, embb_internal__atomic_load_)
#ifdef EMBB_64_BIT_ATOMIC_AVAILABLE
EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VAL_PAR1_POINTER
(8, load_implementation, Load, embb_internal__atomic_load_)
#endif

// fetch_and_add_implementation
EMBB_ATOMIC_GENERAL_TEMPLATE(fetch_and_add_implementation)
EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VAL_PAR1_POINTER_PAR2_VALUE
(1, fetch_and_add_implementation, fetch_and_add, embb_internal__atomic_fetch_and_add_)
EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VAL_PAR1_POINTER_PAR2_VALUE
(2, fetch_and_add_implementation, fetch_and_add, embb_internal__atomic_fetch_and_add_)
EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VAL_PAR1_POINTER_PAR2_VALUE
(4, fetch_and_add_implementation, fetch_and_add, embb_internal__atomic_fetch_and_add_)
#ifdef EMBB_64_BIT_ATOMIC_AVAILABLE
EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VAL_PAR1_POINTER_PAR2_VALUE
(8, fetch_and_add_implementation, fetch_and_add, embb_internal__atomic_fetch_and_add_)
#endif

// store_implementation
EMBB_ATOMIC_GENERAL_TEMPLATE(store_implementation)
EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VOID_PAR1_POINTER_PAR2_VAL
(1, store_implementation, Store, embb_internal__atomic_store_)
EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VOID_PAR1_POINTER_PAR2_VAL
(2, store_implementation, Store, embb_internal__atomic_store_)
EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VOID_PAR1_POINTER_PAR2_VAL
(4, store_implementation, Store, embb_internal__atomic_store_)
#ifdef EMBB_64_BIT_ATOMIC_AVAILABLE
EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VOID_PAR1_POINTER_PAR2_VAL
(8, store_implementation, Store, embb_internal__atomic_store_)
#endif

// and_assign_implementation
EMBB_ATOMIC_GENERAL_TEMPLATE(and_assign_implementation)
EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VOID_PAR1_POINTER_PAR2_VAL
(1, and_assign_implementation, and_assign, embb_internal__atomic_and_assign_)
EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VOID_PAR1_POINTER_PAR2_VAL
(2, and_assign_implementation, and_assign, embb_internal__atomic_and_assign_)
EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VOID_PAR1_POINTER_PAR2_VAL
(4, and_assign_implementation, and_assign, embb_internal__atomic_and_assign_)
#ifdef EMBB_64_BIT_ATOMIC_AVAILABLE
EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VOID_PAR1_POINTER_PAR2_VAL
(8, and_assign_implementation, and_assign, embb_internal__atomic_and_assign_)
#endif

// compare_and_swap_implementation
EMBB_ATOMIC_GENERAL_TEMPLATE(compare_and_swap_implementation)
EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_INT_PAR1_POINTER_PAR2_POINTER_PAR3_VAL
(1, compare_and_swap_implementation, compare_and_swap, embb_internal__atomic_compare_and_swap_)
EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_INT_PAR1_POINTER_PAR2_POINTER_PAR3_VAL
(2, compare_and_swap_implementation, compare_and_swap, embb_internal__atomic_compare_and_swap_)
EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_INT_PAR1_POINTER_PAR2_POINTER_PAR3_VAL
(4, compare_and_swap_implementation, compare_and_swap, embb_internal__atomic_compare_and_swap_)
#ifdef EMBB_64_BIT_ATOMIC_AVAILABLE
EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_INT_PAR1_POINTER_PAR2_POINTER_PAR3_VAL
(8, compare_and_swap_implementation, compare_and_swap, embb_internal__atomic_compare_and_swap_)
#endif

// or_assign_implementation
EMBB_ATOMIC_GENERAL_TEMPLATE(or_assign_implementation)
EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VOID_PAR1_POINTER_PAR2_VAL
(1, or_assign_implementation, or_assign, embb_internal__atomic_or_assign_)
EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VOID_PAR1_POINTER_PAR2_VAL
(2, or_assign_implementation, or_assign, embb_internal__atomic_or_assign_)
EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VOID_PAR1_POINTER_PAR2_VAL
(4, or_assign_implementation, or_assign, embb_internal__atomic_or_assign_)
#ifdef EMBB_64_BIT_ATOMIC_AVAILABLE
EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VOID_PAR1_POINTER_PAR2_VAL
(8, or_assign_implementation, or_assign, embb_internal__atomic_or_assign_)
#endif

// xor_assign_implementation
EMBB_ATOMIC_GENERAL_TEMPLATE(xor_assign_implementation)
EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VOID_PAR1_POINTER_PAR2_VAL
(1, xor_assign_implementation, xor_assign, embb_internal__atomic_xor_assign_)
EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VOID_PAR1_POINTER_PAR2_VAL
(2, xor_assign_implementation, xor_assign, embb_internal__atomic_xor_assign_)
EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VOID_PAR1_POINTER_PAR2_VAL
(4, xor_assign_implementation, xor_assign, embb_internal__atomic_xor_assign_)
#ifdef EMBB_64_BIT_ATOMIC_AVAILABLE
EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VOID_PAR1_POINTER_PAR2_VAL
(8, xor_assign_implementation, xor_assign, embb_internal__atomic_xor_assign_)
#endif

// swap_implementation
EMBB_ATOMIC_GENERAL_TEMPLATE(swap_implementation)
EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VAL_PAR1_POINTER_PAR2_VALUE
(1, swap_implementation, Swap, embb_internal__atomic_swap_)
EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VAL_PAR1_POINTER_PAR2_VALUE
(2, swap_implementation, Swap, embb_internal__atomic_swap_)
EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VAL_PAR1_POINTER_PAR2_VALUE
(4, swap_implementation, Swap, embb_internal__atomic_swap_)
#ifdef EMBB_64_BIT_ATOMIC_AVAILABLE
EMBB_ATOMIC_IMPLEMENTATION_FOR_SIZE_RET_VAL_PAR1_POINTER_PAR2_VALUE
(8, swap_implementation, Swap, embb_internal__atomic_swap_)
#endif

}  // namespace atomic
}  // namespace internal
}  // namespace base
}  // namespace embb

#endif // DOXYGEN

#endif // EMBB_BASE_INTERNAL_ATOMIC_ATOMIC_IMPLEMENTATION_H_
