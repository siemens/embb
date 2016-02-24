  Producer<int>
    producer1(1),
    producer2(2),
    producer3(3),
    producer4(4);

  Network::Source<int>
    source1(
      embb::base::MakeFunction(producer1, &Producer<int>::Run) ),
    source2(
      embb::base::MakeFunction(producer2, &Producer<int>::Run) ),
    source3(
      embb::base::MakeFunction(producer3, &Producer<int>::Run) ),
    source4(
      embb::base::MakeFunction(producer4, &Producer<int>::Run) );

  nw.AddSource(source1);
  nw.AddSource(source2);
  nw.AddSource(source3);
  nw.AddSource(source4);
