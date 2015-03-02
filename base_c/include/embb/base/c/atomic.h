/*
 * Copyright (c) 2014-2015, Siemens AG. All rights reserved.
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

#ifndef EMBB_BASE_C_ATOMIC_H_
#define EMBB_BASE_C_ATOMIC_H_

/**
 * \defgroup C_BASE_ATOMIC Atomic
 * 
 * \ingroup C_BASE
 *
 * Atomic operations.
 *
 * \anchor general_desc_atomic_base
 * Atomic operations are not directly applied to fundamental types. Instead,
 * there is for each character and integer type an associated atomic type that
 * has the same bit width (if the target CPU supports atomic operations on
 * that type):
 *
 * Fundamental type   | Atomic type
 * :------------------| :----------------------------------
 * char               | embb_atomic_char
 * short              | embb_atomic_short
 * unsigned short     | embb_atomic_unsigned_short
 * int                | embb_atomic_int
 * unsigned int       | embb_atomic_unsigned_int
 * long               | embb_atomic_long
 * unsigned long      | embb_atomic_unsigned_long
 * long long          | embb_atomic_long_long
 * unsigned long long | embb_atomic_unsigned_long_long
 * intptr_t           | embb_atomic_intptr_t
 * uintptr_t          | embb_atomic_uintptr_t
 * size_t             | embb_atomic_size_t
 * ptrdiff_t          | embb_atomic_ptrdiff_t
 * uintmax_t          | embb_atomic_uintmax_t
 *
 * Each of the atomic operations described in the following can be applied to
 * the types listed above. However, to avoid unnecessary redundancy, we document
 * them only once in a generic way. The keyword TYPE serves as a placeholder
 * which has to be replaced by the concrete type (e.g., int). If the fundamental
 * type contains spaces (e.g., unsigned int), "_" is used for concatenation
 * (e.g. unsigned_int).
 *
 * Usage example:
 * --------------
 *
 * Store the value \c 5 in an atomic \c "unsigned int" variable.
 *
 * Step 1 (declare atomic variable):
 * \code
 * embb_atomic_unsigned_int my_var;
 * \endcode

 * Step 2 (store the value):
 * \code
 * embb_atomic_store_unsigned_int( &my_var, 5 );
 * \endcode
 *
 * The current implementation guarantees sequential consistency (full fences)
 * for all atomic operations. Relaxed memory models may be added in the future.
 */

#ifdef DOXYGEN
/**
 * Computes the logical "and" of the value stored in \p variable and \c value.
 *
 * The result is stored in \p variable.
 *
 * \see \ref general_desc_atomic_base "Detailed description" for general
 * information and the meaning of \b TYPE.
 *
 * \ingroup C_BASE_ATOMIC
 * \waitfree
 */
EMBB_PLATFORM_INLINE void embb_atomic_and_assign_TYPE(
  embb_atomic_TYPE* variable,
  /**< [IN,OUT] Pointer to atomic variable which serves as left-hand side for
                the "and" operation and is used to store the result. */
  TYPE value
  /** [IN] Right-hand side of "and" operation, passed by value */
  );

/**
 * Compares \p variable with \p expected and, if equivalent, swaps its value
 * with \p desired.
 *
 * Stores \p desired in \p variable if the value of \p variable is equivalent
 * to the value of \p expected. Otherwise, stores the value of \p variable in
 * \p expected.
 *
 * \return != 0 if the values of \p variable and \p expected were equivalent \n
 *          0 otherwise
 *
 * \see \ref general_desc_atomic_base "Detailed description" for general
 * information and the meaning of \b TYPE.
 *
 * \ingroup C_BASE_ATOMIC
 * \waitfree
 */
EMBB_PLATFORM_INLINE int embb_atomic_compare_and_swap_TYPE(
  embb_atomic_TYPE* variable,
  /**< [IN,OUT] Pointer to atomic variable */
  TYPE* expected,
  /**< [IN,OUT] Pointer to expected value */
  TYPE desired
  /**< [IN] Value to be stored in \p variable */
  );

/**
 * Adds \p value to \p variable and returns its old value.
 *
 * \return The value before the operation
 *
 * \see \ref general_desc_atomic_base "Detailed description" for general
 * information and the meaning of \b TYPE.
 *
 * \ingroup C_BASE_ATOMIC
 * \waitfree
 */
EMBB_PLATFORM_INLINE TYPE embb_atomic_fetch_and_add_TYPE(
  embb_atomic_TYPE* variable,
  /**< [IN,OUT] Pointer to atomic variable*/
  TYPE value
  /**< [IN] The value to be added to \p variable (can be negative) */
  );

/**
 * Loads the value of \p variable and returns it.
 *
 * \return The value of the atomic variable.
 *
 * \see \ref general_desc_atomic_base "Detailed description" for general
 * information and the meaning of \b TYPE.
 *
 * \ingroup C_BASE_ATOMIC
 * \waitfree
 */
EMBB_PLATFORM_INLINE TYPE embb_atomic_load_TYPE(
  const embb_atomic_TYPE* variable
  /**< [IN] Pointer to atomic variable */
  );

/**
 * Enforces a memory barrier (full fence).
 *
 * \ingroup C_BASE_ATOMIC
 * \waitfree
 */
EMBB_PLATFORM_INLINE void embb_atomic_memory_barrier();

/**
 * Computes the logical "or" of the value stored in \p variable and \c value.
 *
 * The result is stored in \p variable.
 *
 * \see \ref general_desc_atomic_base "Detailed description" for general
 * information and the meaning of \b TYPE.
 *
 * \ingroup C_BASE_ATOMIC
 * \waitfree
 */
EMBB_PLATFORM_INLINE void embb_atomic_or_assign_TYPE(
  embb_atomic_TYPE* variable,
  /**< [IN,OUT] Pointer to atomic variable which serves as left-hand side for
                the "or" operation and is used to store the result. */
  TYPE value
  /** [IN] Right-hand side of "or" operation, passed by value */
  );

/**
 * Stores \p value in \p variable.
 *
 * \see \ref general_desc_atomic_base "Detailed description" for general
 * information and the meaning of \b TYPE.
 *
 * \ingroup C_BASE_ATOMIC
 * \waitfree
 */
EMBB_PLATFORM_INLINE void embb_atomic_store_TYPE(
  embb_atomic_TYPE* variable,
  /**< [IN,OUT] Pointer to atomic variable */
  int value
  /**< [IN] Value to be stored */
  );

/**
 * Swaps the current value of \p variable with \p value.
 *
 * \return The old value of \p variable
 *
 * \see \ref general_desc_atomic_base "Detailed description" for general
 * information and the meaning of \b TYPE.
 *
 * \ingroup C_BASE_ATOMIC
 * \waitfree
 */
EMBB_PLATFORM_INLINE TYPE embb_atomic_swap_TYPE(
  embb_atomic_TYPE* variable,
  /**< [IN,OUT] Pointer to atomic variable whose value is swapped */
  TYPE value
  /** [IN] Value which will be stored in the atomic variable */
  );

/**
* Computes the logical "xor" of the value stored in \p variable and \c value.
*
* The result is stored in \p variable.
*
* \see \ref general_desc_atomic_base "Detailed description" for general
* information and the meaning of \b TYPE.
*
* \ingroup C_BASE_ATOMIC
* \waitfree
*/
EMBB_PLATFORM_INLINE void embb_atomic_xor_assign_TYPE(
  embb_atomic_TYPE* variable,
  /**< [IN,OUT] Pointer to atomic variable which serves as left-hand side for
                the "xor" operation and is used to store the result. */
  TYPE value
  /** [IN] Right-hand side of "xor" operation, passed by value */
  );
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <embb/base/c/internal/platform.h>
#include <embb/base/c/internal/atomic/atomic_sizes.h>
#include <embb/base/c/internal/atomic/atomic_variables.h>
#include <embb/base/c/internal/macro_helper.h>
#include <embb/base/c/internal/atomic/load.h>
#include <embb/base/c/internal/atomic/and_assign.h>
#include <embb/base/c/internal/atomic/store.h>
#include <embb/base/c/internal/atomic/or_assign.h>
#include <embb/base/c/internal/atomic/xor_assign.h>
#include <embb/base/c/internal/atomic/swap.h>
#include <embb/base/c/internal/atomic/fetch_and_add.h>
#include <embb/base/c/internal/atomic/compare_and_swap.h>
#include <embb/base/c/internal/atomic/memory_barrier.h>

#ifdef __cplusplus
}
#endif

#endif //EMBB_BASE_C_ATOMIC_H_
