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

#ifndef EMBB_BASE_BASE_H_
#define EMBB_BASE_BASE_H_

/**
 * \defgroup CPP C++ Components
 * Components written in C++.
 */

/**
 * \defgroup CPP_CONCEPT C++ Concepts
 * Concepts for C++ components.
 */

/**
 * \defgroup CPP_BASE Base
 * \ingroup CPP
 * Platform-independent abstraction layer for multithreading and basic
 * operations.
 *
 * Base C++ is mainly a C++ wrapper around the Base C abstractions. It adds
 * additional convenience types and functions that leverage the capabilities of
 * C++ such as templates, operator overloading, or RAII paradigms.
 */

#include <embb/base/atomic.h>
#include <embb/base/condition_variable.h>
#include <embb/base/core_set.h>
#include <embb/base/duration.h>
#include <embb/base/exceptions.h>
#include <embb/base/function.h>
#include <embb/base/memory_allocation.h>
#include <embb/base/mutex.h>
#include <embb/base/thread.h>
#include <embb/base/thread_specific_storage.h>
#include <embb/base/time.h>

#endif // EMBB_BASE_BASE_H_
