  /* check size of arguments (in this case we only expect one int
     value)*/
  mtapi_status_t status;
  if (arg_size != sizeof(int)) {
    printf("wrong size of arguments\n");
    mtapi_context_status_set(task_context, MTAPI_ERR_ARG_SIZE, &status);
    MTAPI_CHECK_STATUS(status);
    return;
  }
  
  /* cast arguments to the desired type */
  int n = *(int*)args;
