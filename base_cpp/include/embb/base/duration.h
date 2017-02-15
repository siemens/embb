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

#ifndef EMBB_BASE_DURATION_H_
#define EMBB_BASE_DURATION_H_

#include <embb/base/c/duration.h>
#include <embb/base/exceptions.h>
#include <cassert>

namespace embb {
namespace base {

/**
 * \defgroup CPP_BASE_TIMEDURATION Duration and Time
 *
 * Relative time durations and absolute time points
 *
 * \ingroup CPP_BASE
 */

/**
 * Represents a relative time duration for a given tick type.
 *
 * \notthreadsafe
 * \note The typedefs DurationSeconds, DurationMilliseconds,
 *       DurationMicroseconds, and DurationNanoseconds provide directly usable
 *       duration types.
 * \tparam Tick Possible tick types are Seconds, Milliseconds, Microseconds,
 *              Nanoseconds
 * \ingroup CPP_BASE_TIMEDURATION
 */
template<typename Tick>
class Duration {
 public:
  /**
   * Returns duration of length zero.
   * \return Duration of length zero
   */
  static const Duration<Tick>& Zero();

  /**
   * Returns duration with maximum ticks representable by implementation.
   *
   * This value depends on the tick type and on the platform.
   *
   * \return Reference to duration with maximum value
   */
  static const Duration<Tick>& Max();

  /**
   * Returns duration with minimum ticks representable by implementation.
   *
   * This value depends on the tick type and on the platform.
   *
   * \return Reference to duration with minimum value
   */
  static const Duration<Tick>& Min();

  /**
   * Constructs a duration of length zero.
   */
  Duration();

  /**
   * Constructs a duration with given number of ticks.
   */
  explicit Duration(
    unsigned long long ticks
    /**< [IN] Number of ticks */
    );

  /**
   * Constructs a duration by copying from an existing duration.
   */
  Duration(
    const Duration<Tick>& to_copy
    /**< [IN] %Duration to copy */
    );

  /**
   * Assigns an existing duration.
   *
   * \return Reference to \c *this
   */
  Duration<Tick>& operator=(
    const Duration<Tick>& to_assign
    /**< [IN] %Duration to assign */
    );

  /**
   * Returns the number of ticks of the duration.
   * \return Number of ticks of the duration
   */
  unsigned long long Count() const;

  /**
   * Assignment by addition of another duration with same tick type.
   *
   * \return Reference to \c *this
   */
  Duration<Tick>& operator+=(
    const Duration<Tick>& rhs
    /**< [IN] %Duration to add to this duration */
    );

 private:
  /**
   * Constructs a duration from the internal representation.
   *
   * \pre \c duration needs to fit into the duration type
   * \throws
   */
  Duration(
    const embb_duration_t& duration
    /**< [IN] %Duration to copy from */
    );

  /**
   * Internal representation from Base C.
   */
  embb_duration_t rep_;

  /**
   * For accessing rep_ and using Base C functionality.
   */
  friend class Time;

  /**
   * For accessing rep_ and using Base C functionality.
   */
  friend class ConditionVariable;
};

/**
 * Compares two durations (equality).
 *
 * \ingroup CPP_BASE_TIMEDURATION
 *
 * \return \c true if \c lhs is equal to \c rhs, otherwise \c false
 */
template<typename Tick>
bool operator==(
  const Duration<Tick>& lhs,
  /**< [IN] Left-hand side of equality operator */
  const Duration<Tick>& rhs
  /**< [IN] Right-hand side of equality operator */
  ) {
  return embb_duration_compare(&lhs, &rhs) == 0;
}

/**
 * Compares two durations (inequality).
 *
 * \ingroup CPP_BASE_TIMEDURATION
 *
 * \return \c true if \c lhs is not equal to \c rhs, otherwise \c false
 */
template<typename Tick>
bool operator!=(
  const Duration<Tick>& lhs,
  /**< [IN] Left-hand side of inequality operator */
  const Duration<Tick>& rhs
  /**< [IN] Right-hand side of inequality operator */
  ) {
  return embb_duration_compare(&lhs, &rhs) != 0;
}

/**
 * Compares two durations (less than)
 *
 * \ingroup CPP_BASE_TIMEDURATION
 *
 * \return \c true if \c lhs is shorter than \c rhs.
 */
template<typename Tick>
bool operator<(
  const Duration<Tick>& lhs,
  /**< [IN] Left-hand side of less than operator */
  const Duration<Tick>& rhs
  /**< [IN] Right-hand side of less than operator */
  ) {
  return embb_duration_compare(&lhs, &rhs) == -1;
}

/**
 * Compares two durations (greater than)
 *
 * \ingroup CPP_BASE_TIMEDURATION
 *
 * \return \c true if \c lhs is longer than \c rhs.
 */
template<typename Tick>
bool operator>(
  const Duration<Tick>& lhs,
  /**< [IN] Left-hand side of greater than operator */
  const Duration<Tick>& rhs
  /**< [IN] Right-hand side of greater than operator */
  ) {
  return embb_duration_compare(&lhs, &rhs) == 1;
}

/**
 * Compares two durations (less than or equal to)
 *
 * \ingroup CPP_BASE_TIMEDURATION
 *
 * \return \c true if \c lhs is shorter than or equal to \c rhs.
 */
template<typename Tick>
bool operator<=(
  const Duration<Tick>& lhs,
  /**< [IN] Left-hand side of less than or equal to operator */
  const Duration<Tick>& rhs
  /**< [IN] Right-hand side of less than or equal to operator */
  ) {
  return embb_duration_compare(&lhs, &rhs) < 1;
}

/**
 * Compares two durations (greater than or equal to)
 *
 * \ingroup CPP_BASE_TIMEDURATION
 *
 * \return \c true if \c lhs is longer than or equal to \c rhs.
 */
template<typename Tick>
bool operator>=(
  const Duration<Tick>& lhs,
  /**< [IN] Left-hand side of greater than or equal to operator */
  const Duration<Tick>& rhs
  /**< [IN] Right-hand side of greater than or equal to operator */
  ) {
  return embb_duration_compare(&lhs, &rhs) > -1;
}

/**
 * Adds two durations
 *
 * \ingroup CPP_BASE_TIMEDURATION
 *
 * \return Sum of \c lhs and \c rhs.
 */
template<typename Tick>
Duration<Tick> operator+(
  const Duration<Tick>& lhs,
  /**< [IN] Left-hand side of addition operator */
  const Duration<Tick>& rhs
  /**< [IN] Right-hand side of addition operator */
  ) {
  return Duration<Tick>(lhs.Count() + rhs.Count());
}

namespace internal {

/**
 * Base class for ticks.
 */
class Tick {
 public:
  /**
   * Checks the status for under- and overflow and, in such a case, throws an 
   * exception.
   */
  static void CheckExceptions(
    int status,
    /**< [IN] Status code to check */
    const char* msg
    /**< [IN] Exception message if one is thrown */
    );
};

/**
 * %Seconds tick for Duration.
 *
 * \see Milliseconds, Microseconds, Nanoseconds
 * \ingroup CPP_BASE_TIMEDURATION
 */
class Seconds : public Tick {
 public:
  /**
   * Sets the ticks as seconds and returns the status of the duration operation.
   *
   * \return Status code of embb_duration_set_seconds()
   */
  static int Set(
    embb_duration_t& duration,
    /**< [OUT] %Duration representation to be considered */
    unsigned long long ticks
    /**< [IN] Number of ticks to set */
    );

  /**
   * Sets the ticks as seconds and calls CheckExceptions() with the status.
   */
  static void SetAndCheck(
    embb_duration_t& duration,
    /**< [OUT] %Duration representation to be considered */
    unsigned long long ticks
    /**< [IN] Number of ticks to set */
    );

  /**
   * Returns the number of ticks in seconds for the given duration.
   *
   * \return Number of second ticks of \c duration
   */
  static unsigned long long Get(
    const embb_duration_t& duration
    /**< [IN] %Duration representation to be considered */
    );

  /**
   * Returns the minimum number of second ticks for the available implementation.
   *
   * \return Minimum number of second ticks.
   */
  static unsigned long long Min();

  /**
   * Returns the maximum number of second ticks for the available implementation.
   *
   * \return Maximum number of second ticks.
   */
  static unsigned long long Max();
};

/**
 * %Milliseconds tick for Duration.
 *
 * \see Seconds, Microseconds, Nanoseconds
 * \ingroup CPP_BASE_TIMEDURATION
 */
class Milliseconds : public Tick {
 public:
  /**
   * Sets the ticks as milliseconds and returns the status of the duration
   * operation.
   *
   * \return Status code of embb_duration_set_milliseconds()
   */
  static int Set(
    embb_duration_t& duration,
    /**< [OUT] %Duration representation to be considered */
    unsigned long long ticks
    /**< [IN] Number of ticks to set */
    );

  /**
   * Sets the ticks as milliseconds and calls CheckExceptions() with the status.
   */
  static void SetAndCheck(
    embb_duration_t& duration,
    /**< [OUT] %Duration representation to be considered */
    unsigned long long ticks
    /**< [IN] Number of ticks to set */
    );

  /**
   * Returns the number of ticks in milliseconds for the given duration.
   *
   * \return Number of millisecond ticks of \c duration
   */
  static unsigned long long Get(
    const embb_duration_t& duration
    /**< [IN] %Duration representation to be considered */
    );

  /**
   * Returns the minimum number of millisecond ticks for the available
   * implementation.
   *
   * \return Minimum number of microsecond ticks.
   */
  static unsigned long long Min();

  /**
   * Returns the maximum number of millisecond ticks for the available
   * implementation.
   *
   * \return Maximum number of millisecond ticks.
   */
  static unsigned long long Max();
};

/**
 * %Microseconds tick for Duration.
 *
 * \see Seconds, Milliseconds, Nanoseconds
 * \ingroup CPP_BASE_TIMEDURATION
 */
class Microseconds : public Tick {
 public:
  /**
   * Sets the ticks as microseconds and returns the status of the duration
   * operation.
   *
   * \return Status code of embb_duration_set_microseconds()
   */
  static int Set(
    embb_duration_t& duration,
    /**< [OUT] %Duration representation to be considered */
    unsigned long long ticks
    /**< [IN] Number of ticks to set */
    );

  /**
   * Sets the ticks as microseconds and calls CheckExceptions() with the status.
   */
  static void SetAndCheck(
    embb_duration_t& duration,
    /**< [OUT] %Duration representation to be considered */
    unsigned long long ticks
    /**< [IN] Number of ticks to set */
    );

  /**
   * Returns the number of ticks in microseconds for the given duration.
   *
   * \return Number of microsecond ticks of \c duration
   */
  static unsigned long long Get(
    const embb_duration_t& duration
    /**< [IN] %Duration representation to be considered */
    );

  /**
   * Returns the minimum number of microsecond ticks for the available
   * implementation.
   *
   * \return Minimum number of microsecond ticks.
   */
  static unsigned long long Min();

  /**
   * Returns the maximum number of microsecond ticks for the available
   * implementation.
   *
   * \return Maximum number of microsecond ticks.
   */
  static unsigned long long Max();
};

/**
 * %Nanoseconds tick for Duration.
 *
 * \see Seconds, Milliseconds, Microseconds
 * \ingroup CPP_BASE_TIMEDURATION
 */
class Nanoseconds : public Tick {
 public:
  /**
   * Sets the ticks as nanoseconds and returns the status of the duration
   * operation.
   *
   * \return Status code of embb_duration_set_nanoseconds()
   */
  static int Set(
    embb_duration_t& duration,
    /**< [OUT] %Duration representation to be considered */
    unsigned long long ticks
    /**< [IN] Number of ticks to set */
    );

  /**
   * Sets the ticks as nanoseconds and calls CheckExceptions() with the status.
   */
  static void SetAndCheck(
    embb_duration_t& duration,
    /**< [OUT] %Duration representation to be considered */
    unsigned long long ticks
    /**< [IN] Number of ticks to set */
    );

  /**
   * Returns the number of ticks in nanoseconds for the given duration.
   *
   * \return Number of nanosecond ticks of \c duration
   */
  static unsigned long long Get(
    const embb_duration_t& duration
    /**< [IN] %Duration representation to be considered */
    );

  /**
   * Returns the minimum number of nanosecond ticks for the available
   * implementation.
   *
   * \return Minimum number of nanosecond ticks.
   */
  static unsigned long long Min();

  /**
   * Returns the maximum number of nanosecond ticks for the available
   * implementation.
   *
   * \return Maximum number of nanosecond ticks.
   */
  static unsigned long long Max();
};

} // namespace internal

/**
 * Duration with seconds tick.
 *
 * \ingroup CPP_BASE_TIMEDURATION
 */
typedef Duration<internal::Seconds> DurationSeconds;
/**
 * Duration with milliseconds tick.
 *
 * \ingroup CPP_BASE_TIMEDURATION
 */
typedef Duration<internal::Milliseconds> DurationMilliseconds;
/**
 * Duration with microseconds tick.
 *
 * \ingroup CPP_BASE_TIMEDURATION
 */
typedef Duration<internal::Microseconds> DurationMicroseconds;
/**
 * Duration with nanoseconds tick.
 *
 * \ingroup CPP_BASE_TIMEDURATION
 */
typedef Duration<internal::Nanoseconds> DurationNanoseconds;

} // namespace base
} // namespace embb

#include <embb/base/internal/duration-inl.h>

#endif  // EMBB_BASE_DURATION_H_
