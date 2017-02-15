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

#ifndef EMBB_DATAFLOW_INTERNAL_TYPELIST_H_
#define EMBB_DATAFLOW_INTERNAL_TYPELIST_H_

#include <embb/base/internal/nil.h>

namespace embb {
namespace dataflow {
namespace internal {

template <class T, class U>
struct TypeList {
  typedef T Head;
  typedef U Tail;
};

template <
  typename T1 = embb::base::internal::Nil,
  typename T2 = embb::base::internal::Nil,
  typename T3 = embb::base::internal::Nil,
  typename T4 = embb::base::internal::Nil,
  typename T5 = embb::base::internal::Nil>
struct MakeTypeList {
 private:
  typedef typename MakeTypeList<T2, T3, T4, T5>::Result TailResult;

 public:
  typedef TypeList<T1, TailResult> Result;
};

template <>
struct MakeTypeList<> {
  typedef embb::base::internal::Nil Result;
};

template <class TypeList> struct TypeListLength;

template <> struct TypeListLength<embb::base::internal::Nil> {
  enum { value = 0 };
};

template <class T, class U>
struct TypeListLength<TypeList<T, U> > {
  enum { value = 1 + TypeListLength<U>::value };
};

template <class TypeList, unsigned int Index> struct TypeAt;

template <class Head, class Tail>
struct TypeAt< TypeList<Head, Tail>, 0> {
  typedef Head Result;
};

template <class Head, class Tail, unsigned int Index>
struct TypeAt< TypeList<Head, Tail>, Index> {
  typedef typename TypeAt<Tail, Index - 1>::Result Result;
};

} // namespace internal
} // namespace dataflow
} // namespace embb

#endif // EMBB_DATAFLOW_INTERNAL_TYPELIST_H_
