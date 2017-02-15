-- Copyright (c) 2014-2017, Siemens AG. All rights reserved.
-- 
-- Redistribution and use in source and binary forms, with or without
-- modification, are permitted provided that the following conditions are met:
-- 
-- 1. Redistributions of source code must retain the above copyright notice,
-- this list of conditions and the following disclaimer.
-- 
-- 2. Redistributions in binary form must reproduce the above copyright notice,
-- this list of conditions and the following disclaimer in the documentation
-- and/or other materials provided with the distribution.
-- 
-- THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
-- AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
-- IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
-- ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
-- LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
-- CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
-- SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
-- INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
-- CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
-- ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
-- POSSIBILITY OF SUCH DAMAGE.

max_args = 5

function make_functionN(arg_cnt)
  local file = io.open("function"..arg_cnt..".h", "w")

  local include_guard = "EMBB_BASE_INTERNAL_FUNCTION"..arg_cnt.."_H_"

  local templ_param = ""
  local param_list_a = ""
  local param_list_c = ""
  local param_list = ""
  for i = 1, arg_cnt, 1 do
    if i > 1 then
      templ_param = templ_param..", "
      param_list_a = param_list_a..", "
      param_list_c = param_list_c..", "
      param_list = param_list..", "
    end
    templ_param = templ_param.."typename T"..i
    param_list_a = param_list_a.."T"..i
    param_list_c = param_list_c.."T"..i.." p"..i
    param_list = param_list.."p"..i
  end

  local param_list_a_nil = param_list_a
  for i = arg_cnt+1, max_args, 1 do
    if i > 1 then
	  param_list_a_nil = param_list_a_nil..", "
	end
    param_list_a_nil = param_list_a_nil.."Nil"
  end

  local CR_templ
  local C_templ
  local R_templ
  local void_templ
  local R_param_list_a
  local void_param_list_a
  if arg_cnt > 0 then
    CR_templ = "template <class C, typename R,\n  "..templ_param..">"
    C_templ = "template <class C,\n  "..templ_param..">"
    R_templ = "template <typename R,\n  "..templ_param..">"
    void_templ = "template <"..templ_param..">"
    R_param_list_a = "R, "..param_list_a
    void_param_list_a = "void, "..param_list_a
  else
    CR_templ = "template <class C, typename R>"
    C_templ = "template <class C>"
    R_templ = "template <typename R>"
	void_templ = "template <>"
    R_param_list_a = "R"
    void_param_list_a = "void"
  end

  local FunctionN = "Function"..arg_cnt
  local FunctionPointerN = "FunctionPointer"..arg_cnt
  local MemberFunctionPointerN = "MemberFunctionPointer"..arg_cnt
  local FunctorWrapperN = "FunctorWrapper"..arg_cnt


  file:write("#ifndef "..include_guard.."\n")
  file:write("#define "..include_guard.."\n")
  file:write("\n")

  file:write("#include <new>\n")
  file:write("#include <cstddef>\n")
  file:write("\n")
  file:write("#include <embb/base/internal/nil.h>\n")
  file:write("#include <embb/base/memory_allocation.h>\n")
  file:write("#include <embb/base/atomic.h>\n")
  file:write("#include <embb/base/internal/functionT.h>\n")
  if arg_cnt > 0 then
    file:write("#include <embb/base/internal/function0.h>\n")
  end
  if arg_cnt > 1 then
    file:write("#include <embb/base/internal/function1.h>\n")
  end
  file:write("\n")

  file:write("namespace embb {\n")
  file:write("namespace base {\n")
  file:write("\n")
  file:write("namespace internal {\n")
  file:write("\n")

  file:write(R_templ.."\n")
  file:write("class "..FunctionN.." {\n")
  file:write(" public:\n")
  file:write("  virtual ~"..FunctionN.."() {}\n")
  file:write("  virtual R operator () ("..param_list_a..") = 0;\n")
  file:write("  virtual void CopyTo(void* dst) = 0;\n")
  file:write("};\n")
  file:write("\n")

  file:write(R_templ.."\n")
  file:write("class "..FunctionPointerN.."\n  : public "..FunctionN.."<"..R_param_list_a.."> {\n")
  file:write(" public:\n")
  file:write("  typedef R(*FuncPtrType)("..param_list_a..");\n")
  file:write("  explicit "..FunctionPointerN.."(FuncPtrType func) : function_(func) {}\n")
  file:write("  virtual R operator () ("..param_list_c..") {\n")
  file:write("    return function_("..param_list..");\n")
  file:write("  }\n")
  file:write("  virtual void CopyTo(void* dst) {\n")
  file:write("    new(dst)"..FunctionPointerN.."(function_);\n")
  file:write("  }\n")
  file:write("\n")
  file:write(" private:\n")
  file:write("  FuncPtrType function_;\n")
  file:write("};\n")
  file:write("\n")

  file:write(void_templ.."\n")
  file:write("class "..FunctionPointerN.."<"..void_param_list_a..">\n  : public "..FunctionN.."<"..void_param_list_a.."> {\n")
  file:write(" public:\n")
  file:write("  typedef void(*FuncPtrType)("..param_list_a..");\n")
  file:write("  explicit "..FunctionPointerN.."(FuncPtrType func) : function_(func) {}\n")
  file:write("  virtual void operator () ("..param_list_c..") {\n")
  file:write("    function_("..param_list..");\n")
  file:write("  }\n")
  file:write("  virtual void CopyTo(void* dst) {\n")
  file:write("    new(dst)"..FunctionPointerN.."(function_);\n")
  file:write("  }\n")
  file:write("\n")
  file:write(" private:\n")
  file:write("  FuncPtrType function_;\n")
  file:write("};\n")
  file:write("\n")

  file:write(CR_templ.."\n")
  file:write("class "..MemberFunctionPointerN.."\n  : public "..FunctionN.."<"..R_param_list_a.."> {\n")
  file:write(" public:\n")
  file:write("  typedef R(C::*MemFuncPtrType)("..param_list_a..");\n")
  file:write("  typedef C & ClassRefType;\n")
  file:write("  "..MemberFunctionPointerN.."(ClassRefType obj, MemFuncPtrType func)\n  : object_(obj), function_(func) {}\n")
  file:write("  explicit "..MemberFunctionPointerN.."(ClassRefType obj)\n  : object_(obj), function_(&C::operator()) {}\n")
  file:write("  void operator = ("..MemberFunctionPointerN.." const & memfunc) {\n")
  file:write("    object_ = memfunc.object_;\n")
  file:write("    function_ = memfunc.function_;\n")
  file:write("  }\n")
  file:write("  virtual R operator () ("..param_list_c..") {\n")
  file:write("    return (object_.*function_)("..param_list..");\n")
  file:write("  }\n")
  file:write("  virtual void CopyTo(void* dst) {\n")
  file:write("    new(dst)"..MemberFunctionPointerN.."(object_, function_);\n")
  file:write("  }\n")
  file:write("\n")
  file:write(" private:\n")
  file:write("  ClassRefType object_;\n")
  file:write("  MemFuncPtrType function_;\n")
  file:write("};\n")
  file:write("\n")
 
  file:write(C_templ.."\n")
  file:write("class "..MemberFunctionPointerN.."<C, "..void_param_list_a..">\n  : public "..FunctionN.."<"..void_param_list_a.."> {\n")
  file:write(" public:\n")
  file:write("  typedef void(C::*MemFuncPtrType)("..param_list_a..");\n")
  file:write("  typedef C & ClassRefType;\n")
  file:write("  "..MemberFunctionPointerN.."(ClassRefType obj, MemFuncPtrType func)\n  : object_(obj), function_(func) {}\n")
  file:write("  explicit "..MemberFunctionPointerN.."(ClassRefType obj)\n  : object_(obj), function_(&C::operator()) {}\n")
  file:write("  void operator = ("..MemberFunctionPointerN.." const & memfunc) {\n")
  file:write("    object_ = memfunc.object_;\n")
  file:write("    function_ = memfunc.function_;\n")
  file:write("  }\n")
  file:write("  virtual void operator () ("..param_list_c..") {\n")
  file:write("    (object_.*function_)("..param_list..");\n")
  file:write("  }\n")
  file:write("  virtual void CopyTo(void* dst) {\n")
  file:write("    new(dst)"..MemberFunctionPointerN.."(object_, function_);\n")
  file:write("  }\n")
  file:write("\n")
  file:write(" private:\n")
  file:write("  ClassRefType object_;\n")
  file:write("  MemFuncPtrType function_;\n")
  file:write("};\n")
  file:write("\n")

  file:write(CR_templ.."\n")
  file:write("class "..FunctorWrapperN.."\n  : public "..FunctionN.."<"..R_param_list_a.."> {\n")
  file:write(" public:\n")
  file:write("  "..FunctorWrapperN.."() : object_(NULL), ref_count_(NULL) {}\n")
  file:write("  explicit "..FunctorWrapperN.."(C const & obj) {\n")
  file:write("    object_ = Allocation::New<C>(obj);\n")
  file:write("    ref_count_ = Allocation::New<Atomic<int> >(1);\n")
  file:write("  }\n")
  file:write("  explicit "..FunctorWrapperN.."("..FunctorWrapperN.." const & other) {\n")
  file:write("    object_ = other.object_;\n")
  file:write("    ref_count_ = other.ref_count_;\n")
  file:write("    ++*ref_count_;\n")
  file:write("  }\n")
  file:write("  virtual ~"..FunctorWrapperN.."() {\n")
  file:write("    if (0 == --*ref_count_) {\n")
  file:write("      Allocation::Delete(ref_count_);\n")
  file:write("      Allocation::Delete(object_);\n")
  file:write("    }\n")
  file:write("  }\n")
  file:write("  virtual R operator () ("..param_list_c..") {\n")
  file:write("    return (*object_)("..param_list..");\n")
  file:write("  }\n")
  file:write("  virtual void CopyTo(void* dst) {\n")
  file:write("    new(dst)"..FunctorWrapperN.."(*this);\n")
  file:write("  }\n")
  file:write("\n")
  file:write(" private:\n")
  file:write("  C * object_;\n")
  file:write("  Atomic<int> * ref_count_;\n")
  file:write("};\n")
  file:write("\n")

  file:write(C_templ.."\n")
  file:write("class "..FunctorWrapperN.."<C, "..void_param_list_a..">\n  : public "..FunctionN.."<"..void_param_list_a.."> {\n")
  file:write(" public:\n")
  file:write("  "..FunctorWrapperN.."() : object_(NULL), ref_count_(NULL) {}\n")
  file:write("  explicit "..FunctorWrapperN.."(C const & obj) {\n")
  file:write("    object_ = Allocation::New<C>(obj);\n")
  file:write("    ref_count_ = Allocation::New<Atomic<int> >(1);\n")
  file:write("  }\n")
  file:write("  explicit "..FunctorWrapperN.."("..FunctorWrapperN.." const & other) {\n")
  file:write("    object_ = other.object_;\n")
  file:write("    ref_count_ = other.ref_count_;\n")
  file:write("    ++*ref_count_;\n")
  file:write("  }\n")
  file:write("  virtual ~"..FunctorWrapperN.."() {\n")
  file:write("    if (0 == --*ref_count_) {\n")
  file:write("      Allocation::Delete(ref_count_);\n")
  file:write("      Allocation::Delete(object_);\n")
  file:write("    }\n")
  file:write("  }\n")
  file:write("  virtual void operator () ("..param_list_c..") {\n")
  file:write("    (*object_)("..param_list..");\n")
  file:write("  }\n")
  file:write("  virtual void CopyTo(void* dst) {\n")
  file:write("    new(dst)"..FunctorWrapperN.."(*this);\n")
  file:write("  }\n")
  file:write("\n")
  file:write(" private:\n")
  file:write("  C * object_;\n")
  file:write("  Atomic<int> * ref_count_;\n")
  file:write("};\n")
  file:write("\n")

  -- bind to function0
  if arg_cnt > 0 then
    local param_list_ = ""
    for i = 1, arg_cnt, 1 do
      if i > 1 then
        param_list_ = param_list_..", "
      end
      param_list_ = param_list_.."p"..i.."_"
    end
    file:write("// bind to function0\n")
    file:write(R_templ.."\n")
    file:write("class Bound"..arg_cnt.."Functor0 {\n")
    file:write(" public:\n")
    file:write("  Bound"..arg_cnt.."Functor0(Function<"..R_param_list_a.."> func,\n    "..param_list_c..") : function_(func)\n      ")
    for i = 1, arg_cnt, 1 do
      file:write(", p"..i.."_(p"..i..")")
    end
    file:write(" {}\n")
    file:write("  Bound"..arg_cnt.."Functor0(Bound"..arg_cnt.."Functor0 const & func) : function_(func.function_)\n    ")
    for i = 1, arg_cnt, 1 do
      file:write(", p"..i.."_(func.p"..i.."_)")
	  if i == 4 then
	    file:write("\n    ")
      end
    end
    file:write(" {}\n")
    file:write("  R operator() () {\n")
    file:write("    return function_("..param_list_..");\n")
    file:write("  }\n")
  file:write("\n")
    file:write(" private:\n")
    file:write("  Function<"..R_param_list_a.."> function_;\n")
    for i = 1, arg_cnt, 1 do
      file:write("  T"..i.." p"..i.."_;\n")
    end
    file:write("};\n")
    file:write("\n")
  end

  -- bind to function1
  if arg_cnt > 1 then
    file:write("// bind to function1\n")
    for arg = 1, arg_cnt, 1 do
      local param_list_ = ""
      local param_list_c_ = ""
      for i = 1, arg_cnt, 1 do
        if i > 1 then
          param_list_ = param_list_..", "
        end
        param_list_ = param_list_.."p"..i
        if i ~= arg then
          param_list_ = param_list_.."_"
          param_list_c_ = param_list_c_..", T"..i.." p"..i
        end
      end
      file:write(R_templ.."\n")
      file:write("class Bound"..arg_cnt.."Functor1_Arg"..arg.." {\n")
      file:write(" public:\n")
      file:write("  Bound"..arg_cnt.."Functor1_Arg"..arg.."(Function<"..R_param_list_a.."> func\n    "..param_list_c_..") : function_(func)\n      ")
      for i = 1, arg_cnt, 1 do
        if i ~= arg then
          file:write(", p"..i.."_(p"..i..")")
        end
      end
      file:write(" {}\n")
      file:write("  Bound"..arg_cnt.."Functor1_Arg"..arg.."(Bound"..arg_cnt.."Functor1_Arg"..arg.." const & func)\n    : function_(func.function_)\n    ")
      for i = 1, arg_cnt, 1 do
        if i ~= arg then
          file:write(", p"..i.."_(func.p"..i.."_)")
        end
      end
      file:write(" {}\n")
      file:write("  R operator() (T"..arg.." p"..arg..") {\n")
      file:write("    return function_("..param_list_..");\n")
      file:write("  }\n")
  file:write("\n")
      file:write(" private:\n")
      file:write("  Function<"..R_param_list_a.."> function_;\n")
      for i = 1, arg_cnt, 1 do
        if i ~= arg then
          file:write("  T"..i.." p"..i.."_;\n")
        end
      end
      file:write("};\n")
      file:write("\n")
    end
  end
  
  file:write("} // namespace internal\n")
  file:write("\n")

  file:write("\n")
  file:write("using embb::base::internal::Nil;\n")
  file:write("\n")

  file:write(R_templ.."\n")
  if arg_cnt < max_args then
    file:write("class Function<R, "..param_list_a_nil.."> {\n")
  else
    file:write("class Function {\n")
  end
  file:write(" public:\n")
  file:write("  typedef internal::"..FunctionN.."<"..R_param_list_a.."> * FuncPtrType;\n")
  file:write("  Function() : function_(NULL) {}\n")
  file:write("  template <class C>\n")
  file:write("  explicit Function(C const & obj) {\n")
  file:write("    function_ = new(storage_)\n      internal::"..FunctorWrapperN.."<C, "..R_param_list_a..">(obj);\n")
  file:write("  }\n")
  file:write("  Function(Function const & func) {\n")
  file:write("    func.function_->CopyTo(&storage_[0]);\n")
  file:write("    function_ = reinterpret_cast<FuncPtrType>(&storage_[0]);\n")
  file:write("  }\n")
  file:write("  ~Function() {\n")
  file:write("    Free();\n")
  file:write("  }\n")
  file:write("  void operator = (R(*func)("..param_list_a..")) {\n")
  file:write("    Free();\n")
  file:write("    function_ = new(storage_)\n      internal::"..FunctionPointerN.."<"..R_param_list_a..">(func);\n")
  file:write("  }\n")
  file:write("  void operator = (Function & func) {\n")
  file:write("    Free();\n")
  file:write("    func.function_->CopyTo(&storage_[0]);\n")
  file:write("    function_ = reinterpret_cast<FuncPtrType>(&storage_[0]);\n")
  file:write("  }\n")
  file:write("  template <class C>\n")
  file:write("  void operator = (C const & obj) {\n")
  file:write("    Free();\n")
  file:write("    function_ = new(storage_)\n      internal::"..FunctorWrapperN.."<C, "..R_param_list_a..">(obj);\n")
  file:write("  }\n")
  file:write("  explicit Function(R(*func)("..param_list_a..")) {\n")
  file:write("    function_ = new(storage_)\n      internal::"..FunctionPointerN.."<"..R_param_list_a..">(func);\n")
  file:write("  }\n")
  file:write("  template <class C>\n")
  file:write("  Function(C & obj, R(C::*func)("..param_list_a..")) {\n")
  file:write("    function_ = new(storage_)\n      internal::"..MemberFunctionPointerN.."<C, "..R_param_list_a..">(obj, func);\n")
  file:write("  }\n")
  file:write("  R operator () ("..param_list_c..") {\n")
  file:write("    return (*function_)("..param_list..");\n")
  file:write("  }\n")
  file:write("\n")
  file:write(" private:\n")
  file:write("  char storage_[sizeof(\n    internal::"..MemberFunctionPointerN.."<Nil, "..R_param_list_a..">)];\n")
  file:write("  FuncPtrType function_;\n")
  file:write("  void Free() {\n")
  file:write("    if (NULL != function_) {\n")
  file:write("      function_->~"..FunctionN.."();\n")
  file:write("      function_ = NULL;\n")
  file:write("    }\n")
  file:write("  }\n")
  file:write("};\n")
  file:write("\n")

  -- wrap member function
  file:write("// wrap member function\n")
  file:write(CR_templ.."\n")
  file:write("Function<"..R_param_list_a.."> MakeFunction(C & obj,\n  R(C::*func)("..param_list_a..")) {\n")
  file:write("  return Function<"..R_param_list_a..">(obj, func);\n")
  file:write("}\n")
  file:write("\n")

  -- wrap function pointer
  file:write("// wrap function pointer\n")
  file:write(R_templ.."\n")
  file:write("Function<"..R_param_list_a.."> MakeFunction(\n  R(*func)("..param_list_a..")) {\n")
  file:write("  return Function<"..R_param_list_a..">(func);\n")
  file:write("}\n")
  file:write("\n")

  -- bind to function0
  if arg_cnt > 0 then
    file:write("// bind to function0\n")

    file:write(R_templ.."\n")
    file:write("Function<R> Bind(Function<"..R_param_list_a.."> func,\n  "..param_list_c..") {\n")
    file:write("  return Function<R>(\n  internal::Bound"..arg_cnt.."Functor0<"..R_param_list_a..">(\n    func, "..param_list.."));\n")
    file:write("}\n")
    file:write("\n")

    file:write(R_templ.."\n")
    file:write("Function<R> Bind(R(*func)("..param_list_a.."),\n  "..param_list_c..") {\n")
    file:write("  return Bind(Function<"..R_param_list_a..">(func), "..param_list..");\n")
    file:write("}\n")
    file:write("\n")

    file:write(CR_templ.."\n")
    file:write("Function<R> Bind(C & obj, R(C::*func)("..param_list_a.."),\n  "..param_list_c..") {\n")
    file:write("  return Bind(Function<"..R_param_list_a..">(obj, func), "..param_list..");\n")
    file:write("}\n")
    file:write("\n")
  end

  -- bind to function1
  if arg_cnt > 1 then
    file:write("// bind to function1\n")
    for arg = 1, arg_cnt, 1 do
      local target_arg = "T"..arg
      local bound_param_list_a = ""
      local bound_param_list_c = ""
      local bound_param_list = ""
      for i = 1, arg_cnt, 1 do
        if i == arg then
          bound_param_list_c = bound_param_list_c..", Placeholder::Arg_1 p"..i
          bound_param_list_a = bound_param_list_a..", Placeholder::Arg_1"
        else
          bound_param_list_c = bound_param_list_c..", T"..i.." p"..i
          bound_param_list_a = bound_param_list_a..", T"..i.." p"..i
          bound_param_list = bound_param_list..", p"..i
        end
      end

      file:write(R_templ.."\n")
      file:write("Function<R, "..target_arg.."> Bind(Function<"..R_param_list_a.."> func\n  "..bound_param_list_a..") {\n")
      file:write("  return Function<R, "..target_arg..">(\n  internal::Bound"..arg_cnt.."Functor1_Arg"..arg.."<"..R_param_list_a..">(func\n    "..bound_param_list.."));\n")
      file:write("}\n")
      file:write("\n")

      file:write(R_templ.."\n")
      file:write("Function<R, "..target_arg.."> Bind(R(*func)("..param_list_a..")\n  "..bound_param_list_c..") {\n")
      file:write("  return Bind(Function<"..R_param_list_a..">(func), "..param_list..");\n")
      file:write("}\n")
      file:write("\n")

      file:write(CR_templ.."\n")
      file:write("Function<R, "..target_arg.."> Bind(C & obj, R(C::*func)("..param_list_a..")\n  "..bound_param_list_c..") {\n")
      file:write("  return Bind(Function<"..R_param_list_a..">(obj, func), "..param_list..");\n")
      file:write("}\n")
      file:write("\n")
    end
  end

  -- bind to functionN
  file:write("// bind to "..FunctionN.."\n")

  local bound_param_list_c = ""
  for i = 1, arg_cnt, 1 do
    bound_param_list_c = bound_param_list_c..",\n  Placeholder::Arg_"..i
  end

  file:write(CR_templ.."\n")
  file:write("Function<"..R_param_list_a.."> Bind(\n  C & obj,\n  R(C::*func)("..param_list_a..")"..bound_param_list_c..") {\n")
  file:write("  return Function<"..R_param_list_a..">(obj, func);\n")
  file:write("}\n")
  file:write("\n")

  file:write(R_templ.."\n")
  file:write("Function<"..R_param_list_a.."> Bind(\n  R(*func)("..param_list_a..")"..bound_param_list_c..") {\n")
  file:write("  return Function<"..R_param_list_a..">(func);\n")
  file:write("}\n")
  file:write("\n")

  file:write("} // namespace base\n")
  file:write("} // namespace embb\n")
  file:write("\n")

  file:write("#endif // "..include_guard.."\n")

  file:flush()
  file:close()
end

for i = 0, max_args, 1 do
  make_functionN(i)
end
