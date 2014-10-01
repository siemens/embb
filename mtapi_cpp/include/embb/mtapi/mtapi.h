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

#ifndef EMBB_MTAPI_MTAPI_H_
#define EMBB_MTAPI_MTAPI_H_

/**
 * \defgroup CPP_MTAPI MTAPI
 * C++ wrapper around C implementation of MTAPI.
 * For a description of the basic concepts, see the
 * \ref C_MTAPI "C implementation of MTAPI".
 * \ingroup CPP
 */

#define MTAPI_CPP_TASK_JOB 1
#define MTAPI_CPP_AUTOMATIC_INITIALIZE 1
#if MTAPI_CPP_AUTOMATIC_INITIALIZE
#define MTAPI_CPP_AUTOMATIC_DOMAIN_ID 1
#define MTAPI_CPP_AUTOMATIC_NODE_ID 1
#endif

#include <embb/mtapi/action.h>
#include <embb/mtapi/affinity.h>
#include <embb/mtapi/continuation.h>
#include <embb/mtapi/group.h>
#include <embb/mtapi/node.h>
#include <embb/mtapi/queue.h>
#include <embb/mtapi/task.h>
#include <embb/mtapi/taskcontext.h>

#endif // EMBB_MTAPI_MTAPI_H_
