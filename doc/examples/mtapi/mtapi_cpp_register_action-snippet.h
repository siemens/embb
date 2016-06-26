  /* create action */
  embb::mtapi::Action fibonacciAction = node.CreateAction(
    FIBONACCI_JOB,                     /* action ID, defined by the
                                          application */
    (fibonacciActionFunction)          /* action function */
  );

  /* get job */
  fibonacciJob = node.GetJob(FIBONACCI_JOB, THIS_DOMAIN_ID);
