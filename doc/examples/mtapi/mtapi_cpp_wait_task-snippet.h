    mtapi_status_t task_status = task.Wait(MTAPI_INFINITE);
    if (task_status != MTAPI_SUCCESS) {
      printf("task failed with error: %d\n\n", task_status);
      exit(task_status);
    }
