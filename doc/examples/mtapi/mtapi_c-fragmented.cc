#include <stdio.h>
#include <embb/mtapi/c/mtapi.h>

#include "mtapi/mtapi_check_status-snippet.h"
#include "mtapi/mtapi_c_domain_node_id-snippet.h"

static
#include "mtapi/mtapi_c_action_signature-snippet.h"
#include "mtapi/mtapi_c_validate_arguments-snippet.h"
#include "mtapi/mtapi_c_validate_result_buffer-snippet.h"
  /* calculate */
#include "mtapi/mtapi_terminating_condition-snippet.h"
    /* first recursive call spawned as task (x = fib(n - 1);) */
#include "mtapi/mtapi_c_calc_task-snippet.h"
    /* second recursive call can be called directly (y = fib(n - 2);) */
#include "mtapi/mtapi_c_calc_direct-snippet.h"
    /* wait for completion */
#include "mtapi/mtapi_c_wait_task-snippet.h"
    /* add the two preceeding numbers */
#include "mtapi/mtapi_write_back-snippet.h"

static int fibonacci(int n) {
#include "mtapi/mtapi_c_initialize-snippet.h"
#include "mtapi/mtapi_c_register_action-snippet.h"
#include "mtapi/mtapi_c_start_task-snippet.h"
  /* wait for task completion */
  mtapi_task_wait(task, MTAPI_INFINITE, &status);
  MTAPI_CHECK_STATUS(status);
#include "mtapi/mtapi_c_finalize-snippet.h"
  return result;
}

void RunMTAPI_C() {
  /* run calculation */
  int result;
  result = fibonacci(6);

  /* print result */
  printf("result: %i\n", result);
}
