#!/usr/bin/env python

# Copyright (c) 2014-2017, Siemens AG. All rights reserved.
# Copyright (c) 2016-2017, embedded brains GmbH. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

# Use this script generate the atomic operations implementation header:
#
#   ./gen_atomic.py > ../base_c/include/embb/base/c/internal/atomic/atomic.h
#
# It provides an implementation of the EMBB atomic operations by means provided
# by the C11 and C++11 standards.

import re

print("""/*
 * Copyright (c) 2014-2017, Siemens AG. All rights reserved.
 * Copyright (c) 2016-2017, embedded brains GmbH. All rights reserved.
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

/*
 * This file was automatically generated.  Do not edit it.  Edit gen_atomic.py
 * instead and regenerate this file with it.
 */

#ifndef EMBB_BASE_C_INTERNAL_ATOMIC_ATOMIC_H_
#define EMBB_BASE_C_INTERNAL_ATOMIC_ATOMIC_H_

#include <embb/base/c/internal/config.h>
#include <embb/base/c/internal/atomic/atomic_sizes.h>

#include <stddef.h>
#include <stdint.h>

#if defined EMBB_PLATFORM_ARCH_CXX11
#include <atomic>
#elif defined EMBB_PLATFORM_ARCH_C11
#include <stdatomic.h>
#else
#include <string.h>
#include <embb/base/c/internal/macro_helper.h>
#if defined EMBB_PLATFORM_ARCH_X86
#ifdef EMBB_PLATFORM_COMPILER_MSVC
#include <intrin.h>
extern void __fastcall embb_internal__atomic_memory_barrier_asm();
#endif
#elif !defined EMBB_PLATFORM_ARCH_ARM
#error "unsupported platform"
#endif
#endif

EMBB_PLATFORM_INLINE void embb_atomic_memory_barrier(void) {
#if defined EMBB_PLATFORM_ARCH_CXX11
  std::atomic_thread_fence(std::memory_order_seq_cst);
#elif defined EMBB_PLATFORM_ARCH_C11
  atomic_thread_fence(memory_order_seq_cst);
#elif defined EMBB_PLATFORM_ARCH_X86
#ifdef EMBB_PLATFORM_COMPILER_MSVC
  _ReadWriteBarrier();
  embb_internal__atomic_memory_barrier_asm();
  _ReadWriteBarrier();
#else
  __asm__ __volatile__ ("mfence" : : : "memory");
#endif
#elif defined EMBB_PLATFORM_ARCH_ARM
  __asm__ __volatile__ ("dmb" : : : "memory");
#endif
}
""")

class Type:
	def __init__(self, name, stdName):
		self.mName = name
		self.mStdName = stdName

	def name(self):
		return self.mName

	def stdName(self):
		return self.mStdName

	def designator(self):
		return self.mName.replace(' ', '_')

	def constantDesignator(self):
		return self.mName.replace(' ', '_').upper()

	def sizeDefine(self):
		return "EMBB_" + self.constantDesignator() + "_TYPE_SIZE"

	def basicType(self):
		return "EMBB_XCAT2(EMBB_BASE_BASIC_TYPE_SIZE_, " + self.sizeDefine() + ")"

# Internal typedefs

internalTypes = [
	Type("1", "uint_least8_t"),
	Type("2", "uint_least16_t"),
	Type("4", "uint_least32_t"),
	Type("8", "uint_least64_t"),
]

for t in internalTypes:
	print("typedef " + t.stdName() + " EMBB_BASE_BASIC_TYPE_SIZE_" + t.designator() + ";")
print("")

print("#if defined EMBB_PLATFORM_ARCH_CXX11\n")
for t in internalTypes:
	print("typedef std::atomic_" + t.stdName() + " EMBB_BASE_BASIC_TYPE_ATOMIC_" + t.designator() + ";")
print("\n#elif defined EMBB_PLATFORM_ARCH_C11\n")
for t in internalTypes:
	print("typedef atomic_" + t.stdName() + " EMBB_BASE_BASIC_TYPE_ATOMIC_" + t.designator() + ";")
print("\n#else\n")
for t in internalTypes:
	print("typedef EMBB_BASE_BASIC_TYPE_SIZE_" + t.designator() + " EMBB_BASE_BASIC_TYPE_ATOMIC_" + t.designator() + ";")
print("\n#endif\n")

# Internal ops

class InternalOp:
	def __init__(self, embbOpFormat, stdOpFormat):
		self.mEMBBOpFormat = embbOpFormat
		self.mStdOpFormat = stdOpFormat

	def embbOp(self, d):
		return self.mEMBBOpFormat.replace("%D", d).replace("%A", "EMBB_BASE_BASIC_TYPE_ATOMIC_" + d).replace("%B", "EMBB_BASE_BASIC_TYPE_SIZE_" + d)

	def stdOp(self, p, v, c, d):
		return self.mStdOpFormat.replace("%P", p).replace("%V", v).replace("%C", c).replace("%D", d)

internalOps = [
	InternalOp("void embb_internal__atomic_and_assign_%D(\n  %A* variable,\n  %B value\n  )",
	    "(void)%Pfetch_and(%V%Cvalue)"),
	InternalOp("int embb_internal__atomic_compare_and_swap_%D(\n  %A* variable,\n  %B* expected,\n  %B desired\n  )",
	    "return %Pcompare_exchange_strong(%V%C%Dexpected, desired)"),
	InternalOp("%B embb_internal__atomic_fetch_and_add_%D(\n  %A* variable,\n  %B value\n  )",
	    "return %Pfetch_add(%V%Cvalue)"),
	InternalOp("%B embb_internal__atomic_load_%D(\n  %A* variable\n  )",
	    "return %Pload(%V)"),
	InternalOp("void embb_internal__atomic_or_assign_%D(\n  %A* variable,\n  %B value\n  )",
	    "(void)%Pfetch_or(%V%Cvalue)"),
	InternalOp("void embb_internal__atomic_store_%D(\n  %A* variable,\n  %B value\n  )",
	    "%Pstore(%V%Cvalue)"),
	InternalOp("%B embb_internal__atomic_swap_%D(\n  %A* variable,\n  %B value\n  )",
	    "return %Pexchange(%V%Cvalue)"),
	InternalOp("void embb_internal__atomic_xor_assign_%D(\n  %A* variable,\n  %B value\n  )",
	    "(void)%Pfetch_xor(%V%Cvalue)"),
]

print("#if defined EMBB_PLATFORM_ARCH_CXX11 || defined EMBB_PLATFORM_ARCH_C11\n")
for o in internalOps:
	for t in internalTypes:
		s = "EMBB_PLATFORM_INLINE " + o.embbOp(t.designator()) + " {\n"
		s += "#if defined EMBB_PLATFORM_ARCH_CXX11\n"
		s += "  " + o.stdOp("variable->", "", "", "*") + ";\n"
		s += "#elif defined EMBB_PLATFORM_ARCH_C11\n"
		s += "  " + o.stdOp("atomic_", "variable", ", ", "") + ";\n"
		s += "#endif\n"
		s += "}\n"
		print(s)

print("""#else

#include <embb/base/c/internal/atomic/load.h>
#include <embb/base/c/internal/atomic/and_assign.h>
#include <embb/base/c/internal/atomic/store.h>
#include <embb/base/c/internal/atomic/or_assign.h>
#include <embb/base/c/internal/atomic/xor_assign.h>
#include <embb/base/c/internal/atomic/swap.h>
#include <embb/base/c/internal/atomic/fetch_and_add.h>
#include <embb/base/c/internal/atomic/compare_and_swap.h>

#endif
""")

# Typedefs

types = [
	Type("char", "char"),
	Type("short", "short"),
	Type("unsigned short", "ushort"),
	Type("int", "int"),
	Type("unsigned int", "uint"),
	Type("long", "long"),
	Type("unsigned long", "ulong"),
	Type("long long", "llong"),
	Type("unsigned long long", "ullong"),
	Type("intptr_t", "intptr_t"),
	Type("uintptr_t", "uintptr_t"),
	Type("size_t", "size_t"),
	Type("ptrdiff_t", "ptrdiff_t"),
	Type("uintmax_t", "uintmax_t")
]

for t in types:
	s = "#ifdef EMBB_" + t.constantDesignator() + "_TYPE_IS_ATOMIC\n"
	s += "typedef struct {\n"
	s += "#if defined EMBB_PLATFORM_ARCH_CXX11\n"
	s += "  std::atomic_" + t.stdName() + " internal_variable;\n"
	s += "#elif defined EMBB_PLATFORM_ARCH_C11\n"
	s += "  atomic_" + t.stdName() + " internal_variable;\n"
	s += "#else\n"
	s += "  " + t.basicType() + " internal_variable;\n"
	s += "#endif\n"
	s += "#ifdef EMBB_THREADING_ANALYSIS_MODE\n"
	s += "  embb_mutex_t internal_mutex;\n"
	s += "#endif\n"
	s += "#ifdef EMBB_DEBUG\n"
	s += "  volatile uint32_t marker;\n"
	s += "#endif\n"
	s += "} embb_atomic_" + t.designator() + ";\n"
	s += "#endif\n"
	print(s)

# Ops

class Op:
	def __init__(self, returnType, embbOpFormat, internalOpFormat, stdCOpFormat, stdCXXOpFormat = None, preOp = "  EMBB_ATOMIC_INIT_CHECK(variable);\n  EMBB_ATOMIC_MUTEX_LOCK(variable->internal_mutex);\n", postOp = "  EMBB_ATOMIC_MUTEX_UNLOCK(variable->internal_mutex);\n"):
		self.mReturnType = returnType
		self.mEMBBOpFormat = embbOpFormat
		self.mInternalOpFormat = internalOpFormat
		self.mStdCOpFormat = stdCOpFormat
		self.mPreOp = preOp
		self.mPostOp = postOp

		if stdCXXOpFormat == None:
			self.mStdCXXOpFormat = stdCOpFormat
		else:
			self.mStdCXXOpFormat = stdCXXOpFormat

	def embbOp(self, t):
		n = t.name()
		d = t.designator()
		return self.mReturnType.replace("%T", n) + " " + self.mEMBBOpFormat.replace("%T", n).replace("%D", d).replace("%A", "embb_atomic_" + d)

	def stdOp(self, t, f, p, v, c, d):
		s = ""
		if self.hasReturnValue():
			s += self.mReturnType.replace("%T", t.name()) + " return_val = "
		return s + f.replace("%P", p).replace("%V", v).replace("%C", c).replace("%D", d)

	def stdCOp(self, t):
		return self.stdOp(t, self.mStdCOpFormat, "atomic_", "&variable->internal_variable", ", ", "")

	def stdCXXOp(self, t):
		return self.stdOp(t, self.mStdCXXOpFormat, "variable->internal_variable.", "", "", "*")

	def legacyOp(self, t):
		s = ""
		r = ""
		if self.hasReturnValue():
			if self.hasBasicReturnValue():
				r = t.basicType() + " return_val_basic = "
			else:
				r = self.mReturnType + " return_val = "
		p = "EMBB_XCAT2(embb_internal__atomic_"
		sz = "_, " + t.sizeDefine() + ")"
		a = "&variable->internal_variable"
		c = "(" + t.basicType() + " *)"
		s += re.sub(r"%I{([^}]+)}", t.basicType() + " \\1_basic;\n  memcpy(&\\1_basic, &\\1, sizeof(\\1_basic));\n  ", self.mInternalOpFormat).replace("%R", r).replace("%P", p).replace("%S", sz).replace("%A", a).replace("%C", c)
		if self.hasBasicReturnValue():
			s += ";\n  " + t.name() + " return_val;\n"
			s += "  memcpy(&return_val, &return_val_basic, sizeof(return_val))"
		return s

	def preOp(self):
		return self.mPreOp

	def postOp(self):
		return self.mPostOp

	def hasReturnValue(self):
		return self.mReturnType != "void"

	def hasBasicReturnValue(self):
		return self.mReturnType == "%T"

	def returnStatement(self):
		if self.hasReturnValue():
			return "  return return_val;\n"
		else:
			return ""

ops = [
	Op("void",
	    "embb_atomic_init_%D(\n  %A* variable,\n  %T value\n  )",
	    "%I{value}%Pstore%S(%A, value_basic)",
	    "%Pinit(%V%Cvalue)",
	    "%Pstore(%V%Cvalue)",
	    "  EMBB_ATOMIC_MUTEX_INIT(variable->internal_mutex);\n  EMBB_ATOMIC_INIT_MARKER(variable);\n",
	    ""),
	Op("void",
	    "embb_atomic_and_assign_%D(\n  %A* variable,\n  %T value\n  )",
	    "%I{value}%Pand_assign%S(%A, value_basic)",
	    "(void)%Pfetch_and(%V%Cvalue)"),
	Op("int",
	    "embb_atomic_compare_and_swap_%D(\n  %A* variable,\n  %T* expected,\n  %T desired\n  )",
	    "%I{desired}%R%Pcompare_and_swap%S(%A, %Cexpected, desired_basic)",
	    "%Pcompare_exchange_strong(%V%C%Dexpected, desired)"),
	Op("%T",
	    "embb_atomic_fetch_and_add_%D(\n  %A* variable,\n  %T value\n  )",
	    "%I{value}%R%Pfetch_and_add%S(%A, value_basic)",
	    "%Pfetch_add(%V%Cvalue)"),
	Op("%T",
	    "embb_atomic_load_%D(\n  %A* variable\n  )",
	    "%R%Pload%S(%A)",
	    "%Pload(%V)"),
	Op("void",
	    "embb_atomic_or_assign_%D(\n  %A* variable,\n  %T value\n  )",
	    "%I{value}%Por_assign%S(%A, value_basic)",
	    "(void)%Pfetch_or(%V%Cvalue)"),
	Op("void",
	    "embb_atomic_store_%D(\n  %A* variable,\n  %T value\n  )",
	    "%I{value}%Pstore%S(%A, value_basic)",
	    "%Pstore(%V%Cvalue)"),
	Op("%T",
	    "embb_atomic_swap_%D(\n  %A* variable,\n  %T value\n  )",
	    "%I{value}%R%Pswap%S(%A, value_basic)",
	    "%Pexchange(%V%Cvalue)"),
	Op("void",
	    "embb_atomic_xor_assign_%D(\n  %A* variable,\n  %T value\n  )",
	    "%I{value}%Pxor_assign%S(%A, value_basic)",
	    "(void)%Pfetch_xor(%V%Cvalue)"),
	Op("void",
	    "embb_atomic_destroy_%D(\n  %A* variable\n  )",
	    "(void)variable",
	    "(void)variable",
	    None,
	    "  EMBB_ATOMIC_INIT_CHECK(variable);\n",
	    "  EMBB_ATOMIC_MUTEX_DESTROY(variable->internal_mutex);\n  EMBB_ATOMIC_DESTROY_MARKER(variable);\n"),
]

for o in ops:
	for t in types:
		s = "#ifdef EMBB_" + t.constantDesignator() + "_TYPE_IS_ATOMIC\n"
		s += "EMBB_PLATFORM_INLINE " + o.embbOp(t) + " {\n"
		s += o.preOp()
		s += "#if defined EMBB_PLATFORM_ARCH_CXX11\n"
		s += "  " + o.stdCXXOp(t) + ";\n"
		s += "#elif defined EMBB_PLATFORM_ARCH_C11\n"
		s += "  " + o.stdCOp(t) + ";\n"
		s += "#else\n"
		s += "  " + o.legacyOp(t) + ";\n"
		s += "#endif\n"
		s += o.postOp()
		s += o.returnStatement()
		s += "}\n"
		s += "#endif\n"
		print(s)

print("#endif //EMBB_BASE_C_INTERNAL_ATOMIC_ATOMIC_H_")
