#include <embb/algorithms/algorithms.h>

/**
 * Example using embb::algorithms::Count and CountIf.
 *
 * Counting elements of a range that fulfill certain properties.
 */
void RunCounting() {
  #include "algorithms/counting/setup-snippet.h"

  #include "algorithms/counting/count-snippet.h"
  assert(count == 2);

  #include "algorithms/counting/count_if-snippet.h"
  assert(count == 6);
}
