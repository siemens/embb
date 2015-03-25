    mtapi_status_t status = task.Wait(MTAPI_INFINITE);
    if (status != MTAPI_SUCCESS) {
      printf("task failed with error: %d\n\n", status);
      exit(status);
    }
