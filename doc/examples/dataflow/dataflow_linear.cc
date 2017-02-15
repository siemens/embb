/*
 * Copyright (c) 2014-2017, Siemens AG. All rights reserved.
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

// snippet_begin:dataflow_include
#include <embb/dataflow/dataflow.h>
// snippet_end

std::stringstream file("hi world!\nhi there!\nhi hi!?\nhi!\n");
std::string what("hi");
std::string with("hello");

// snippet_begin:dataflow_network
typedef embb::dataflow::Network Network;
// snippet_end

// snippet_begin:dataflow_source_function
bool SourceFunction(std::string & str) {
  if (!file.eof()) {
    std::getline(file, str);
    return true;
  } else {
    return false;
  }
}
// snippet_end

// snippet_begin:dataflow_replace_function
void ReplaceFunction(std::string const & istr, std::string & ostr) {
  ostr = istr;
  repl(ostr, what, with);
}
// snippet_end

// snippet_begin:dataflow_sink_function
void SinkFunction(std::string const & str) {
  std::cout << str << std::endl;
}
// snippet_end

void RunDataflowLinear() {
// snippet_begin:dataflow_make
  Network network(4);
// snippet_end
// snippet_begin:dataflow_declare_source
  Network::Source<std::string> read(
    network, embb::base::MakeFunction(SourceFunction)
  );
// snippet_end
// snippet_begin:dataflow_declare_replace
  Network::ParallelProcess<
    Network::Inputs<std::string>,
    Network::Outputs<std::string> > replace(
      network, embb::base::MakeFunction(ReplaceFunction)
    );
// snippet_end
// snippet_begin:dataflow_declare_sink
  Network::Sink<std::string> write(
    network, embb::base::MakeFunction(SinkFunction)
  );
// snippet_end
// snippet_begin:dataflow_connect
  read >> replace >> write;
// snippet_end
// snippet_begin:dataflow_run
  network();
// snippet_end
}
