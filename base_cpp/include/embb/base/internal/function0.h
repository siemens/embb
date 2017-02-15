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

#ifndef EMBB_BASE_INTERNAL_FUNCTION0_H_
#define EMBB_BASE_INTERNAL_FUNCTION0_H_

#include <new>
#include <cstddef>

#include <embb/base/internal/nil.h>
#include <embb/base/memory_allocation.h>
#include <embb/base/atomic.h>
#include <embb/base/internal/functionT.h>

namespace embb {
namespace base {

namespace internal {

template <typename R>
class Function0 {
 public:
  virtual ~Function0() {}
  virtual R operator () () = 0;
  virtual void CopyTo(void* dst) = 0;
};

template <typename R>
class FunctionPointer0
  : public Function0<R> {
 public:
  typedef R(*FuncPtrType)();
  explicit FunctionPointer0(FuncPtrType func) : function_(func) {}
  virtual R operator () () {
    return function_();
  }
  virtual void CopyTo(void* dst) {
    new(dst)FunctionPointer0(function_);
  }

 private:
  FuncPtrType function_;
};

template <>
class FunctionPointer0<void>
  : public Function0<void> {
 public:
  typedef void(*FuncPtrType)();
  explicit FunctionPointer0(FuncPtrType func) : function_(func) {}
  virtual void operator () () {
    function_();
  }
  virtual void CopyTo(void* dst) {
    new(dst)FunctionPointer0(function_);
  }

 private:
  FuncPtrType function_;
};

template <class C, typename R>
class MemberFunctionPointer0
  : public Function0<R> {
 public:
  typedef R(C::*MemFuncPtrType)();
  typedef C & ClassRefType;
  MemberFunctionPointer0(ClassRefType obj, MemFuncPtrType func)
  : object_(obj), function_(func) {}
  explicit MemberFunctionPointer0(ClassRefType obj)
  : object_(obj), function_(&C::operator()) {}
  void operator = (MemberFunctionPointer0 const & memfunc) {
    object_ = memfunc.object_;
    function_ = memfunc.function_;
  }
  virtual R operator () () {
    return (object_.*function_)();
  }
  virtual void CopyTo(void* dst) {
    new(dst)MemberFunctionPointer0(object_, function_);
  }

 private:
  ClassRefType object_;
  MemFuncPtrType function_;
};

template <class C>
class MemberFunctionPointer0<C, void>
  : public Function0<void> {
 public:
  typedef void(C::*MemFuncPtrType)();
  typedef C & ClassRefType;
  MemberFunctionPointer0(ClassRefType obj, MemFuncPtrType func)
  : object_(obj), function_(func) {}
  explicit MemberFunctionPointer0(ClassRefType obj)
  : object_(obj), function_(&C::operator()) {}
  void operator = (MemberFunctionPointer0 const & memfunc) {
    object_ = memfunc.object_;
    function_ = memfunc.function_;
  }
  virtual void operator () () {
    (object_.*function_)();
  }
  virtual void CopyTo(void* dst) {
    new(dst)MemberFunctionPointer0(object_, function_);
  }

 private:
  ClassRefType object_;
  MemFuncPtrType function_;
};

template <class C, typename R>
class FunctorWrapper0
  : public Function0<R> {
 public:
  FunctorWrapper0() : object_(NULL), ref_count_(NULL) {}
  explicit FunctorWrapper0(C const & obj) {
    object_ = Allocation::New<C>(obj);
    ref_count_ = Allocation::New<Atomic<int> >(1);
  }
  explicit FunctorWrapper0(FunctorWrapper0 const & other) {
    object_ = other.object_;
    ref_count_ = other.ref_count_;
    ++*ref_count_;
  }
  virtual ~FunctorWrapper0() {
    if (0 == --*ref_count_) {
      Allocation::Delete(ref_count_);
      Allocation::Delete(object_);
    }
  }
  virtual R operator () () {
    return (*object_)();
  }
  virtual void CopyTo(void* dst) {
    new(dst)FunctorWrapper0(*this);
  }

 private:
  C * object_;
  Atomic<int> * ref_count_;
};

template <class C>
class FunctorWrapper0<C, void>
  : public Function0<void> {
 public:
  FunctorWrapper0() : object_(NULL), ref_count_(NULL) {}
  explicit FunctorWrapper0(C const & obj) {
    object_ = Allocation::New<C>(obj);
    ref_count_ = Allocation::New<Atomic<int> >(1);
  }
  explicit FunctorWrapper0(FunctorWrapper0 const & other) {
    object_ = other.object_;
    ref_count_ = other.ref_count_;
    ++*ref_count_;
  }
  virtual ~FunctorWrapper0() {
    if (0 == --*ref_count_) {
      Allocation::Delete(ref_count_);
      Allocation::Delete(object_);
    }
  }
  virtual void operator () () {
    (*object_)();
  }
  virtual void CopyTo(void* dst) {
    new(dst)FunctorWrapper0(*this);
  }

 private:
  C * object_;
  Atomic<int> * ref_count_;
};

} // namespace internal


using embb::base::internal::Nil;

template <typename R>
class Function<R, Nil, Nil, Nil, Nil, Nil> {
 public:
  typedef internal::Function0<R> * FuncPtrType;
  Function() : function_(NULL) {}
  template <class C>
  explicit Function(C const & obj) {
    function_ = new(storage_)
      internal::FunctorWrapper0<C, R>(obj);
  }
  Function(Function const & func) {
    func.function_->CopyTo(&storage_[0]);
    function_ = reinterpret_cast<FuncPtrType>(&storage_[0]);
  }
  ~Function() {
    Free();
  }
  void operator = (R(*func)()) {
    Free();
    function_ = new(storage_)
      internal::FunctionPointer0<R>(func);
  }
  void operator = (Function & func) {
    Free();
    func.function_->CopyTo(&storage_[0]);
    function_ = reinterpret_cast<FuncPtrType>(&storage_[0]);
  }
  template <class C>
  void operator = (C const & obj) {
    Free();
    function_ = new(storage_)
      internal::FunctorWrapper0<C, R>(obj);
  }
  explicit Function(R(*func)()) {
    function_ = new(storage_)
      internal::FunctionPointer0<R>(func);
  }
  template <class C>
  Function(C & obj, R(C::*func)()) {
    function_ = new(storage_)
      internal::MemberFunctionPointer0<C, R>(obj, func);
  }
  R operator () () {
    return (*function_)();
  }

 private:
  char storage_[sizeof(
    internal::MemberFunctionPointer0<Nil, R>)];
  FuncPtrType function_;
  void Free() {
    if (NULL != function_) {
      function_->~Function0();
      function_ = NULL;
    }
  }
};

// wrap member function
template <class C, typename R>
Function<R> MakeFunction(C & obj,
  R(C::*func)()) {
  return Function<R>(obj, func);
}

// wrap function pointer
template <typename R>
Function<R> MakeFunction(
  R(*func)()) {
  return Function<R>(func);
}

// bind to Function0
template <class C, typename R>
Function<R> Bind(
  C & obj,
  R(C::*func)()) {
  return Function<R>(obj, func);
}

template <typename R>
Function<R> Bind(
  R(*func)()) {
  return Function<R>(func);
}

} // namespace base
} // namespace embb

#endif // EMBB_BASE_INTERNAL_FUNCTION0_H_
