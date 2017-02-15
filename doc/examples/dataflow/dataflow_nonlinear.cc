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

#include <algorithm>
#include <iostream>

#ifdef NDEBUG
#undef assert
#define assert(x) if(!x) std::cout << "assert failed." << std::endl
#endif

static int SimpleRand(int & seed) {
  const int a = 16807;
  const int m = 2147483647;
  seed = (a * seed) % m;
  return seed % 100;
}

#include <embb/dataflow/dataflow.h>

// snippet_begin:dataflow_producer
template <typename T>
class Producer {
 public:
  explicit Producer(int seed) : seed_(seed), count_(4) {}
  bool Run(T& x) {
    if (count_ >= 0) {
      // produce a new value x
      x = SimpleRand(seed_);
      count_--;
      return true;
    } else {
      return false;
    }
  }

 private:
  int seed_;
  int count_;
};
// snippet_end

// snippet_begin:dataflow_comparator
template <typename T>
class Comparator {
public:
  void Run(const T& a, const T& b, T& x, T& y) {
    x = std::min(a,b);
    y = std::max(a,b);
  }
};
// snippet_end

// snippet_begin:dataflow_consumer
template <typename T>
class Consumer {
public:
  void Run(const T& x1, const T& x2, const T& x3, const T& x4) {
    if (x1 <= x2 && x2 <= x3 && x3 <= x4) {
      // consume values
    }
  }
};
// snippet_end

typedef embb::dataflow::Network Network;

void RunDataflowNonLinear() {
  Network network(4);

  // snippet_begin:dataflow_declare_add_sources
  Producer<int>
    producer1(1),
    producer2(2),
    producer3(3),
    producer4(4);

  Network::Source<int>
    source1(
      network,
      embb::base::MakeFunction(producer1, &Producer<int>::Run) ),
    source2(
      network,
      embb::base::MakeFunction(producer2, &Producer<int>::Run) ),
    source3(
      network,
      embb::base::MakeFunction(producer3, &Producer<int>::Run) ),
    source4(
      network,
      embb::base::MakeFunction(producer4, &Producer<int>::Run) );
  // snippet_end

  Comparator<int> comparator;

  Network::ParallelProcess<
    Network::Inputs<int, int>, Network::Outputs<int, int> >
    process1(network,
      embb::base::MakeFunction(comparator, &Comparator<int>::Run)),
    process2(network,
      embb::base::MakeFunction(comparator, &Comparator<int>::Run)),
    process3(network,
      embb::base::MakeFunction(comparator, &Comparator<int>::Run)),
    process4(network,
      embb::base::MakeFunction(comparator, &Comparator<int>::Run)),
    process5(network,
      embb::base::MakeFunction(comparator, &Comparator<int>::Run));

  Consumer<int> consumer;

  Network::Sink<int, int, int, int>
    sink1(network, embb::base::MakeFunction(consumer, &Consumer<int>::Run));

  source1.GetOutput<0>() >> process1.GetInput<0>();
  source2.GetOutput<0>() >> process2.GetInput<0>();
  source3.GetOutput<0>() >> process1.GetInput<1>();
  source4.GetOutput<0>() >> process2.GetInput<1>();

  process1.GetOutput<0>() >> process3.GetInput<0>();
  process2.GetOutput<0>() >> process3.GetInput<1>();
  process1.GetOutput<1>() >> process4.GetInput<0>();
  process2.GetOutput<1>() >> process4.GetInput<1>();

  process3.GetOutput<1>() >> process5.GetInput<0>();
  process4.GetOutput<0>() >> process5.GetInput<1>();

  process3.GetOutput<0>() >> sink1.GetInput<0>();
  process5.GetOutput<0>() >> sink1.GetInput<1>();
  process5.GetOutput<1>() >> sink1.GetInput<2>();
  process4.GetOutput<1>() >> sink1.GetInput<3>();

  network();
}
