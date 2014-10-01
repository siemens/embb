  /* start task */
  int result;
  task = mtapi_task_start(
    MTAPI_TASK_ID_NONE,                /* optional task ID */
    fibonacciJob,                      /* job */
    (void*)&n,                         /* arguments passed to action
                                          functions */
    sizeof(int),                       /* size of arguments */
    (void*)&result,                    /* result buffer */
    sizeof(int),                       /* size of result buffer */
    MTAPI_DEFAULT_TASK_ATTRIBUTES,     /* task attributes */
    MTAPI_GROUP_NONE,                  /* optional task group */
    &status                            /* status out - parameter */
  );
  MTAPI_CHECK_STATUS(status);
