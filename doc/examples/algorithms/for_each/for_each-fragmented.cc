#include <embb/algorithms/algorithms.h>
#include <embb/base/c/internal/unused.h>
#include <vector>

static void CheckResults(const std::vector<int>& range) {
  int i = 1;
  for (const int& value : range) {
    assert(value == i * 2);
    EMBB_UNUSED_IN_RELEASE(value);
    i++;
  }
}

/**
 * Example using embb::algorithms::ForEach.
 *
 * Traversing and modifying a sequence with a for-each loop.
 */
void RunForEach() {
  #include "stl_for_each/setup-snippet.h"
  #include "algorithms/for_each/doubling-snippet.h"
  CheckResults(range);

  #include "algorithms/for_each/setup_zip-snippet.h"
  #include "algorithms/for_each/doubling_zip-snippet.h"
  CheckResults(output_range);
}
