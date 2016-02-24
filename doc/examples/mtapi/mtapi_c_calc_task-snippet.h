    int a = n - 1;
    int x;
    mtapi_task_hndl_t task = mtapi_task_start(
      MTAPI_TASK_ID_NONE,              /* optional task ID */
      fibonacciJob,                    /* job */
      (void*)&a,                       /* arguments passed to action
                                          functions */
      sizeof(int),                     /* size of arguments */
      (void*)&x,                       /* result buffer */
      sizeof(int),                     /* size of result buffer */
      MTAPI_DEFAULT_TASK_ATTRIBUTES,   /* task attributes */
      MTAPI_GROUP_NONE,                /* optional task group */
      &status                          /* status out - parameter */
    );
    MTAPI_CHECK_STATUS(status);
