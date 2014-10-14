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

#include "dataflow/dataflow_include-snippet.h"

#include "dataflow/dataflow_producer-snippet.h"
#include "dataflow/dataflow_comparator-snippet.h"
#include "dataflow/dataflow_consumer-snippet.h"

#include "dataflow/dataflow_network-snippet.h"

void RunDataflowNonLinear() {
#include "dataflow/dataflow_declare_add_sources-snippet.h"

  Comparator<int> comparator;

  Network::ParallelProcess<
    Network::Inputs<int, int>::Type, Network::Outputs<int, int>::Type>
    process1( embb::base::MakeFunction(comparator, &Comparator<int>::Run) ),
    process2( embb::base::MakeFunction(comparator, &Comparator<int>::Run) ),
    process3( embb::base::MakeFunction(comparator, &Comparator<int>::Run) ),
    process4( embb::base::MakeFunction(comparator, &Comparator<int>::Run) ),
    process5( embb::base::MakeFunction(comparator, &Comparator<int>::Run) );

  nw.Add(process1);
  nw.Add(process2);
  nw.Add(process3);
  nw.Add(process4);
  nw.Add(process5);

  Consumer<int> consumer;

  Network::Sink<int, int, int, int>
    sink1(embb::base::MakeFunction(consumer, &Consumer<int>::Run));

  nw.Add(sink1);

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

  nw();
}
