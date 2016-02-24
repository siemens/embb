  /* create action */
  embb::mtapi::Action fibonacciAction(
    FIBONACCI_JOB,                     /* action ID, defined by the
                                          application */
    (fibonacciActionFunction)          /* action function */
  );

  /* get job */
  fibonacciJob = embb::mtapi::Job(FIBONACCI_JOB, THIS_DOMAIN_ID);
