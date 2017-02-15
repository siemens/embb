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

#ifndef EMBB_BASE_INTERNAL_FUNCTION1_H_
#define EMBB_BASE_INTERNAL_FUNCTION1_H_

#include <new>
#include <cstddef>

#include <embb/base/internal/nil.h>
#include <embb/base/memory_allocation.h>
#include <embb/base/atomic.h>
#include <embb/base/internal/functionT.h>
#include <embb/base/internal/function0.h>

namespace embb {
namespace base {

namespace internal {

template <typename R,
  typename T1>
class Function1 {
 public:
  virtual ~Function1() {}
  virtual R operator () (T1) = 0;
  virtual void CopyTo(void* dst) = 0;
};

template <typename R,
  typename T1>
class FunctionPointer1
  : public Function1<R, T1> {
 public:
  typedef R(*FuncPtrType)(T1);
  explicit FunctionPointer1(FuncPtrType func) : function_(func) {}
  virtual R operator () (T1 p1) {
    return function_(p1);
  }
  virtual void CopyTo(void* dst) {
    new(dst)FunctionPointer1(function_);
  }

 private:
  FuncPtrType function_;
};

template <typename T1>
class FunctionPointer1<void, T1>
  : public Function1<void, T1> {
 public:
  typedef void(*FuncPtrType)(T1);
  explicit FunctionPointer1(FuncPtrType func) : function_(func) {}
  virtual void operator () (T1 p1) {
    function_(p1);
  }
  virtual void CopyTo(void* dst) {
    new(dst)FunctionPointer1(function_);
  }

 private:
  FuncPtrType function_;
};

template <class C, typename R,
  typename T1>
class MemberFunctionPointer1
  : public Function1<R, T1> {
 public:
  typedef R(C::*MemFuncPtrType)(T1);
  typedef C & ClassRefType;
  MemberFunctionPointer1(ClassRefType obj, MemFuncPtrType func)
  : object_(obj), function_(func) {}
  explicit MemberFunctionPointer1(ClassRefType obj)
  : object_(obj), function_(&C::operator()) {}
  void operator = (MemberFunctionPointer1 const & memfunc) {
    object_ = memfunc.object_;
    function_ = memfunc.function_;
  }
  virtual R operator () (T1 p1) {
    return (object_.*function_)(p1);
  }
  virtual void CopyTo(void* dst) {
    new(dst)MemberFunctionPointer1(object_, function_);
  }

 private:
  ClassRefType object_;
  MemFuncPtrType function_;
};

template <class C,
  typename T1>
class MemberFunctionPointer1<C, void, T1>
  : public Function1<void, T1> {
 public:
  typedef void(C::*MemFuncPtrType)(T1);
  typedef C & ClassRefType;
  MemberFunctionPointer1(ClassRefType obj, MemFuncPtrType func)
  : object_(obj), function_(func) {}
  explicit MemberFunctionPointer1(ClassRefType obj)
  : object_(obj), function_(&C::operator()) {}
  void operator = (MemberFunctionPointer1 const & memfunc) {
    object_ = memfunc.object_;
    function_ = memfunc.function_;
  }
  virtual void operator () (T1 p1) {
    (object_.*function_)(p1);
  }
  virtual void CopyTo(void* dst) {
    new(dst)MemberFunctionPointer1(object_, function_);
  }

 private:
  ClassRefType object_;
  MemFuncPtrType function_;
};

template <class C, typename R,
  typename T1>
class FunctorWrapper1
  : public Function1<R, T1> {
 public:
  FunctorWrapper1() : object_(NULL), ref_count_(NULL) {}
  explicit FunctorWrapper1(C const & obj) {
    object_ = Allocation::New<C>(obj);
    ref_count_ = Allocation::New<Atomic<int> >(1);
  }
  explicit FunctorWrapper1(FunctorWrapper1 const & other) {
    object_ = other.object_;
    ref_count_ = other.ref_count_;
    ++*ref_count_;
  }
  virtual ~FunctorWrapper1() {
    if (0 == --*ref_count_) {
      Allocation::Delete(ref_count_);
      Allocation::Delete(object_);
    }
  }
  virtual R operator () (T1 p1) {
    return (*object_)(p1);
  }
  virtual void CopyTo(void* dst) {
    new(dst)FunctorWrapper1(*this);
  }

 private:
  C * object_;
  Atomic<int> * ref_count_;
};

template <class C,
  typename T1>
class FunctorWrapper1<C, void, T1>
  : public Function1<void, T1> {
 public:
  FunctorWrapper1() : object_(NULL), ref_count_(NULL) {}
  explicit FunctorWrapper1(C const & obj) {
    object_ = Allocation::New<C>(obj);
    ref_count_ = Allocation::New<Atomic<int> >(1);
  }
  explicit FunctorWrapper1(FunctorWrapper1 const & other) {
    object_ = other.object_;
    ref_count_ = other.ref_count_;
    ++*ref_count_;
  }
  virtual ~FunctorWrapper1() {
    if (0 == --*ref_count_) {
      Allocation::Delete(ref_count_);
      Allocation::Delete(object_);
    }
  }
  virtual void operator () (T1 p1) {
    (*object_)(p1);
  }
  virtual void CopyTo(void* dst) {
    new(dst)FunctorWrapper1(*this);
  }

 private:
  C * object_;
  Atomic<int> * ref_count_;
};

// bind to function0
template <typename R,
  typename T1>
class Bound1Functor0 {
 public:
  Bound1Functor0(Function<R, T1> func,
    T1 p1) : function_(func)
      , p1_(p1) {}
  Bound1Functor0(Bound1Functor0 const & func) : function_(func.function_)
    , p1_(func.p1_) {}
  R operator() () {
    return function_(p1_);
  }

 private:
  Function<R, T1> function_;
  T1 p1_;
};

} // namespace internal


using embb::base::internal::Nil;

template <typename R,
  typename T1>
class Function<R, T1, Nil, Nil, Nil, Nil> {
 public:
  typedef internal::Function1<R, T1> * FuncPtrType;
  Function() : function_(NULL) {}
  template <class C>
  explicit Function(C const & obj) {
    function_ = new(storage_)
      internal::FunctorWrapper1<C, R, T1>(obj);
  }
  Function(Function const & func) {
    func.function_->CopyTo(&storage_[0]);
    function_ = reinterpret_cast<FuncPtrType>(&storage_[0]);
  }
  ~Function() {
    Free();
  }
  void operator = (R(*func)(T1)) {
    Free();
    function_ = new(storage_)
      internal::FunctionPointer1<R, T1>(func);
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
      internal::FunctorWrapper1<C, R, T1>(obj);
  }
  explicit Function(R(*func)(T1)) {
    function_ = new(storage_)
      internal::FunctionPointer1<R, T1>(func);
  }
  template <class C>
  Function(C & obj, R(C::*func)(T1)) {
    function_ = new(storage_)
      internal::MemberFunctionPointer1<C, R, T1>(obj, func);
  }
  R operator () (T1 p1) {
    return (*function_)(p1);
  }

 private:
  char storage_[sizeof(
    internal::MemberFunctionPointer1<Nil, R, T1>)];
  FuncPtrType function_;
  void Free() {
    if (NULL != function_) {
      function_->~Function1();
      function_ = NULL;
    }
  }
};

// wrap member function
template <class C, typename R,
  typename T1>
Function<R, T1> MakeFunction(C & obj,
  R(C::*func)(T1)) {
  return Function<R, T1>(obj, func);
}

// wrap function pointer
template <typename R,
  typename T1>
Function<R, T1> MakeFunction(
  R(*func)(T1)) {
  return Function<R, T1>(func);
}

// bind to function0
template <typename R,
  typename T1>
Function<R> Bind(Function<R, T1> func,
  T1 p1) {
  return Function<R>(
  internal::Bound1Functor0<R, T1>(
    func, p1));
}

template <typename R,
  typename T1>
Function<R> Bind(R(*func)(T1),
  T1 p1) {
  return Bind(Function<R, T1>(func), p1);
}

template <class C, typename R,
  typename T1>
Function<R> Bind(C & obj, R(C::*func)(T1),
  T1 p1) {
  return Bind(Function<R, T1>(obj, func), p1);
}

// bind to Function1
template <class C, typename R,
  typename T1>
Function<R, T1> Bind(
  C & obj,
  R(C::*func)(T1),
  Placeholder::Arg_1) {
  return Function<R, T1>(obj, func);
}

template <typename R,
  typename T1>
Function<R, T1> Bind(
  R(*func)(T1),
  Placeholder::Arg_1) {
  return Function<R, T1>(func);
}

} // namespace base
} // namespace embb

#endif // EMBB_BASE_INTERNAL_FUNCTION1_H_
