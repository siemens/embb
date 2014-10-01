#include <embb/base/c/internal/unused.h>
#include <vector>
#include <algorithm>
#include <cassert>

#include "stl_for_each/function_define-snippet.h"

#include "stl_for_each/functor_define-snippet.h"

static void CheckResults(const std::vector<int>& range) {
  int i = 1;
  for (const int& value : range) {
    assert(value == i * 2);
    EMBB_UNUSED_IN_RELEASE(value);
    i++;
  }
}

/**
 * Example using embb::patterns::ForEach.
 *
 * Traversing and modifying a sequence with a for-each loop.
 */

void RunSTLForEach() {
  {
    #include "stl_for_each/setup-snippet.h"
    #include "stl_for_each/manual-snippet.h"
    CheckResults(range);
  }

  {
    #include "stl_for_each/setup-snippet.h"
    #include "stl_for_each/function-snippet.h"
    CheckResults(range);
  }

  {
    #include "stl_for_each/setup-snippet.h"
    #include "stl_for_each/functor-snippet.h"
    CheckResults(range);
  }

  {
    #include "stl_for_each/setup-snippet.h"
    #include "stl_for_each/lambda-snippet.h"
    CheckResults(range);
  }
}
