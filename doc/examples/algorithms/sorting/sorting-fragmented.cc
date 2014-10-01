#include <embb/algorithms/algorithms.h>
#include <vector>

/**
 * Example using embb::algorithms::QuickSort and MergeSort.
 *
 * Sorting a range of values.
 */
void RunSorting() {
  #include "algorithms/sorting/range_define-snippet.h"
  range = {4, 2, 3, 5, 1};

  #include "algorithms/sorting/quick_sort-snippet.h"
  for (size_t i = 0; i < range.size(); i++) {
    assert(range[i] == static_cast<int>(i) + 1);
  }

  #include "algorithms/sorting/quick_sort_custom_compare-snippet.h"
  for (size_t i = 0; i < range.size(); i++) {
    assert(range[i] == static_cast<int>(range.size() - i));
  }

  #include "algorithms/sorting/merge_sort_preallocated-snippet.h"
  for (size_t i = 0; i < range.size(); i++) {
    assert(range[i] == static_cast<int>(i) + 1);
  }
}



