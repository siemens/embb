#include <embb/algorithms/algorithms.h>
#include <embb/base/c/internal/unused.h>
#include <vector>
#include <cassert>

/**
 * Example using embb::algorithms::Reduce.
 *
 * Summing up a range of values and more.
 */
void RunReduce() {
  #include "algorithms/reduce/range_init-snippet.h"

  #include "algorithms/reduce/sequential-snippet.h"
  assert(sum == 1 + 2 + 3 + 4 + 5);

  #include "algorithms/reduce/parallel-snippet.h"
  assert(sum == 1 + 2 + 3 + 4 + 5);

  #include "algorithms/reduce/second_range_init-snippet.h"
  #include "algorithms/reduce/dot_product-snippet.h"
  assert(dot_product == 1*5 + 2*6 + 3*7 + 4*8 + 5*9);
  EMBB_UNUSED_IN_RELEASE(dot_product);
}
