  /* if the caller is not interested in results, result_buffer may be
     MTAPI_NULL. Of course, this depends on the application */
  int* result = MTAPI_NULL;
  if (result_buffer == MTAPI_NULL) {
    mtapi_context_status_set(
      task_context, MTAPI_ERR_RESULT_SIZE, &status);
    MTAPI_CHECK_STATUS(status);
  } else {
    /* if results are expected by the caller, check result buffer
       size... */
    if (result_buffer_size == sizeof(int)) {
      /* ... and cast the result buffer */
      result = (int*)result_buffer;
    } else {
      printf("wrong size of result buffer\n");
      mtapi_context_status_set(
        task_context, MTAPI_ERR_RESULT_SIZE, &status);
      MTAPI_CHECK_STATUS(status);
      return;
    }
  }
