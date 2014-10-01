  /* delete action */
  mtapi_action_delete(fibonacciAction, 100, &status);
  MTAPI_CHECK_STATUS(status);

  /* finalize the node */
  mtapi_finalize(&status);
  MTAPI_CHECK_STATUS(status);
