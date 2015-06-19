/*
 * Copyright (c) 2014-2015, Siemens AG. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

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
