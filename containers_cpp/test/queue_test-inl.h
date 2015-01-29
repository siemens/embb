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

#ifndef CONTAINERS_CPP_TEST_QUEUE_TEST_INL_H_
#define CONTAINERS_CPP_TEST_QUEUE_TEST_INL_H_

#include <algorithm>
#include <vector>

#include <embb/base/internal/config.h>

namespace embb {
namespace containers {
namespace test {
template<typename Queue_t, bool MultipleProducers, bool MultipleConsumers>
QueueTest<Queue_t, MultipleProducers, MultipleConsumers>::QueueTest() :
n_threads(static_cast<int>
  (partest::TestSuite::GetDefaultNumThreads())),
  n_iterations(200),
  n_queue_elements_per_thread(100),
  n_queue_elements(n_queue_elements_per_thread*n_threads),
  queueSize(0) {
  CreateUnit("QueueTestSingleThreadEnqueueDequeue").
  Pre(&QueueTest::QueueTestSingleThreadEnqueueDequeue_Pre, this).
  Add(&QueueTest::QueueTestSingleThreadEnqueueDequeue_ThreadMethod, this).
  Post(&QueueTest::QueueTestSingleThreadEnqueueDequeue_Post, this);

  CreateUnit("QueueTestTwoThreadsSingleProducerSingleConsumer").
  Pre(&QueueTest::QueueTestSingleProducedSingleConsumer_Pre, this).
  Add(&QueueTest::QueueTestSingleProducedSingleConsumer_ThreadMethod,
  this,
  2,
  TOTAL_PRODUCE_CONSUME_COUNT).
  Post(&QueueTest::QueueTestSingleProducedSingleConsumer_Post, this);

#ifdef EMBB_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable:4127)
#endif
  if (MultipleProducers == true &&
    MultipleConsumers == true) {
#ifdef EMBB_COMPILER_MSVC
#pragma warning(pop)
#endif
    CreateUnit("QueueTestMultipleThreadsMultipleProducerMultipleConsumer").
    Pre(&QueueTest::QueueTestMultipleProducerMultipleConsumer_Pre, this).
    Add(&QueueTest::QueueTestMultipleProducerMultipleConsumer_ThreadMethod,
    this,
    static_cast<size_t>(n_threads),
    static_cast<size_t>(n_iterations)).
    Post(&QueueTest::QueueTestMultipleProducerMultipleConsumer_Post, this);
  }
}

template<typename Queue_t, bool MultipleProducers, bool MultipleConsumers>
void QueueTest<Queue_t, MultipleProducers, MultipleConsumers>::
QueueTestMultipleProducerMultipleConsumer_Pre() {
  embb_internal_thread_index_reset();
  queue = new Queue_t(static_cast<size_t>(n_queue_elements));

  thread_local_vectors =
    new std::vector<int>[static_cast<unsigned int>(n_threads)];

  for (int i = 0; i != n_threads; ++i) {
    int offset = n_queue_elements_per_thread * 2;

    for (int i2 = 0; i2 != n_queue_elements_per_thread; ++i2) {
      int push_element = i2 + (offset*i);
      thread_local_vectors[i].push_back(push_element);
      expected_queue_elements.push_back(push_element);
    }
  }
}

template<typename Queue_t, bool MultipleProducers, bool MultipleConsumers>
void QueueTest<Queue_t, MultipleProducers, MultipleConsumers>::
QueueTestMultipleProducerMultipleConsumer_Post() {
  std::vector<int> produced;
  for (int i = 0; i != n_threads; ++i) {
    std::vector<int>& loc_elements = thread_local_vectors[i];
    for (std::vector<int>::iterator it = loc_elements.begin();
      it != loc_elements.end();
      ++it) {
      produced.push_back(*it);
    }
  }

  PT_ASSERT(produced.size() == expected_queue_elements.size());

  std::sort(expected_queue_elements.begin(), expected_queue_elements.end());
  std::sort(produced.begin(), produced.end());

  for (unsigned int i = 0;
    i != static_cast<unsigned int>(produced.size()); ++i) {
    PT_ASSERT(expected_queue_elements[i] == produced[i]);
  }

  delete[] thread_local_vectors;
  delete queue;
}

template<typename Queue_t, bool MultipleProducers, bool MultipleConsumers>
void QueueTest<Queue_t, MultipleProducers, MultipleConsumers>::
QueueTestMultipleProducerMultipleConsumer_ThreadMethod() {
  unsigned int thread_index;
  int return_val = embb_internal_thread_index(&thread_index);

  PT_ASSERT(EMBB_SUCCESS == return_val);

  std::vector<int>& my_elements = thread_local_vectors[thread_index];

  for (std::vector<int>::iterator it = my_elements.begin();
    it != my_elements.end();
    ++it) {
    int enq = *it;
    bool success = queue->TryEnqueue(enq);
    PT_ASSERT(success == true);
  }

  my_elements.clear();

  for (int i = 0; i != n_queue_elements_per_thread; ++i) {
    int dequ;
    bool success = queue->TryDequeue(dequ);
    PT_ASSERT(success == true);
    my_elements.push_back(dequ);
  }
}

template<typename Queue_t, bool MultipleProducers, bool MultipleConsumers>
void QueueTest<Queue_t, MultipleProducers, MultipleConsumers>::
QueueTestSingleProducedSingleConsumer_Pre() {
  embb_internal_thread_index_reset();

  queue = new Queue_t(QUEUE_SIZE);
  thread_selector_producer = -1;
  produce_count = 0;
  consume_count = 0;
  consumed_elements.clear();
  produced_elements.clear();
}

template<typename Queue_t, bool MultipleProducers, bool MultipleConsumers>
void QueueTest<Queue_t, MultipleProducers, MultipleConsumers>::
QueueTestSingleProducedSingleConsumer_Post() {
  embb_atomic_memory_barrier();
  ::std::sort(consumed_elements.begin(), consumed_elements.end());
  ::std::sort(produced_elements.begin(), produced_elements.end());

  PT_ASSERT(consumed_elements.size() == produced_elements.size());

  for (unsigned int i = 0;
    i != static_cast<unsigned int>(consumed_elements.size()); i++) {
    PT_ASSERT(consumed_elements[i] == produced_elements[i]);
  }

  delete queue;
}

template<typename Queue_t, bool MultipleProducers, bool MultipleConsumers>
void QueueTest<Queue_t, MultipleProducers, MultipleConsumers>::
QueueTestSingleProducedSingleConsumer_ThreadMethod() {
  unsigned int thread_index;
  int return_val = embb_internal_thread_index(&thread_index);
  PT_ASSERT(return_val == EMBB_SUCCESS);

  if (thread_selector_producer == -1) {
    int expected = -1;
    thread_selector_producer.CompareAndSwap(expected,
      static_cast<int>(thread_index));
    while (thread_selector_producer == -1) {}
  }

  // we are the producer
  if (static_cast<unsigned int>(thread_selector_producer.Load()) ==
    thread_index) {
    while (produce_count >= QUEUE_SIZE) {}

    int random_var = rand() % 10000;
    bool success = queue->TryEnqueue(random_var);
    PT_ASSERT(success == true);
    produce_count++;
    produced_elements.push_back(random_var);
  // we are the consumer
  } else {
    while (consume_count < TOTAL_PRODUCE_CONSUME_COUNT) {
      consume_count++;

      while (produce_count == 0) {}

      int consumed;
      bool success = queue->TryDequeue(consumed);
      PT_ASSERT(success == true);
      produce_count--;
      consumed_elements.push_back(consumed);
    }
  }
}

template<typename Queue_t, bool MultipleProducers, bool MultipleConsumers>
void QueueTest<Queue_t, MultipleProducers, MultipleConsumers>::
QueueTestSingleThreadEnqueueDequeue_ThreadMethod() {
  for (int i = 0; i != QUEUE_SIZE; ++i) {
    bool success = queue->TryEnqueue(i * 133);
    PT_ASSERT(success == true);
  }
  for (int i = 0; i != QUEUE_SIZE; ++i) {
    int dequ = -1;
    bool success = queue->TryDequeue(dequ);
    PT_ASSERT(success == true);
    PT_ASSERT(dequ == i * 133);
  }
}

template<typename Queue_t, bool MultipleProducers, bool MultipleConsumers>
void QueueTest<Queue_t, MultipleProducers, MultipleConsumers>::
QueueTestSingleThreadEnqueueDequeue_Pre() {
  queue = new Queue_t(QUEUE_SIZE);
}
template<typename Queue_t, bool MultipleProducers, bool MultipleConsumers>
void QueueTest<Queue_t, MultipleProducers, MultipleConsumers>::
QueueTestSingleThreadEnqueueDequeue_Post() {
  delete queue;
}
} // namespace test
} // namespace containers
} // namespace embb

#endif  // CONTAINERS_CPP_TEST_QUEUE_TEST_INL_H_
