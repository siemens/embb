    int a = n - 1;
    int x;
    embb::tasks::Task task = node.Spawn(
      embb::base::Bind(
        embb::base::MakeFunction(fibonacciActionFunction),
        a,                             /* argument */
        &x,                            /* result */
        embb::base::Placeholder::_1
      )
    );
