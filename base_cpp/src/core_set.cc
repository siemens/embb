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

#include <embb/base/core_set.h>
#include <embb/base/internal/config.h>

#include <embb/base/c/core_set.h>

namespace embb {
namespace base {

unsigned int CoreSet::CountAvailable() {
  return embb_core_count_available();
}

CoreSet::CoreSet() : rep_() {
  embb_core_set_init(&rep_, 0);
}

CoreSet::CoreSet(bool value) : rep_() {
  int value_converted = value == true ? 1 : 0;
  embb_core_set_init(&rep_, value_converted);
}

CoreSet::CoreSet(const CoreSet& to_copy) : rep_() {
  embb_core_set_init(&rep_, 0);
  embb_core_set_union(&rep_, &(to_copy.rep_));
}

CoreSet& CoreSet::operator=(const CoreSet& to_assign) {
  embb_core_set_init(&rep_, 0);
  embb_core_set_union(&rep_, &(to_assign.rep_));
  return *this;
}

void CoreSet::Reset(bool value) {
  int value_converted = value ? 1 : 0;
  embb_core_set_init(&rep_, value_converted);
}

void CoreSet::Add(unsigned int core) {
  embb_core_set_add(&rep_, core);
}

void CoreSet::Remove(unsigned int core) {
  embb_core_set_remove(&rep_, core);
}

bool CoreSet::IsContained(unsigned int core) const {
  return embb_core_set_contains(&rep_, core) != 0;
}

unsigned int CoreSet::Count() const {
  return embb_core_set_count(&rep_);
}

CoreSet CoreSet::operator&(const CoreSet& rhs) const {
  CoreSet result(*this);
  embb_core_set_intersection(&(result.rep_), &(rhs.rep_));
  return result;
}

CoreSet CoreSet::operator|(const CoreSet& rhs) const {
  CoreSet result(*this);
  embb_core_set_union(&(result.rep_), &(rhs.rep_));
  return result;
}

CoreSet& CoreSet::operator&=(const CoreSet& rhs) {
  embb_core_set_intersection(&rep_, &(rhs.rep_));
  return *this;
}

CoreSet& CoreSet::operator|=(const CoreSet& rhs) {
  embb_core_set_union(&rep_, &(rhs.rep_));
  return *this;
}

} // namespace base
} // namespace embb
