#!/usr/bin/env bash
# Copyright (c) 2014-2015, Siemens AG. All rights reserved.
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
echo "Specify license file to add to each source file";
echo "and directory where to start to find files.";
echo "Example call:";
echo "./insert_license.sh -d ../ -l license.txt";
echo "";
echo "Usage: $0 [-d directory] [-l license_file]" 1>&2; exit 1;
}

while getopts ":d:l:" o; do
    case "${o}" in
        d)
            d=${OPTARG}
            ;;
        l)
            l=${OPTARG}
            ;;
        *)
            usage
            ;;
    esac
done
shift $((OPTIND-1))

if [ -z "${d}" ] || [ -z "${l}" ]; then
    usage
fi

if [ ! -d "$d" ]; then
  echo "Error, directory $d does not exist or is not a directory!" 
  echo "" 
  usage
fi

if [ ! -f "$l" ]; then
  echo "Error, license file $l does not exist or is not a file!" 
  echo "" 
  usage
fi

for FILE in `find ${d} -name "*.h" -o -name "*.cc" -o -name "*.c" -o -name "*.h.in" -type f`
do
if ! grep -q COPYRIGHT $FILE
  then
    echo "Inserting license to: $FILE"
    cat ${l} $FILE > $FILE.new && mv $FILE.new $FILE
fi
done
