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

#include <invoke_test.h>
#include <embb/algorithms/invoke.h>

InvokeTest::InvokeTest() {
  CreateUnit("Preliminary").Add(&InvokeTest::Test, this);
}

static void Invocable1() {}
static void Invocable2() {}
static void Invocable3() {}
static void Invocable4() {}
static void Invocable5() {}
static void Invocable6() {}
static void Invocable7() {}
static void Invocable8() {}
static void Invocable9() {}
static void Invocable10() {}

#define HETEROGENEOUS_JOB 17

static void InvocableActionFunction(
  const void* /*args*/,
  mtapi_size_t /*args_size*/,
  void* /*result_buffer*/,
  mtapi_size_t /*result_buffer_size*/,
  const void* /*node_local_data*/,
  mtapi_size_t /*node_local_data_size*/,
  mtapi_task_context_t * /*context*/) {
  // emtpy
}

void InvokeTest::Test() {
  using embb::algorithms::Invoke;
  Invoke(&Invocable1, &Invocable2);
  Invoke(&Invocable1, &Invocable2, &Invocable3);
  Invoke(&Invocable1, &Invocable2, &Invocable3, &Invocable4);
  Invoke(&Invocable1, &Invocable2, &Invocable3, &Invocable4, &Invocable5);
  Invoke(&Invocable1, &Invocable2, &Invocable3, &Invocable4, &Invocable5,
         &Invocable6);
  Invoke(&Invocable1, &Invocable2, &Invocable3, &Invocable4, &Invocable5,
         &Invocable6, &Invocable7);
  Invoke(&Invocable1, &Invocable2, &Invocable3, &Invocable4, &Invocable5,
         &Invocable6, &Invocable7, &Invocable8);
  Invoke(&Invocable1, &Invocable2, &Invocable3, &Invocable4, &Invocable5,
         &Invocable6, &Invocable7, &Invocable8, &Invocable9);
  Invoke(&Invocable1, &Invocable2, &Invocable3, &Invocable4, &Invocable5,
         &Invocable6, &Invocable7, &Invocable8, &Invocable9);
  Invoke(&Invocable1, &Invocable2, &Invocable3, &Invocable4, &Invocable5,
         &Invocable6, &Invocable7, &Invocable8, &Invocable9, &Invocable10);

  embb::mtapi::ExecutionPolicy policy;
  Invoke(&Invocable1, &Invocable2, policy);
  Invoke(&Invocable1, &Invocable2, &Invocable3, policy);
  Invoke(&Invocable1, &Invocable2, &Invocable3, &Invocable4, policy);
  Invoke(&Invocable1, &Invocable2, &Invocable3, &Invocable4, &Invocable5,
         policy);
  Invoke(&Invocable1, &Invocable2, &Invocable3, &Invocable4, &Invocable5,
         &Invocable6, policy);
  Invoke(&Invocable1, &Invocable2, &Invocable3, &Invocable4, &Invocable5,
         &Invocable6, &Invocable7, policy);
  Invoke(&Invocable1, &Invocable2, &Invocable3, &Invocable4, &Invocable5,
         &Invocable6, &Invocable7, &Invocable8, policy);
  Invoke(&Invocable1, &Invocable2, &Invocable3, &Invocable4, &Invocable5,
         &Invocable6, &Invocable7, &Invocable8, &Invocable9, policy);
  Invoke(&Invocable1, &Invocable2, &Invocable3, &Invocable4, &Invocable5,
         &Invocable6, &Invocable7, &Invocable8, &Invocable9, policy);
  Invoke(&Invocable1, &Invocable2, &Invocable3, &Invocable4, &Invocable5,
         &Invocable6, &Invocable7, &Invocable8, &Invocable9, &Invocable10,
         policy);

  // test one heterogeneous variant
  embb::mtapi::Node & node = embb::mtapi::Node::GetInstance();
  embb::mtapi::Action action = node.CreateAction(
    HETEROGENEOUS_JOB, InvocableActionFunction);
  embb::mtapi::Job job = node.GetJob(HETEROGENEOUS_JOB);

  Invoke(job, &Invocable1);

  action.Delete();
}
