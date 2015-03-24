  int result;
  embb::mtapi::Task task = node.Start(fibonacciJob, &n, &result);
