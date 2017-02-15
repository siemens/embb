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

#ifndef EMBB_BASE_C_BASE_H_
#define EMBB_BASE_C_BASE_H_

/**
 * \defgroup C C Components
 * Components written in C.
 */

/**
 * \defgroup C_BASE Base
 * \ingroup C
 * Platform-independent abstraction layer for multithreading and basic
 * operations.
 *
 * This component provides basic functionalities, mainly for creating and
 * synchronizing threads. Most of the functions are essentially wrappers for
 * functions specific to the underlying operating system.
 */

#include <embb/base/c/atomic.h>
#include <embb/base/c/condition_variable.h>
#include <embb/base/c/core_set.h>
#include <embb/base/c/counter.h>
#include <embb/base/c/duration.h>
#include <embb/base/c/errors.h>
#include <embb/base/c/log.h>
#include <embb/base/c/memory_allocation.h>
#include <embb/base/c/mutex.h>
#include <embb/base/c/thread.h>
#include <embb/base/c/thread_specific_storage.h>
#include <embb/base/c/time.h>

#endif /* EMBB_BASE_C_BASE_H_ */
