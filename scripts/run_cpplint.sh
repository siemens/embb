#!/usr/bin/env bash
# Copyright (c) 2014-2017, Siemens AG. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

usage() {
echo "Check Google C++ Style Guide on project.";
echo "Specify EMBB root directory with -d and the";
echo "cpplint executable with -c. If not specified,";
echo "it is assumed that the cpplint binary is contained";
echo "in the current working directory.";
echo "";
echo "Usage: $0 [-d directory] [-c cpplint_executable] " 1>&2; exit 1;
}

while getopts ":d:c:" o; do
    case "${o}" in
        d)
            d=${OPTARG}
            ;;
        c)
            c=${OPTARG}
            ;;
        *)
            usage
            ;;
    esac
done
shift $((OPTIND-1))

if [ -z "${d}" ]; then
    usage
fi

if [ ! -d "$d" ]; then
  echo "Error: directory $d does not exist or is not a directory!"
  echo ""
  usage
fi

if [ -z "${c}" ]; then
  echo "cpplint binary not specified, assuming to be contained in the current working directory!"
  c="./cpplint.py"
fi

if [ ! -f "${c}" ]; then
  echo "Error: cannot find cpplint script (Location: ${c})"
  echo
  usage
fi

##Excluded Rules
EXCLUDED_RULES="-runtime/references,-runtime/int,-build/include_order,-readability/multiline_comment,-readability/streams,-readability/alt_tokens,-whitespace/comments"
PARENTHESIS_RULE=",-whitespace/parens"
C_CAST_RULE=",-readability/casting"
LONG_LINES_RULE=",-whitespace/line_length"

retval=0

##Excluded files
RAND_FILES=( embb_mtapi_test_group.cc embb_mtapi_test_queue.cc embb_mtapi_test_task.cc queue_test-inl.h )
for project in base_c base_cpp mtapi_c mtapi_plugins_c/mtapi_network_c mtapi_plugins_c/mtapi_opencl_c mtapi_plugins_c/mtapi_cuda_c mtapi_cpp algorithms_cpp containers_cpp dataflow_cpp doc/tutorial/application
do
  echo "-> Doing project: $project"
  dir=$d/$project
  if [ ! -d "$dir" ]; then
    echo "Error: cannot find directory $dir for project $project"
    echo
    usage
  fi
  for file in `find $dir \( -name "*.cc" -o -name "*.h" -o -name "*inl.h" -o -name "*.c" \) -and -not -path "*/src/CL/*"`
  do
    echo "--> Run cpplint on file $file"
    current_rules=$EXCLUDED_RULES
    if [[ $file =~ \.h$ ]]; then
      current_rules+=$PARENTHESIS_RULE
    fi
    if [[ $file =~ \.c$ ]] || [[ $file =~ \mtapi.h$ ]]; then
      current_rules+=$C_CAST_RULE
    fi
    if [[ $file == *atomic* ]]; then
      current_rules+=$LONG_LINES_RULE
    fi
    ############
    #Per file exclusion rules
    if [[ $file == *generate_atomic_implementation_template.h ]]; then
      current_rules+=",-build/header_guard" # This file needs to be included multiple times
    fi
    if [[ $file == *atomic.h ]]; then
      current_rules+=",-whitespace/indent" # Indentation is misinterpreted for this file
    fi
    if [[ $file == *atomic_arithmetic.h ]]; then
      current_rules+=",-readability/function" # All parameters should be named in a function
    fi
    if [[ $file == *object_pool-inl.h ]]; then
      current_rules+=",-readability/function" # All parameters should be named in a function (triggers error with clang if named...)
    fi
    for filename in "${RAND_FILES[@]}"; do
      if [[ $file =~ $filename ]]; then
        current_rules+=",-runtime/threadsafe_fn" # These tests are allowed to use the thread unsafe rand()
      fi
    done
    if [[ $file == */doc/tutorial/application/main.cc ]]; then
      current_rules+=",-build/c++11" # Avoid "<chrono> is an unapproved C++11 header" error
    fi
    python ${c} --extensions=c,cc,h --filter=$current_rules --root="$project/include" --output=vs7 $file
    if [[ ! $? -eq 0 ]]; then
      retval=$((retval+1))
    fi
  done
done
echo "$retval files contained errors."
exit $retval
