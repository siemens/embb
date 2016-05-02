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
