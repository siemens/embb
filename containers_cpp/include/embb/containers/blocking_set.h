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

#ifndef EMBB_CONTAINERS_BLOCKING_SET_H_
#define EMBB_CONTAINERS_BLOCKING_SET_H_


#include <embb/base/base.h>
#include <set>


namespace embb {
namespace containers {

/*
* Wrapper for the standard library set.
*
* \tparam T Element type.
*/
template < typename Type >
class BlockingSet {
  typedef embb::base::Mutex Mutex;
  typedef embb::base::LockGuard<Mutex> LockGuard;
 private:
  std::set<Type> internalSet;

  Mutex mutex;

 public:
  BlockingSet();

  bool Insert(const Type& element);

  bool Erase(const Type& element);

  bool Contains(const Type& element);
  
};

}
}

#include <embb/containers/internal/blocking_set-inl.h>

#endif  // EMBB_CONTAINERS_BLOCKING_CONTAINER_