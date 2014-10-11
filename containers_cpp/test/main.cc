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

#include <partest/partest.h>
#include <embb/base/thread.h>

#include "./pool_test.h"
#include "./queue_test.h"
#include "./stack_test.h"
#include "./hazard_pointer_test.h"
#include "./object_pool_test.h"
#include <embb/containers/lock_free_tree_value_pool.h>
#include <embb/containers/wait_free_array_value_pool.h>
#include <embb/containers/wait_free_spsc_queue.h>
#include <embb/containers/object_pool.h>
#include <embb/containers/lock_free_stack.h>
#include <embb/containers/lock_free_mpmc_queue.h>

#define COMMA ,

PT_MAIN("Data Structures C++") {
  unsigned int max_threads =
      static_cast<unsigned int>(2 * partest::TestSuite::GetDefaultNumThreads());
  embb_thread_set_max_count(max_threads);

  PT_RUN(embb::containers::test::PoolTest<
    embb::containers::WaitFreeArrayValuePool<int COMMA -1> >);
  PT_RUN(embb::containers::test::PoolTest<
    embb::containers::LockFreeTreeValuePool<int COMMA -1> >);

  PT_RUN(embb::containers::test::HazardPointerTest);

  PT_RUN(embb::containers::test::QueueTest<
    embb::containers::WaitFreeSPSCQueue<int> >);

  PT_RUN(embb::containers::test::QueueTest<
    embb::containers::LockFreeMPMCQueue<int> COMMA true COMMA true >);

  PT_RUN(embb::containers::test::StackTest<
    embb::containers::LockFreeStack<int> >);

  PT_RUN(embb::containers::test::ObjectPoolTest
    <embb::containers::LockFreeTreeValuePool<bool COMMA false > >);

  PT_RUN(embb::containers::test::ObjectPoolTest
    <embb::containers::WaitFreeArrayValuePool<bool COMMA false> >);
}
