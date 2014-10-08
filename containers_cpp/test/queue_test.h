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

#ifndef CONTAINERS_CPP_TEST_QUEUE_TEST_H_
#define CONTAINERS_CPP_TEST_QUEUE_TEST_H_

#include <vector>
#include <partest/partest.h>
#include <embb/base/duration.h>
#include <embb/containers/wait_free_spsc_queue.h>

namespace embb {
namespace containers {
namespace test {
template<typename Queue_t,
  bool MultipleProducers = false,
  bool MultipleConsumers = false>
class QueueTest : public partest::TestCase {
 private:
  static const int QUEUE_SIZE = 100;
  static const int TOTAL_PRODUCE_CONSUME_COUNT = 10000;
  int n_threads;
  embb::base::Atomic<int> thread_selector_producer;
  embb::base::Atomic<int> produce_count;
  std::vector<int> consumed_elements;
  std::vector<int> produced_elements;

  //for multiple p/c
  int n_iterations;
  int n_queue_elements_per_thread;
  int n_queue_elements;
  std::vector<int> expected_queue_elements;
  std::vector<int>* thread_local_vectors;
  embb::base::Atomic<int> queueSize;

  int consume_count;
  Queue_t* queue;

  void QueueTestMultipleProducerMultipleConsumer_Pre();
  void QueueTestMultipleProducerMultipleConsumer_Post();
  void QueueTestMultipleProducerMultipleConsumer_ThreadMethod();
  void QueueTestSingleProducedSingleConsumer_Pre();
  void QueueTestSingleProducedSingleConsumer_Post();
  void QueueTestSingleProducedSingleConsumer_ThreadMethod();
  void QueueTestSingleThreadEnqueueDequeue_ThreadMethod();
  void QueueTestSingleThreadEnqueueDequeue_Pre();
  void QueueTestSingleThreadEnqueueDequeue_Post();

 public:
  QueueTest();
};
} // namespace test
} // namespace containers
} // namespace embb

#include "./queue_test-inl.h"

#endif  // CONTAINERS_CPP_TEST_QUEUE_TEST_H_
