/*
 * Copyright (c) 2014, Siemens AG. All rights reserved.
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

#ifndef EMBB_BASE_CPP_PERF_INTERNAL_TIMER_TIMESTAMP_PAPI_INL_H_
#define EMBB_BASE_CPP_PERF_INTERNAL_TIMER_TIMESTAMP_PAPI_INL_H_

#if defined(EMBB_BASE_CPP_PERF_TIMER_PAPI)

#include <embb/base/perf/internal/timestamp_papi.h>

namespace embb {
namespace base {
namespace perf {
namespace internal {
namespace timer {

template<TimeMeasure::MeasureMode TTimer>
int TimestampPAPI<TTimer>::timer_mode = 0; 

} // namespace timer
} // namespace internal
} // namespace perf
} // namespace base
} // namespace embb

#endif // EMBB_BASE_CPP_PERF_TIMER_PAPI

#endif // EMBB_BASE_CPP_PERF_INTERNAL_TIMER_TIMESTAMP_PAPI_INL_H_

