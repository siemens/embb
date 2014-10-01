#include <iostream>

#include <embb/mtapi/mtapi.h>

#include "mtapi/mtapi_check_status-snippet.h"

static
#include "mtapi/mtapi_cpp_action_signature-snippet.h"
  /* get the node instance */
#include "mtapi/mtapi_cpp_get_node-snippet.h"
  /* calculate */
#include "mtapi/mtapi_terminating_condition-snippet.h"
    /* first recursive call spawned as task (x = fib(n - 1);) */
#include "mtapi/mtapi_cpp_calc_task-snippet.h"
    /* second recursive call can be called directly (y = fib(n - 2);) */
#include "mtapi/mtapi_cpp_calc_direct-snippet.h"
    /* wait for completion */
#include "mtapi/mtapi_cpp_wait_task-snippet.h"
    /* add the two preceeding numbers */
#include "mtapi/mtapi_write_back-snippet.h"

static
int fibonacci(int n) {
  /* get the node instance, the node is initialized automatically */
  embb::mtapi::Node& node = embb::mtapi::Node::GetInstance();
  /* start calculation */
#include "mtapi/mtapi_cpp_start_task-snippet.h"
  /* wait for task completion */
  mtapi_status_t status = task.Wait(MTAPI_INFINITE);
  MTAPI_CHECK_STATUS(status);

  return result;
}

void RunMTAPI_CPP() {
  int result = fibonacci(6);
  std::cout << "result: " << result << std::endl;
}
