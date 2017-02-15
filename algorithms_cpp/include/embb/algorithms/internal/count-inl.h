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

#ifndef EMBB_ALGORITHMS_INTERNAL_COUNT_INL_H_
#define EMBB_ALGORITHMS_INTERNAL_COUNT_INL_H_

#include <functional>
#include <embb/algorithms/reduce.h>
#include <embb/algorithms/internal/predicate_job_functor.h>

namespace embb {
namespace algorithms {
namespace internal {

template<typename ValueType>
class ValueComparisonFunction{
 public:
  explicit ValueComparisonFunction(const ValueType& value)
  : value_(value) {}
  ValueComparisonFunction(const ValueComparisonFunction& other)
  : value_(other.value_) {}

  template<typename ElementType>
  int operator()(ElementType element) {
    if (element == value_) {
      return 1;
    } else {
      return 0;
    }
  }
 private:
  const ValueType &value_;
  ValueComparisonFunction &operator=(
    const ValueComparisonFunction& other);
};

template<typename Function>
class FunctionComparisonFunction{
 public:
  explicit FunctionComparisonFunction(Function function)
  : function_(function) {}
  FunctionComparisonFunction(const FunctionComparisonFunction &other)
  : function_(other.function_) {}

  template<typename ElementType>
  int operator()(ElementType element) {
    if (function_(element)) {
      return 1;
    } else {
      return 0;
    }
  }
 private:
  Function function_;
  FunctionComparisonFunction &operator=(
    const FunctionComparisonFunction& other);
};

}  // namespace internal

template<typename RAI, typename ValueType>
typename std::iterator_traits<RAI>::difference_type
  Count(RAI first, RAI last, const ValueType& value,
        const embb::mtapi::ExecutionPolicy& policy, size_t block_size) {
  typedef typename std::iterator_traits<RAI>::difference_type Difference;
  return Reduce(first, last, Difference(0), std::plus<Difference>(),
                internal::ValueComparisonFunction<ValueType>(value), policy,
                block_size);
}

template<typename RAI>
typename std::iterator_traits<RAI>::difference_type
CountIf(RAI first, RAI last, embb::mtapi::Job comparison,
  const embb::mtapi::ExecutionPolicy& policy, size_t block_size) {
  typedef typename std::iterator_traits<RAI>::difference_type Difference;
  typedef internal::PredicateJobFunctor<
    typename std::iterator_traits<RAI>::value_type> Predicate;
  return Reduce(first, last, Difference(0), std::plus<Difference>(),
    internal::FunctionComparisonFunction<Predicate>(
      Predicate(comparison, policy)),
    policy, block_size);
}

template<typename RAI, typename ComparisonFunction>
typename std::iterator_traits<RAI>::difference_type
  CountIf(RAI first, RAI last, ComparisonFunction comparison,
          const embb::mtapi::ExecutionPolicy& policy, size_t block_size) {
  typedef typename std::iterator_traits<RAI>::difference_type Difference;
  return Reduce(first, last, Difference(0), std::plus<Difference>(),
                internal::FunctionComparisonFunction<ComparisonFunction>
                (comparison), policy, block_size);
}

}  // namespace algorithms
}  // namespace embb

#endif  // EMBB_ALGORITHMS_INTERNAL_COUNT_INL_H_
