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

#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>

// replace all occurrences of 'what' in 'str' with 'with'
void repl(std::string& str, const std::string &what,
  const std::string& with) {
  std::string::size_type pos = 0;
  while ((pos = str.find(what, pos)) != std::string::npos) {
    str.replace(pos, what.length(), with);
    pos += with.length();
  }
}

#include "dataflow/dataflow_include-snippet.h"

std::stringstream file("hi world!\nhi there!\nhi hi!?\nhi!\n");
std::string what("hi");
std::string with("hello");

#include "dataflow/dataflow_network-snippet.h"

#include "dataflow/dataflow_source_function-snippet.h"
#include "dataflow/dataflow_replace_function-snippet.h"
#include "dataflow/dataflow_sink_function-snippet.h"

void RunDataflowLinear() {
#include "dataflow/dataflow_declare_source-snippet.h"
#include "dataflow/dataflow_declare_replace-snippet.h"
#include "dataflow/dataflow_declare_sink-snippet.h"
#include "dataflow/dataflow_connect-snippet.h"
#include "dataflow/dataflow_add-snippet.h"
#include "dataflow/dataflow_run-snippet.h"
}
