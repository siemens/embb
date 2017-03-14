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

#ifndef CONTAINERS_CPP_TEST_QUEUE_TEST_INL_H_
#define CONTAINERS_CPP_TEST_QUEUE_TEST_INL_H_

#include <algorithm>
#include <vector>

namespace embb {
namespace containers {
namespace test {
template<typename Queue_t, bool MultipleProducers, bool MultipleConsumers>
QueueTest<Queue_t, MultipleProducers, MultipleConsumers>::QueueTest() :
  n_threads(static_cast<int>(partest::TestSuite::GetDefaultNumThreads())),
  n_queue_size(
    static_cast<int>(partest::TestSuite::GetDefaultNumIterations()) *
    MIN_TOTAL_PRODUCE_CONSUME_COUNT),
  n_total_produce_consume_count(n_queue_size),
  n_producers(1),
  n_consumers(1),
  next_producer_id(0),
  next_consumer_id(0),
  n_producer_elements(
    static_cast<int>(partest::TestSuite::GetDefaultNumIterations() *
    MIN_ENQ_ELEMENTS)),
  consume_count(0),
  queue(NULL) {
  CreateUnit("QueueTestSingleThreadEnqueueDequeue").
  Pre(&QueueTest::QueueTestSingleThreadEnqueueDequeue_Pre, this).
  Add(&QueueTest::QueueTestSingleThreadEnqueueDequeue_ThreadMethod, this).
  Post(&QueueTest::QueueTestSingleThreadEnqueueDequeue_Post, this);
  CreateUnit("QueueTestTwoThreadsSingleProducerSingleConsumer").
  Pre(&QueueTest::QueueTestSingleProducerSingleConsumer_Pre, this).
  Add(&QueueTest::QueueTestSingleProducerSingleConsumer_ThreadMethod,
    this,
    2,
    static_cast<size_t>(n_total_produce_consume_count)).
  Post(&QueueTest::QueueTestSingleProducerSingleConsumer_Post, this);

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4127)
#endif
  if (MultipleProducers == true && MultipleConsumers == true) {
    // MP/MC
    n_producers = n_threads / 2;
    n_consumers = n_threads / 2;
#ifdef _MSC_VER
#pragma warning(pop)
#endif
    CreateUnit("QueueTestOrderMultipleProducerMultipleConsumer").
      Pre(&QueueTest::QueueTestOrderMPMC_Pre, this).
      Add(&QueueTest::QueueTestOrderMPMC_ConsumerThreadMethod,
      this,
      static_cast<size_t>(n_consumers),
      static_cast<size_t>(1)).
      Add(&QueueTest::QueueTestOrderMPMC_ProducerThreadMethod,
      this,
      static_cast<size_t>(n_producers),
      static_cast<size_t>(1)).
      Post(&QueueTest::QueueTestOrderMPMC_Post, this);
  }
}

template<typename Queue_t, bool MultipleProducers, bool MultipleConsumers>
void QueueTest<Queue_t, MultipleProducers, MultipleConsumers>::
QueueTestOrderMPMC_Pre() {
  queue = new Queue_t(static_cast<size_t>(n_producer_elements));
  embb_internal_thread_index_reset();
  next_producer_id = 0;
  next_consumer_id = 0;
  consumers.clear();
  producers.clear();
  for (size_t p = 0; p < static_cast<size_t>(n_producers); ++p) {
    producers.push_back(Producer(queue, p, n_producer_elements));
  }
  for (size_t c = 0; c < static_cast<size_t>(n_consumers); ++c) {
    consumers.push_back(Consumer(queue, n_producers, n_producer_elements));
  }
}

template<typename Queue_t, bool MultipleProducers, bool MultipleConsumers>
void QueueTest<Queue_t, MultipleProducers, MultipleConsumers>::
QueueTestOrderMPMC_Post() {
  delete queue;
  // Tally for all elements enqueued by all producers,
  // initialized with all 0:
  ::std::vector<unsigned char> total_tally;
  size_t n_elements_total =
    static_cast<size_t>(n_producers * n_producer_elements);
  for (size_t i = 0; i < n_elements_total / 8; ++i) {
    total_tally.push_back(0);
  }
  // Collect all dequeued element flags from consumers:
  for (size_t c = 0; c < static_cast<size_t>(n_consumers); ++c) {
    for (size_t e = 0; e < n_elements_total / 8; ++e) {
      total_tally[e] |= consumers[c].Tally()[e];
    }
  }
  // Test if all elements have been dequeued by any
  // consumer.
  // To avoid static cast warning:
  for (size_t t = 0;
       t < static_cast<size_t>(n_producers * n_producer_elements / 8);
       ++t) {
    PT_ASSERT_EQ_MSG(total_tally[t], 0xff,
      "missing dequeued elements");
  }
}

template<typename Queue_t, bool MultipleProducers, bool MultipleConsumers>
void QueueTest<Queue_t, MultipleProducers, MultipleConsumers>::
QueueTestOrderMPMC_ProducerThreadMethod() {
  size_t p_id = next_producer_id.FetchAndAdd(1);
  producers[p_id].Run();
}

template<typename Queue_t, bool MultipleProducers, bool MultipleConsumers>
void QueueTest<Queue_t, MultipleProducers, MultipleConsumers>::
QueueTestOrderMPMC_ConsumerThreadMethod() {
  size_t c_id = next_consumer_id.FetchAndAdd(1);
  consumers[c_id].Run();
}

template<typename Queue_t, bool MultipleProducers, bool MultipleConsumers>
void QueueTest<Queue_t, MultipleProducers, MultipleConsumers>::Producer::
Run() {
  // Enqueue pairs of (producer id, counter):
  for (int i = 0; i < n_producer_elements; ++i) {
    while (!q->TryEnqueue(element_t(producer_id, i))) {
      embb::base::Thread::CurrentYield();
    }
  }
  // Enqueue -1 as terminator element of this producer:
  while (!q->TryEnqueue(element_t(producer_id, -1))) {
    embb::base::Thread::CurrentYield();
  }
}

template<typename Queue_t, bool MultipleProducers, bool MultipleConsumers>
QueueTest<Queue_t, MultipleProducers, MultipleConsumers>::Consumer::
Consumer(Queue_t * const queue, int numProducers, int numProducerElements) :
  q(queue),
  n_producers(numProducers),
  n_producer_elements(numProducerElements) {
  for (int p_id = 0; p_id < n_producers; ++p_id) {
    // Initialize last value dequeued from producers with
    // below-minimum value:
    sequence_number.push_back(-1);
    // Initialize element tally for producer with all 0,
    // 8 flags / char:
    for (int i = 0; i < n_producer_elements / 8; ++i) {
      consumer_tally.push_back(0);
    }
  }
}

template<typename Queue_t, bool MultipleProducers, bool MultipleConsumers>
void QueueTest<Queue_t, MultipleProducers, MultipleConsumers>::Consumer::
Run() {
  element_t element;
  size_t producerId;
  // To avoid compiler warning
  bool forever = true;
  while (forever) {
    if (!q->TryDequeue(element)) {
      continue;
    }
    if (element.second < 0) {
      break;
    }
    producerId = element.first;
    // Assert on dequeued element:
    PT_ASSERT_LT_MSG(producerId, static_cast<size_t>(n_producers),
      "Invalid producer id in dequeue");
    PT_ASSERT_LT_MSG(sequence_number[producerId], element.second,
      "Invalid element sequence");
    // Store last value received from the element's producer:
    sequence_number[producerId] = element.second;
    const size_t pos((producerId * n_producer_elements) +
      static_cast<size_t>(element.second));
    // Test dequeued element's position flag: tally[pos] == 1
    PT_ASSERT_EQ_MSG(consumer_tally[pos / 8] & (0x80 >> (pos % 8)), 0,
      "Element dequeued twice");
    // Set flag at dequeued element's position:
    // tally[pos] = 1
    consumer_tally[pos / 8] |= (0x80 >> (pos % 8));
  }
}

template<typename Queue_t, bool MultipleProducers, bool MultipleConsumers>
void QueueTest<Queue_t, MultipleProducers, MultipleConsumers>::
QueueTestSingleProducerSingleConsumer_Pre() {
  embb_internal_thread_index_reset();
  queue = new Queue_t(static_cast<size_t>(n_queue_size));
  thread_selector_producer = -1;
  produce_count = 0;
  consume_count = 0;
  consumed_elements.clear();
  produced_elements.clear();
}

template<typename Queue_t, bool MultipleProducers, bool MultipleConsumers>
void QueueTest<Queue_t, MultipleProducers, MultipleConsumers>::
QueueTestSingleProducerSingleConsumer_Post() {
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
QueueTestSingleProducerSingleConsumer_ThreadMethod() {
  unsigned int thread_index;
  int return_val = embb_internal_thread_index(&thread_index);
  PT_ASSERT(return_val == EMBB_SUCCESS);
  if (thread_selector_producer == -1) {
    int expected = -1;
    thread_selector_producer.CompareAndSwap(expected,
      static_cast<int>(thread_index));
    while (thread_selector_producer == -1) {}
  }
  if (static_cast<unsigned int>(thread_selector_producer.Load()) ==
    thread_index) {
    // we are the producer
    while (produce_count >= n_queue_size) { }

    element_t random_var(0, rand() % 10000);
    bool success = queue->TryEnqueue(random_var);
    PT_ASSERT(success == true);
    produce_count++;
    produced_elements.push_back(random_var);
  } else {
    // we are the consumer
    while (consume_count < n_total_produce_consume_count) {
      consume_count++;
      while (produce_count == 0) {}

      element_t consumed;
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
  // Enqueue the expected amount of elements
  for (int i = 0; i != n_queue_size; ++i) {
    bool success = queue->TryEnqueue(element_t(0, i * 133));
    PT_ASSERT(success == true);
  }

  // Some queues may allow enqueueing more elements than their capacity
  // permits, so try to enqueue additional elements until the queue is full
  int oversized_count = n_queue_size;
  while ( queue->TryEnqueue(element_t(0, oversized_count * 133)) ) {
    ++oversized_count;
  }
  // Oversized amount should not be larger than the original capacity
  PT_ASSERT_LT(oversized_count, 2 * n_queue_size);

  // Dequeue the expected amount of elements
  for (int i = 0; i != n_queue_size; ++i) {
    element_t dequ(0, -1);
    bool success = queue->TryDequeue(dequ);
    PT_ASSERT(success == true);
    PT_ASSERT(dequ.second == i * 133);
  }

  // Dequeue any elements enqueued above the original capacity
  for (int i = n_queue_size; i != oversized_count; ++i) {
    element_t dequ(0, -1);
    bool success = queue->TryDequeue(dequ);
    PT_ASSERT(success == true);
    PT_ASSERT(dequ.second == i * 133);
  }

  // Ensure the queue is now empty
  {
    element_t dequ;
    bool success = queue->TryDequeue(dequ);
    PT_ASSERT(success == false);
  }
}

template<typename Queue_t, bool MultipleProducers, bool MultipleConsumers>
void QueueTest<Queue_t, MultipleProducers, MultipleConsumers>::
QueueTestSingleThreadEnqueueDequeue_Pre() {
  queue = new Queue_t(static_cast<size_t>(n_queue_size));
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
