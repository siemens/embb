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

#ifndef EMBB_BASE_C_CORE_SET_H_
#define EMBB_BASE_C_CORE_SET_H_

#include <stdint.h>

/**
 * \defgroup C_BASE_CORESET Core Set
 *
 * Core sets for thread-to-core affinities
 *
 * \ingroup C_BASE
 * \{
 */

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Opaque type representing a set of processor cores.
 *
 * An instance of this type represents a subset of processor cores. Core sets
 * can be used to set thread-to-core affinities. A core in a core set might
 * just represent a logical core (hyper-thread), depending on the underlying
 * hardware. Each core is identified by a unique integer starting with 0.
 * For example, the cores of a quad-core system are represented by the set
 * {0,1,2,3}.
 *
 * \see embb_core_count_available()
 */
#ifdef DOXYGEN
typedef opaque_type embb_core_set_t;
#else
typedef struct embb_core_set_t {
  uint64_t rep;
} embb_core_set_t;
#endif /* else defined(DOXYGEN) */

/**
 * Returns the number of available processor cores.
 *
 * If the processor supports hyper-threading, each hyper-thread is treated as a
 * separate processor core.
 *
 * \return Number of cores including hyper-threads
 *
 * \notthreadsafe
 */
unsigned int embb_core_count_available();

/**
 * Initializes the specified core set.
 *
 * The second parameter specifies whether the set is initially empty or contains
 * all cores.
 *
 * \pre \c core_set is not NULL.
 *
 * \notthreadsafe
 */
void embb_core_set_init(
  embb_core_set_t* core_set,
  /**< [OUT] Core set to initialize */
  int initializer
  /**< [IN] The set is initially empty if initializer == 0, otherwise it
  contains all available processor cores. */
  );

/**
 * Adds a core to the specified set.
 *
 * If the core is already contained in the set, the operation has no effect.
 *
 * \pre \c core_set is not NULL and \c core_number is smaller than
 *      embb_core_count_available().
 *
 * \notthreadsafe
 * \see embb_core_set_remove()
 */
void embb_core_set_add(
  embb_core_set_t* core_set,
  /**< [IN/OUT] Core set to be manipulated */
  unsigned int core_number
  /**< [IN] Number of core to be added. */
  );

/**
 * Removes a core from the specified set.
 *
 * If the core is not in the set, the operation has no effect.
 *
 * \pre \c core_set is not NULL and \c core_number is smaller than
 *      embb_core_count_available().
 *
 * \notthreadsafe
 * \see embb_core_set_add()
 */
void embb_core_set_remove(
  embb_core_set_t* core_set,
  /**< [IN/OUT] Core set to be manipulated */
  unsigned int core_number
  /**< [IN] Number of core to be removed */
  );

/**
 * Determines whether a core is contained in the specified set.
 *
 * \pre \c core_set is not NULL and \c core_number is smaller than
 *      embb_core_count_available().
 *
 * \return 0 if the core is not contained in the set, otherwise a number
 *         greater than zero.
 * \notthreadsafe
 */
int embb_core_set_contains(
  const embb_core_set_t* core_set,
  /**< [IN] Core set */
  unsigned int core_number
  /**< [IN] Number of core */
  );

/**
 * Computes the intersection of core \c set1 and \c set2.
 *
 * The result is stored in \c set1.
 *
 * \pre \c set1 and \c set2 are not NULL.
 *
 * \notthreadsafe
 * \see embb_core_set_union()
 */
void embb_core_set_intersection(
  embb_core_set_t* set1,
  /**< [IN/OUT] First set, gets overwritten by the result */
  const embb_core_set_t* set2
  /**< [IN] Second set */
  );

/**
* Computes the union of core \c set1 and \c set2.
*
* The result is stored in \c set1.
*
* \pre \c set1 and \c set2 are not NULL.
*
* \notthreadsafe
* \see embb_core_set_intersection()
*/
void embb_core_set_union(
  embb_core_set_t* set1,
  /**< [IN/OUT] First set */
  const embb_core_set_t* set2
  /**< [IN] Second set */
  );

/**
 * Returns the number of cores contained in the specified set.
 *
 * \pre \c core_set is not NULL.
 *
 * \notthreadsafe
 * \return Number of cores in \c core_set
 */
unsigned int embb_core_set_count(
  const embb_core_set_t* core_set
  /**< [IN] Core set whose elements are counted */
  );

#ifdef __cplusplus
} /* Close extern "C" { */
#endif

/**
 * \}
 */

#endif /* EMBB_BASE_C_CORE_SET_H_ */
