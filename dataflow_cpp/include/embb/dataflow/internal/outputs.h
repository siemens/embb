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

#ifndef EMBB_DATAFLOW_INTERNAL_OUTPUTS_H_
#define EMBB_DATAFLOW_INTERNAL_OUTPUTS_H_

#include <embb/dataflow/internal/tuple.h>
#include <embb/dataflow/internal/out.h>

namespace embb {
namespace dataflow {
namespace internal {

template <
  int,
  typename = embb::base::internal::Nil,
  typename = embb::base::internal::Nil,
  typename = embb::base::internal::Nil,
  typename = embb::base::internal::Nil,
  typename = embb::base::internal::Nil >
class Outputs;

template <int Slices>
class Outputs<Slices, embb::base::internal::Nil, embb::base::internal::Nil,
  embb::base::internal::Nil, embb::base::internal::Nil,
  embb::base::internal::Nil>
  : public Tuple<embb::base::internal::Nil, embb::base::internal::Nil,
    embb::base::internal::Nil, embb::base::internal::Nil,
    embb::base::internal::Nil> {
 public:
};

template <int Slices, typename T1>
class Outputs<Slices, T1, embb::base::internal::Nil, embb::base::internal::Nil,
  embb::base::internal::Nil, embb::base::internal::Nil>
  : public Tuple<Out<T1, Slices>, embb::base::internal::Nil,
    embb::base::internal::Nil, embb::base::internal::Nil,
    embb::base::internal::Nil> {
 public:
};

template <int Slices, typename T1, typename T2>
class Outputs<Slices, T1, T2, embb::base::internal::Nil,
  embb::base::internal::Nil, embb::base::internal::Nil>
  : public Tuple<Out<T1, Slices>, Out<T2, Slices>, embb::base::internal::Nil,
    embb::base::internal::Nil, embb::base::internal::Nil> {
 public:
};

template <int Slices, typename T1, typename T2, typename T3>
class Outputs<Slices, T1, T2, T3, embb::base::internal::Nil,
  embb::base::internal::Nil>
  : public Tuple<Out<T1, Slices>, Out<T2, Slices>, Out<T3, Slices>,
    embb::base::internal::Nil, embb::base::internal::Nil> {
 public:
};

template <int Slices, typename T1, typename T2, typename T3, typename T4>
class Outputs<Slices, T1, T2, T3, T4, embb::base::internal::Nil>
  : public Tuple<Out<T1, Slices>, Out<T2, Slices>, Out<T3, Slices>,
      Out<T4, Slices>, embb::base::internal::Nil>{
 public:
};

template <int Slices, typename T1, typename T2, typename T3, typename T4,
  typename T5>
class Outputs
  : public Tuple<Out<T1, Slices>, Out<T2, Slices>, Out<T3, Slices>,
      Out<T4, Slices>, Out<T5, Slices> > {
 public:
};

} // namespace internal
} // namespace dataflow
} // namespace embb

#endif // EMBB_DATAFLOW_INTERNAL_OUTPUTS_H_
