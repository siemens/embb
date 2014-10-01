#include <embb/algorithms/algorithms.h>
#include <cassert>

#include "algorithms/invoke/packages-snippet.h"

static int a = 0, b = 0, c = 0;

void WorkPackageA() {
  a++;
}

void WorkPackageB() {
  b++;
}

void WorkPackageC() {
  c++;
}

int* Partition(int* first, int* last) {
  int* pivot = last - 1;
  while (first != last) {
    while (*first < *pivot) {
      ++first;
      if (first == last) return first;
    }
    do {
      --last;
      if (first == last) return first;
    } while (*pivot < *last);
    std::swap(*first, *last);
    if(pivot == first) {
      pivot = last;
    } else if (pivot == last) {
      pivot = first;
    }
    ++first;
  }
  return first;
}

#include "algorithms/invoke/quick_sort-snippet.h"

#include "algorithms/invoke/parallel_quick_sort-snippet.h"

/**
 * Example using embb::algorithms::ParallelInvoke() to execute work packages in
 * parallel.
 */
void RunInvoke() {
  #include "algorithms/invoke/invocation-snippet.h"
  assert(a == 1);
  assert(b == 1);
  assert(c == 1);

  {
    int range[] = {2, 5, 3, 1, 4};
    QuickSort(range, range + 5);
    for (size_t i = 0; i < 5; i++) {
      assert(range[i] == static_cast<int>(i) + 1);
    }
  }
  {
    int range[] = {2, 5, 3, 1, 4};
    ParallelQuickSort(range, range + 5);
    for (size_t i = 0; i < 5; i++) {
      assert(range[i] == static_cast<int>(i) + 1);
    }
  }
}



