#include <stdlib.h>
#include <embb/base/c/log.h>
#define MTAPI_CHECK_STATUS(status) \
if (MTAPI_SUCCESS != status) { \
  embb_log_error("MTAPI C Example", "...error %d\n\n", status); \
  abort(); \
}
