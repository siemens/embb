void plugin_task_schedule(embb_mtapi_task_t* local_task) {
  // here the task might be dispatched to some hardware or separate thread

  // mark the task as running
  embb_mtapi_task_set_state(local_task, MTAPI_TASK_RUNNING);

  // nothing to do to execute the no-op task

  // just mark the task as done
  embb_mtapi_task_set_state(local_task, MTAPI_TASK_COMPLETED);
}
