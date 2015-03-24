  int result;
  embb::tasks::Task task = node.Spawn(
    embb::base::Bind(
      embb::base::MakeFunction(fibonacciActionFunction),
      n,
      &result,
      embb::base::Placeholder::_1
    )
  );
