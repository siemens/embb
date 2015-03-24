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

#ifndef EMBB_TASKS_TASKS_H_
#define EMBB_TASKS_TASKS_H_

/**
 * \defgroup CPP_TASKS Tasks
 * Simple task management based on MTAPI.
 * \ingroup CPP
 */

#include <embb/tasks/internal/cmake_config.h>

#define TASKS_CPP_JOB 1
#if TASKS_CPP_AUTOMATIC_INITIALIZE
#define TASKS_CPP_AUTOMATIC_DOMAIN_ID 1
#define TASKS_CPP_AUTOMATIC_NODE_ID 1
#endif

#include <embb/tasks/execution_policy.h>
#include <embb/tasks/action.h>
#include <embb/tasks/continuation.h>
#include <embb/tasks/group.h>
#include <embb/tasks/node.h>
#include <embb/tasks/queue.h>
#include <embb/tasks/task.h>
#include <embb/tasks/task_context.h>

#endif // EMBB_TASKS_TASKS_H_
