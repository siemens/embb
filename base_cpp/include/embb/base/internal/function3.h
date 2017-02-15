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

#ifndef EMBB_BASE_INTERNAL_FUNCTION3_H_
#define EMBB_BASE_INTERNAL_FUNCTION3_H_

#include <new>
#include <cstddef>

#include <embb/base/internal/nil.h>
#include <embb/base/memory_allocation.h>
#include <embb/base/atomic.h>
#include <embb/base/internal/functionT.h>
#include <embb/base/internal/function0.h>
#include <embb/base/internal/function1.h>

namespace embb {
namespace base {

namespace internal {

template <typename R,
  typename T1, typename T2, typename T3>
class Function3 {
 public:
  virtual ~Function3() {}
  virtual R operator () (T1, T2, T3) = 0;
  virtual void CopyTo(void* dst) = 0;
};

template <typename R,
  typename T1, typename T2, typename T3>
class FunctionPointer3
  : public Function3<R, T1, T2, T3> {
 public:
  typedef R(*FuncPtrType)(T1, T2, T3);
  explicit FunctionPointer3(FuncPtrType func) : function_(func) {}
  virtual R operator () (T1 p1, T2 p2, T3 p3) {
    return function_(p1, p2, p3);
  }
  virtual void CopyTo(void* dst) {
    new(dst)FunctionPointer3(function_);
  }

 private:
  FuncPtrType function_;
};

template <typename T1, typename T2, typename T3>
class FunctionPointer3<void, T1, T2, T3>
  : public Function3<void, T1, T2, T3> {
 public:
  typedef void(*FuncPtrType)(T1, T2, T3);
  explicit FunctionPointer3(FuncPtrType func) : function_(func) {}
  virtual void operator () (T1 p1, T2 p2, T3 p3) {
    function_(p1, p2, p3);
  }
  virtual void CopyTo(void* dst) {
    new(dst)FunctionPointer3(function_);
  }

 private:
  FuncPtrType function_;
};

template <class C, typename R,
  typename T1, typename T2, typename T3>
class MemberFunctionPointer3
  : public Function3<R, T1, T2, T3> {
 public:
  typedef R(C::*MemFuncPtrType)(T1, T2, T3);
  typedef C & ClassRefType;
  MemberFunctionPointer3(ClassRefType obj, MemFuncPtrType func)
  : object_(obj), function_(func) {}
  explicit MemberFunctionPointer3(ClassRefType obj)
  : object_(obj), function_(&C::operator()) {}
  void operator = (MemberFunctionPointer3 const & memfunc) {
    object_ = memfunc.object_;
    function_ = memfunc.function_;
  }
  virtual R operator () (T1 p1, T2 p2, T3 p3) {
    return (object_.*function_)(p1, p2, p3);
  }
  virtual void CopyTo(void* dst) {
    new(dst)MemberFunctionPointer3(object_, function_);
  }

 private:
  ClassRefType object_;
  MemFuncPtrType function_;
};

template <class C,
  typename T1, typename T2, typename T3>
class MemberFunctionPointer3<C, void, T1, T2, T3>
  : public Function3<void, T1, T2, T3> {
 public:
  typedef void(C::*MemFuncPtrType)(T1, T2, T3);
  typedef C & ClassRefType;
  MemberFunctionPointer3(ClassRefType obj, MemFuncPtrType func)
  : object_(obj), function_(func) {}
  explicit MemberFunctionPointer3(ClassRefType obj)
  : object_(obj), function_(&C::operator()) {}
  void operator = (MemberFunctionPointer3 const & memfunc) {
    object_ = memfunc.object_;
    function_ = memfunc.function_;
  }
  virtual void operator () (T1 p1, T2 p2, T3 p3) {
    (object_.*function_)(p1, p2, p3);
  }
  virtual void CopyTo(void* dst) {
    new(dst)MemberFunctionPointer3(object_, function_);
  }

 private:
  ClassRefType object_;
  MemFuncPtrType function_;
};

template <class C, typename R,
  typename T1, typename T2, typename T3>
class FunctorWrapper3
  : public Function3<R, T1, T2, T3> {
 public:
  FunctorWrapper3() : object_(NULL), ref_count_(NULL) {}
  explicit FunctorWrapper3(C const & obj) {
    object_ = Allocation::New<C>(obj);
    ref_count_ = Allocation::New<Atomic<int> >(1);
  }
  explicit FunctorWrapper3(FunctorWrapper3 const & other) {
    object_ = other.object_;
    ref_count_ = other.ref_count_;
    ++*ref_count_;
  }
  virtual ~FunctorWrapper3() {
    if (0 == --*ref_count_) {
      Allocation::Delete(ref_count_);
      Allocation::Delete(object_);
    }
  }
  virtual R operator () (T1 p1, T2 p2, T3 p3) {
    return (*object_)(p1, p2, p3);
  }
  virtual void CopyTo(void* dst) {
    new(dst)FunctorWrapper3(*this);
  }

 private:
  C * object_;
  Atomic<int> * ref_count_;
};

template <class C,
  typename T1, typename T2, typename T3>
class FunctorWrapper3<C, void, T1, T2, T3>
  : public Function3<void, T1, T2, T3> {
 public:
  FunctorWrapper3() : object_(NULL), ref_count_(NULL) {}
  explicit FunctorWrapper3(C const & obj) {
    object_ = Allocation::New<C>(obj);
    ref_count_ = Allocation::New<Atomic<int> >(1);
  }
  explicit FunctorWrapper3(FunctorWrapper3 const & other) {
    object_ = other.object_;
    ref_count_ = other.ref_count_;
    ++*ref_count_;
  }
  virtual ~FunctorWrapper3() {
    if (0 == --*ref_count_) {
      Allocation::Delete(ref_count_);
      Allocation::Delete(object_);
    }
  }
  virtual void operator () (T1 p1, T2 p2, T3 p3) {
    (*object_)(p1, p2, p3);
  }
  virtual void CopyTo(void* dst) {
    new(dst)FunctorWrapper3(*this);
  }

 private:
  C * object_;
  Atomic<int> * ref_count_;
};

// bind to function0
template <typename R,
  typename T1, typename T2, typename T3>
class Bound3Functor0 {
 public:
  Bound3Functor0(Function<R, T1, T2, T3> func,
    T1 p1, T2 p2, T3 p3) : function_(func)
      , p1_(p1), p2_(p2), p3_(p3) {}
  Bound3Functor0(Bound3Functor0 const & func) : function_(func.function_)
    , p1_(func.p1_), p2_(func.p2_), p3_(func.p3_) {}
  R operator() () {
    return function_(p1_, p2_, p3_);
  }

 private:
  Function<R, T1, T2, T3> function_;
  T1 p1_;
  T2 p2_;
  T3 p3_;
};

// bind to function1
template <typename R,
  typename T1, typename T2, typename T3>
class Bound3Functor1_Arg1 {
 public:
  Bound3Functor1_Arg1(Function<R, T1, T2, T3> func
    , T2 p2, T3 p3) : function_(func)
      , p2_(p2), p3_(p3) {}
  Bound3Functor1_Arg1(Bound3Functor1_Arg1 const & func)
    : function_(func.function_)
    , p2_(func.p2_), p3_(func.p3_) {}
  R operator() (T1 p1) {
    return function_(p1, p2_, p3_);
  }

 private:
  Function<R, T1, T2, T3> function_;
  T2 p2_;
  T3 p3_;
};

template <typename R,
  typename T1, typename T2, typename T3>
class Bound3Functor1_Arg2 {
 public:
  Bound3Functor1_Arg2(Function<R, T1, T2, T3> func
    , T1 p1, T3 p3) : function_(func)
      , p1_(p1), p3_(p3) {}
  Bound3Functor1_Arg2(Bound3Functor1_Arg2 const & func)
    : function_(func.function_)
    , p1_(func.p1_), p3_(func.p3_) {}
  R operator() (T2 p2) {
    return function_(p1_, p2, p3_);
  }

 private:
  Function<R, T1, T2, T3> function_;
  T1 p1_;
  T3 p3_;
};

template <typename R,
  typename T1, typename T2, typename T3>
class Bound3Functor1_Arg3 {
 public:
  Bound3Functor1_Arg3(Function<R, T1, T2, T3> func
    , T1 p1, T2 p2) : function_(func)
      , p1_(p1), p2_(p2) {}
  Bound3Functor1_Arg3(Bound3Functor1_Arg3 const & func)
    : function_(func.function_)
    , p1_(func.p1_), p2_(func.p2_) {}
  R operator() (T3 p3) {
    return function_(p1_, p2_, p3);
  }

 private:
  Function<R, T1, T2, T3> function_;
  T1 p1_;
  T2 p2_;
};

} // namespace internal


using embb::base::internal::Nil;

template <typename R,
  typename T1, typename T2, typename T3>
class Function<R, T1, T2, T3, Nil, Nil> {
 public:
  typedef internal::Function3<R, T1, T2, T3> * FuncPtrType;
  Function() : function_(NULL) {}
  template <class C>
  explicit Function(C const & obj) {
    function_ = new(storage_)
      internal::FunctorWrapper3<C, R, T1, T2, T3>(obj);
  }
  Function(Function const & func) {
    func.function_->CopyTo(&storage_[0]);
    function_ = reinterpret_cast<FuncPtrType>(&storage_[0]);
  }
  ~Function() {
    Free();
  }
  void operator = (R(*func)(T1, T2, T3)) {
    Free();
    function_ = new(storage_)
      internal::FunctionPointer3<R, T1, T2, T3>(func);
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
      internal::FunctorWrapper3<C, R, T1, T2, T3>(obj);
  }
  explicit Function(R(*func)(T1, T2, T3)) {
    function_ = new(storage_)
      internal::FunctionPointer3<R, T1, T2, T3>(func);
  }
  template <class C>
  Function(C & obj, R(C::*func)(T1, T2, T3)) {
    function_ = new(storage_)
      internal::MemberFunctionPointer3<C, R, T1, T2, T3>(obj, func);
  }
  R operator () (T1 p1, T2 p2, T3 p3) {
    return (*function_)(p1, p2, p3);
  }

 private:
  char storage_[sizeof(
    internal::MemberFunctionPointer3<Nil, R, T1, T2, T3>)];
  FuncPtrType function_;
  void Free() {
    if (NULL != function_) {
      function_->~Function3();
      function_ = NULL;
    }
  }
};

// wrap member function
template <class C, typename R,
  typename T1, typename T2, typename T3>
Function<R, T1, T2, T3> MakeFunction(C & obj,
  R(C::*func)(T1, T2, T3)) {
  return Function<R, T1, T2, T3>(obj, func);
}

// wrap function pointer
template <typename R,
  typename T1, typename T2, typename T3>
Function<R, T1, T2, T3> MakeFunction(
  R(*func)(T1, T2, T3)) {
  return Function<R, T1, T2, T3>(func);
}

// bind to function0
template <typename R,
  typename T1, typename T2, typename T3>
Function<R> Bind(Function<R, T1, T2, T3> func,
  T1 p1, T2 p2, T3 p3) {
  return Function<R>(
  internal::Bound3Functor0<R, T1, T2, T3>(
    func, p1, p2, p3));
}

template <typename R,
  typename T1, typename T2, typename T3>
Function<R> Bind(R(*func)(T1, T2, T3),
  T1 p1, T2 p2, T3 p3) {
  return Bind(Function<R, T1, T2, T3>(func), p1, p2, p3);
}

template <class C, typename R,
  typename T1, typename T2, typename T3>
Function<R> Bind(C & obj, R(C::*func)(T1, T2, T3),
  T1 p1, T2 p2, T3 p3) {
  return Bind(Function<R, T1, T2, T3>(obj, func), p1, p2, p3);
}

// bind to function1
template <typename R,
  typename T1, typename T2, typename T3>
Function<R, T1> Bind(Function<R, T1, T2, T3> func
  , Placeholder::Arg_1, T2 p2, T3 p3) {
  return Function<R, T1>(
  internal::Bound3Functor1_Arg1<R, T1, T2, T3>(func
    , p2, p3));
}

template <typename R,
  typename T1, typename T2, typename T3>
Function<R, T1> Bind(R(*func)(T1, T2, T3)
  , Placeholder::Arg_1 p1, T2 p2, T3 p3) {
  return Bind(Function<R, T1, T2, T3>(func), p1, p2, p3);
}

template <class C, typename R,
  typename T1, typename T2, typename T3>
Function<R, T1> Bind(C & obj, R(C::*func)(T1, T2, T3)
  , Placeholder::Arg_1 p1, T2 p2, T3 p3) {
  return Bind(Function<R, T1, T2, T3>(obj, func), p1, p2, p3);
}

template <typename R,
  typename T1, typename T2, typename T3>
Function<R, T2> Bind(Function<R, T1, T2, T3> func
  , T1 p1, Placeholder::Arg_1, T3 p3) {
  return Function<R, T2>(
  internal::Bound3Functor1_Arg2<R, T1, T2, T3>(func
    , p1, p3));
}

template <typename R,
  typename T1, typename T2, typename T3>
Function<R, T2> Bind(R(*func)(T1, T2, T3)
  , T1 p1, Placeholder::Arg_1 p2, T3 p3) {
  return Bind(Function<R, T1, T2, T3>(func), p1, p2, p3);
}

template <class C, typename R,
  typename T1, typename T2, typename T3>
Function<R, T2> Bind(C & obj, R(C::*func)(T1, T2, T3)
  , T1 p1, Placeholder::Arg_1 p2, T3 p3) {
  return Bind(Function<R, T1, T2, T3>(obj, func), p1, p2, p3);
}

template <typename R,
  typename T1, typename T2, typename T3>
Function<R, T3> Bind(Function<R, T1, T2, T3> func
  , T1 p1, T2 p2, Placeholder::Arg_1) {
  return Function<R, T3>(
  internal::Bound3Functor1_Arg3<R, T1, T2, T3>(func
    , p1, p2));
}

template <typename R,
  typename T1, typename T2, typename T3>
Function<R, T3> Bind(R(*func)(T1, T2, T3)
  , T1 p1, T2 p2, Placeholder::Arg_1 p3) {
  return Bind(Function<R, T1, T2, T3>(func), p1, p2, p3);
}

template <class C, typename R,
  typename T1, typename T2, typename T3>
Function<R, T3> Bind(C & obj, R(C::*func)(T1, T2, T3)
  , T1 p1, T2 p2, Placeholder::Arg_1 p3) {
  return Bind(Function<R, T1, T2, T3>(obj, func), p1, p2, p3);
}

// bind to Function3
template <class C, typename R,
  typename T1, typename T2, typename T3>
Function<R, T1, T2, T3> Bind(
  C & obj,
  R(C::*func)(T1, T2, T3),
  Placeholder::Arg_1,
  Placeholder::Arg_2,
  Placeholder::Arg_3) {
  return Function<R, T1, T2, T3>(obj, func);
}

template <typename R,
  typename T1, typename T2, typename T3>
Function<R, T1, T2, T3> Bind(
  R(*func)(T1, T2, T3),
  Placeholder::Arg_1,
  Placeholder::Arg_2,
  Placeholder::Arg_3) {
  return Function<R, T1, T2, T3>(func);
}

} // namespace base
} // namespace embb

#endif // EMBB_BASE_INTERNAL_FUNCTION3_H_
