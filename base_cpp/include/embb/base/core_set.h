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

#ifndef EMBB_BASE_CORE_SET_H_
#define EMBB_BASE_CORE_SET_H_

#include <embb/base/c/core_set.h>

namespace embb {
namespace base {

/**
 * \defgroup CPP_BASE_CORESET Core Set
 *
 * Core sets for thread-to-core affinities
 *
 * \ingroup CPP_BASE
 */

/**
 * Represents a set of processor cores, used to set thread-to-core affinities.
 *
 * An instance of this type represents a subset of processor cores. Core sets
 * can be used to set thread-to-core affinities. A core in a core set might
 * just represent a logical core (hyper-thread), depending on the underlying
 * hardware. Each core is identified by a unique integer starting with 0.
 * For example, the cores of a quad-core system are represented by the set
 * {0,1,2,3}.
 *
 * This class is essentially a wrapper for the underlying C implementation.
 *
 * \notthreadsafe
 * \ingroup CPP_BASE_CORESET
 */
class CoreSet {
 public:
  /**
   * Returns the number of available processor cores.
   *
   * If the processor supports hyper-threading, each hyper-thread is treated as
   * a separate processor core.
   *
   * \return Number of cores including hyper-threads
   */
  static unsigned int CountAvailable();

  /**
   * Constructs an empty core set.
   */
  CoreSet();

  /**
   * Constructs a core set with all or no cores.
   */
  explicit CoreSet(
    bool value
    /**< [IN] \c true includes all cores in the set, \c false excludes all */
    );

  /**
   * Constructs a copy of the specified core set.
   */
  CoreSet(
    const CoreSet& to_copy
    /**< [IN] Core set to copy */
    );

  /**
   * Assigns an existing core set.
   *
   * \return Reference to \c *this
   */
  CoreSet& operator=(
    const CoreSet& to_assign
    /**< [IN] Core set to assign */
    );

  /**
   * Resets the core set according to the specified value.
   */
  void Reset(
    bool value
    /**< [IN] \c true includes all cores in the set, \c false excludes all */
    );

  /**
   * Adds one core to the core set.
   */
  void Add(
    unsigned int core
    /**< [IN] Core to add (from 0 to number of cores - 1) */
    );

  /**
   * Removes one core from the core set.
   */
  void Remove(
    unsigned int core
    /** [IN] Core to remove (from 0 to number of cores - 1) */
    );

  /**
   * Checks whether the specified core is included in the set.
   *
   * \return \c true if core is included, otherwise \c false
   */
  bool IsContained(
    unsigned int core
    /**< [IN] Core to check (from 0 to number of cores - 1) */
    ) const;

  /**
   * Counts the number of cores in the set.
   *
   * \return Number of cores in the set
   */
  unsigned int Count() const;

  /**
   * Intersects this core set with the specified one.
   *
   * This core set is not modified by the operation.
   *
   * \return Copy of the result
   */
  CoreSet operator&(
    const CoreSet& rhs
    /** [IN] Core set on right-hand side of intersection operation */
    ) const;

  /**
   * Unites this core set with the specified one.
   *
   * This core set is not modified by the operation.
   *
   * \return Copy of the result
   */
  CoreSet operator|(
    const CoreSet& rhs
    /** [IN] Core set on right-hand side of union operation */
    ) const;

  /**
   * Intersects this core set with the specified one and overwrites this core
   * set.
   *
   * \return Reference to \c *this
   */
  CoreSet& operator&=(
    const CoreSet& rhs
    /** [IN] Core set on right-hand side of intersection operation */
    );

  /**
   * Unites this core set with the specified one an overwrites this core set.
   *
   * \return Reference to \c *this
   */
  CoreSet& operator|=(
    const CoreSet& rhs
    /** [IN] Core set on right-hand side of union operation */
    );

  /**
   * Provides access to internal representation to use it with C API.
   *
   * \return A reference to the internal embb_core_set_t structure.
   */
  embb_core_set_t const & GetInternal() const { return rep_; }

 private:
  /**
   * Internal representation of core set.
   */
  embb_core_set_t rep_;

  /**
   * Needs access to internal representation to use it with C API.
   */
  friend class Thread;
};

} // namespace base
} // namespace embb



#endif  // EMBB_BASE_CORE_SET_H_
