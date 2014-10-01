#include <embb/algorithms/algorithms.h>
#include <embb/base/c/internal/unused.h>
#include <vector>

void CheckResults(const std::vector<int>& results) {
  assert(results.size() == 5);
  assert(results[0] == 1);
  assert(results[1] == 3);
  assert(results[2] == 6);
  assert(results[3] == 10);
  assert(results[4] == 15);
  EMBB_UNUSED_IN_RELEASE(results);
}

/**
 * Example using embb::algorithms::Scan.
 *
 *
 */
void RunScan() {
  #include "algorithms/scan/setup-snippet.h"

  #include "algorithms/scan/sequential_prefix_sum-snippet.h"
  CheckResults(output_range);

  for (size_t i = 0; i < output_range.size(); i++) output_range[i] = 0;
  #include "algorithms/scan/prefix_sum-snippet.h"
  CheckResults(output_range);
}
