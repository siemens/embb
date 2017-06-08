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

#ifndef EMBB_DATAFLOW_INTERNAL_TUPLE_H_
#define EMBB_DATAFLOW_INTERNAL_TUPLE_H_

#include <cstddef>

#include <embb/dataflow/internal/typelist.h>

namespace embb {
namespace dataflow {
namespace internal {

template <int Index, typename T>
class TupleLeaf {
 public:
  typedef T Type;

  enum { index = Index };

  enum { count = 1 };

  TupleLeaf() : value_() {}

  TupleLeaf(TupleLeaf const & other) : value_(other.value_) {}

  void operator = (TupleLeaf const & other) {
    value_ = other.value_;
  }

  T const & Get() const {
    return value_;
  }

  T & Get() {
    return value_;
  }

 private:
  T value_;
};

template <int Index>
class TupleLeaf<Index, embb::base::internal::Nil> {
 public:
  typedef embb::base::internal::Nil Type;

  enum { index = Index };

  enum { count = 0 };

  embb::base::internal::Nil const & Get() const {
    static embb::base::internal::Nil vv;
    return vv;
  }

  embb::base::internal::Nil & Get() {
    static embb::base::internal::Nil vv;
    return vv;
  }
};

template <
  typename T1 = embb::base::internal::Nil,
  typename T2 = embb::base::internal::Nil,
  typename T3 = embb::base::internal::Nil,
  typename T4 = embb::base::internal::Nil,
  typename T5 = embb::base::internal::Nil>
class Tuple :
  public TupleLeaf<0, T1>,
  public TupleLeaf<1, T2>,
  public TupleLeaf<2, T3>,
  public TupleLeaf<3, T4>,
  public TupleLeaf<4, T5> {
 public:
  typedef typename MakeTypeList<T1, T2, T3, T4, T5>::Result Types;
  typedef typename MakeTypeList<
    TupleLeaf<0, T1>,
    TupleLeaf<1, T2>,
    TupleLeaf<2, T3>,
    TupleLeaf<3, T4>,
    TupleLeaf<4, T5> >::Result LeafTypes;

  size_t Size() const {
    return
      TupleLeaf<0, T1>::count +
      TupleLeaf<1, T2>::count +
      TupleLeaf<2, T3>::count +
      TupleLeaf<3, T4>::count +
      TupleLeaf<4, T5>::count;
  }

  template <int Index>
  typename TypeAt< Types, Index >::Result & Get() {
    typename TypeAt< LeafTypes, Index >::Result * leaf =
      static_cast<typename TypeAt< LeafTypes, Index >::Result *>(this);
    return leaf->Get();
  }

  template <int Index>
  typename TypeAt< Types, Index >::Result const & Get() const {
    typename TypeAt< LeafTypes, Index >::Result const * leaf =
      static_cast<typename TypeAt< LeafTypes, Index >::Result const *>(this);
    return leaf->Get();
  }
};

} // namespace internal
} // namespace dataflow
} // namespace embb

#endif // EMBB_DATAFLOW_INTERNAL_TUPLE_H_
