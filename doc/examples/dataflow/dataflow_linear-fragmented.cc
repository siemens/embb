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
