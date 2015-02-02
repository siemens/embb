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
echo "Create a tarball of the project. Specify the project root with";
echo "-d parameter.";
echo "";
echo "Version number and project name is automatically derived from CMakeLists.txt in the project root.";
echo "Tarball that will be created: [PROJECT_NAME]_[VERSION_NUMBER].tar.gz";
echo "Example call (from the scripts directory as working directory):";
echo "$0 -d ../";
echo "";
echo "Usage: $0 [-d <root project dir>]" 1>&2; exit 1;
}


while getopts ":d:" o; do
    case "${o}" in
        d)
            d=${OPTARG}
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
  echo "Error, directory $d does not exist or is not a directory!" 
  echo "" 
  usage
fi

CMAKEFILE="$d/CMakeLists.txt"

if [ ! -f "$CMAKEFILE" ]; then
  echo "Error, could no locate CMakeLists.txt"
  echo ""
  usage
fi

#derive version number from cmake script
VERSION_MAJOR=`cat $CMAKEFILE | grep EMBB_BASE_VERSION_MAJOR | sed "s/^[^0-9]*\([0-9]\+\)[^0-9]*$/\1/g"`
VERSION_MINOR=`cat $CMAKEFILE | grep EMBB_BASE_VERSION_MINOR | sed "s/^[^0-9]*\([0-9]\+\)[^0-9]*$/\1/g"`
VERSION_PATCH=`cat $CMAKEFILE | grep EMBB_BASE_VERSION_PATCH | sed "s/^[^0-9]*\([0-9]\+\)[^0-9]*$/\1/g"`
VERSION_NUMBER="${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}"
PROJECT_NAME=`cat $CMAKEFILE | grep project.*\(.*\) | sed "s/^[^(]*(\([^)]*\)).*$/\1/g" | tr '[:upper:]' '[:lower:]'`
n="${PROJECT_NAME}-${VERSION_NUMBER}"
TARBALL_NAME="${n}.tar.gz"

if ! [[ $TARBALL_NAME =~ ^[a-zA-Z0-9|\.|\_|-]+$ ]]; then
    echo "Want to create tarball with name $TARBALL_NAME." >&2
    echo 'Filename not valid, only a-z, A-Z, .,- and _ characters are allowed' >&2 # write to stderr
    exit 1
fi

echo "Do you wish to create a tarball with the name $TARBALL_NAME in the current directory?"
select yn in "Yes" "No"; do
    case $yn in
        Yes ) break;;
        No ) echo Leaving tarball creation; exit;;
    esac
done

if [ -f "$TARBALL_NAME" ]; then
  echo "File $TARBALL_NAME exists. Delete file?"
  select yn in "Yes" "No"; do
      case $yn in
        Yes ) break;;
        No ) echo Leaving tarball creation; exit;;
      esac
  done
  rm $TARBALL_NAME
  if [ -f "$TARBALL_NAME" ]; then
    echo "Could not delete $TARBALL_NAME"
    exit
  fi
fi



MYTMPDIR=`mktemp -d`
echo "Using temporary directory $MYTMPDIR"
trap "rm -rf $MYTMPDIR" EXIT

echo "Calling rsync to temporary folder"

rsync \
 --exclude ".git" \
 --exclude ".gitignore" \
 --exclude "build*/" \
 --exclude "scripts/*.tar.gz" \
 --exclude "scripts/cpplint.py" \
 --exclude "scripts/create_tarball.sh" \
 --exclude "scripts/insert_license.sh" \
 --exclude "scripts/license.*" \
 --exclude "scripts/license_*" \
 --exclude "scripts/remove_license.sh" \
 --exclude "mtapi/MTAPI.mm" \
 --exclude ".cproject" \
 --exclude ".gitattributes" \
 --exclude ".project" \
 --exclude "*.blg" \
 --exclude "*.fls" \
 --exclude "*.bbl" \
 --exclude "*.fdb_latexmk" \
 --exclude "*.log" \
 --exclude "*.out" \
 --exclude "*.toc" \
 --exclude "*.aux" \
 --exclude "doc/tutorial/sty" \
 --exclude "doc/tutorial/pics" \
 --exclude "doc/tutorial/content" \
 --exclude "doc/tutorial/*.tex" \
 --exclude "doc/tutorial/*.bib" \
 --exclude "doc/reference/*.xml" \
 --exclude "doc/reference/*.dox" \
 --exclude "doc/reference/*.in" \
 --exclude "doc/reference/header.html" \
 --exclude "doc/reference/*.css" \
 --exclude "doc/examples/insert_snippets.py" \
 --exclude ".travis.yml" \
 --archive --recursive ${d} $MYTMPDIR/${n}
 
echo "Replace version number in README"

README_FILE="$MYTMPDIR/${n}/README.md"

if [ -f $README_FILE ]; then
  sed -i "s/\[VERSION_NUMBER_TEMPLATE\]/$VERSION_NUMBER/g" $README_FILE
fi

echo "Checking line endings"

WINLINES=`find $MYTMPDIR/${n} -not -type d -exec file "{}" ";" | grep CRLF`

if [ -n "$WINLINES" ]; then
  echo "Detected Dos line endings in following files:"
  echo "$WINLINES"
  echo "Warning: The project guidelines forbid Dos line endings. Continue creating tarball?"
  select yn in "Yes" "No"; do
      case $yn in
        Yes ) break;;
        No ) echo Leaving tarball creation; exit;;
      esac
  done
fi

echo "Calling tar"
tar -czf $TARBALL_NAME -C $MYTMPDIR ${n} 

echo "Done. Created $TARBALL_NAME."
