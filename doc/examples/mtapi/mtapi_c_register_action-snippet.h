  /* create action */
  mtapi_action_hndl_t fibonacciAction;
  fibonacciAction = mtapi_action_create(
    FIBONACCI_JOB,                     /* action ID, defined by the
                                          application */
    (fibonacciActionFunction),         /* action function */
    MTAPI_NULL,                        /* no shared data */
    0,                                 /* length of shared data */
    MTAPI_DEFAULT_ACTION_ATTRIBUTES,   /* action attributes */
    &status                            /* status out - parameter */
  );
  MTAPI_CHECK_STATUS(status);

  /* get job */
  mtapi_task_hndl_t task;
  fibonacciJob = mtapi_job_get(FIBONACCI_JOB, THIS_DOMAIN_ID, &status);
  MTAPI_CHECK_STATUS(status);
