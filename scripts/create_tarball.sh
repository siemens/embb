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

#function for printing usage
usage() { 
        echo "Create a tarball of the project. Specify the project root with the";
        echo "-d parameter. Optionally, specify the -v switch to get verbose output";
        echo "and/or the -q switch, for non-interactive (without user inputs)";
        echo "processing.";
        echo "";
        echo "Version number and project name is automatically derived from CMakeLists.txt";
        echo "in the project's root.";
        echo "Tarball name: [PROJECT_NAME]_[VERSION_NUMBER].tar.gz";
        echo "Example call (from the scripts directory as working directory):";
        echo "$0 -d ../";
        echo "";
        echo "Usage: $0 [-d <root project dir>] [-v] [-q]" 1>&2; exit 1;
}

#check if all dependencies are fulfilled
for DEPENDENCY in rsync pdflatex bibtex cp tar mktemp cd grep cmake find file echo python realpath sed
do
        command -v $DEPENDENCY >/dev/null 2>&1 || { echo >&2 "This script requires $DEPENDENCY but it's not installed. Exiting."; exit 1; }
done

#get command line options
while getopts "d:vq" o; do
        case "${o}" in
        d)
                d=${OPTARG}
                ;;
        v)
                v=1
                ;;
        q)
                q=1
                ;;
        *)
                usage
                ;;
        esac
done
shift $((OPTIND-1))

#used as wrapper, for switching between verbose and normal mode
redirect_cmd() {
        if [ -z "${v}" ]; then
                "$@" > /dev/null 2>&1
        else
                "$@"
        fi
}

#user has to specify directory
if [ -z "${d}" ]; then
        usage
fi

#the specified directory has to exist
if [ ! -d "$d" ]; then
        echo "--> ! Error, directory $d does not exist or is not a directory!" 
        echo "" 
        usage
fi

CMAKEFILE="$d/CMakeLists.txt"

if [ ! -f "$CMAKEFILE" ]; then
        echo "--> ! Error, could no locate CMakeLists.txt"
        echo ""
        usage
fi

#derive version number from cmake script
VERSION_MAJOR=`cat $CMAKEFILE | grep EMBB_BASE_VERSION_MAJOR | sed "s/^[^0-9]*\([0-9]\+\)[^0-9]*$/\1/g"`
VERSION_MINOR=`cat $CMAKEFILE | grep EMBB_BASE_VERSION_MINOR | sed "s/^[^0-9]*\([0-9]\+\)[^0-9]*$/\1/g"`
VERSION_PATCH=`cat $CMAKEFILE | grep EMBB_BASE_VERSION_PATCH | sed "s/^[^0-9]*\([0-9]\+\)[^0-9]*$/\1/g"`
VERSION_NUMBER="${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}"

#generate tarball name
PROJECT_NAME=`cat $CMAKEFILE | grep project.*\(.*\) | sed "s/^[^(]*(\([^)]*\)).*$/\1/g" | tr '[:upper:]' '[:lower:]'`
n="${PROJECT_NAME}-${VERSION_NUMBER}"
TARBALL_NAME="${n}.tar.gz"

#verify that tarball doesn't allow forbidden characters
if ! [[ $TARBALL_NAME =~ ^[a-zA-Z0-9|\.|\_|-]+$ ]]; then
        echo "--> Want to create tarball with name $TARBALL_NAME." >&2
        echo '--> ! Filename not valid, only a-z, A-Z, .,- and _ characters are allowed' >&2 # write to stderr
        exit 1
fi

if [ -z "${q}" ]; then
        #in interactive mode, ask the user if the tarball shall be created with this filename
        echo "--> Do you wish to create a tarball with the name $TARBALL_NAME in the current directory?"
        select yn in "Yes" "No"; do
                case $yn in
                Yes ) break;;
                No ) echo Leaving tarball creation; exit 1;;
                esac
        done
else
        echo "--> Tarball with name $TARBALL_NAME will be created."
fi

#check if file with the tarball_name already exists. In interactive mode, ask the user if this file shall be deleted. Otherwise just exit.
if [ -f "$TARBALL_NAME" ]; then
        if [ -z "${q}" ]; then
                echo "--> File $TARBALL_NAME exists. Delete file?"
                select yn in "Yes" "No"; do
                        case $yn in
                        Yes ) break;;
                        No ) echo Leaving tarball creation; exit 1;;
                        esac
                done
                rm $TARBALL_NAME
                if [ -f "$TARBALL_NAME" ]; then
                        echo "Could not delete $TARBALL_NAME"
                        exit 1
                fi
        else
                echo "--> ! File $TARBALL_NAME exists. Delete first. Exiting."
                exit 1;
        fi
fi

#temporary directory for doxygen
MYTMPDIR_DOXY_BUILD=`mktemp -d`
#temporary directory for building other things (e.g. Latex or integrating snippets into examples)
MYTMPDIR_BUILD=`mktemp -d`
#temporary target directory, from this the tarball will be created
MYTMPDIR=`mktemp -d`

echo "--> Creating temporary directories $MYTMPDIR $MYTMPDIR_BUILD $MYTMPDIR_DOXY_BUILD"
#install traps, deleting the temporary directories when exiting
function finish {
rm -rf $MYTMPDIR
rm -rf $MYTMPDIR_DOXY_BUILD
rm -rf $MYTMPDIR_BUILD
}

trap finish EXIT

PROJECT_DIR_FULLPATH=`realpath ${d}`


echo "--> Generating Doxygen"

REMEMBER_CUR_DIR=$(pwd)
cd "$MYTMPDIR_DOXY_BUILD"
echo "---> Initialize CMake"
redirect_cmd cmake "$PROJECT_DIR_FULLPATH" 
echo "---> Call CMake with target Doxygen"
redirect_cmd cmake --build . --target doxygen
REFMAN_TEXFILE="$MYTMPDIR_DOXY_BUILD/latex/refman.tex"
DO_CREATE_LATEXDOC=true

if [ ! -f "$REFMAN_TEXFILE" ]; then
        echo "---> ! Could not find doxygen tex source $REFMAN_TEXFILE. Leaving tarball creation."
        exit 1;
fi

#to resolve all references, pdf and bibtex have to be run more than once. With 4 runs, we should get everything right.
PDFRUNS=4

echo "---> Build Doxygen PDF reference"


if [ "$DO_CREATE_LATEXDOC" = true ] ; then
        cd "$MYTMPDIR_DOXY_BUILD/latex"
        for ((i=1; i<=$PDFRUNS; i++)); do
                echo "----> LaTeX Run ($i/$PDFRUNS)"
                redirect_cmd pdflatex refman.tex  
                redirect_cmd bibtex refman  
        done
fi


cd "$REMEMBER_CUR_DIR"

echo "--> Calling rsync to temporary folder 1/2 ($MYTMPDIR)"

#this is the rsync, to the folder from which the tarball will be created later. Exclude everything, that should not be in the tarball. Also exclude things, that are generated somewhere else, like examples.
redirect_cmd rsync \
        --exclude ".git" \
        --exclude ".gitignore" \
        --exclude ".gitattributes" \
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
        --exclude "doc/examples" \
        --exclude "doc/examples/insert_snippets.py" \
        --exclude ".travis.yml" \
        --archive --recursive ${d} $MYTMPDIR/${n} 

echo "--> Replace version number in README"

README_FILE="$MYTMPDIR/${n}/README.md"

#replace version number in readme
if [ -f $README_FILE ]; then
        sed -i "s/\[VERSION_NUMBER_TEMPLATE\]/$VERSION_NUMBER/g" $README_FILE
fi

echo "--> Calling rsync to temporary folder 2/2 ($MYTMPDIR_BUILD)"

#doing a rsync to another temporary folder, which will be used to build things, like e.g. the tutorial pdf.
redirect_cmd rsync \
        --archive --recursive ${d} $MYTMPDIR_BUILD 

echo "--> Generating Tutorial PDF"
TUTORIAL_TEX_DIR="$MYTMPDIR_BUILD/doc/tutorial"
REMEMBER_CUR_DIR=$(pwd)
TUTORIAL_PDF_SOURCE="$TUTORIAL_TEX_DIR/tutorial.pdf"
TUTORIAL_PDF_TARGET="$MYTMPDIR/${n}/doc/tutorial/tutorial.pdf"

if [ -f "$TUTORIAL_TEX_DIR/tutorial.tex" ]; then

        cd "$TUTORIAL_TEX_DIR"	
        for ((i=1; i<=$PDFRUNS; i++)); do

                echo "---> LaTeX Run ($i/$PDFRUNS)"
                redirect_cmd pdflatex tutorial.tex  
                redirect_cmd bibtex tutorial  
        done
        if [ -f "$TUTORIAL_PDF_SOURCE" ]; then
                cp $TUTORIAL_PDF_SOURCE $TUTORIAL_PDF_TARGET
        fi
fi
cd "$REMEMBER_CUR_DIR"

REFMAN_TARGET="$MYTMPDIR/${n}/doc/reference/reference.pdf"
REFMAN_SOURCE="$MYTMPDIR_DOXY_BUILD/latex/refman.pdf"

echo "--> Integrating Example Snippets"
REMEMBER_CUR_DIR=$(pwd)

EXAMPLES_DIR="$MYTMPDIR_BUILD/doc/examples"
INTEGRATE_SNIPPETS_SCRIPT="insert_snippets.py"
EXAMPLES_TARGET_DIR="$MYTMPDIR/${n}/doc/"

if [ -f $EXAMPLES_DIR/$INTEGRATE_SNIPPETS_SCRIPT ]; then
        cd "$EXAMPLES_DIR"


        echo "---> Calling integrate script"
        redirect_cmd python insert_snippets.py 

        if [ -d $EXAMPLES_TARGET_DIR ]; then
                echo "---> Copy integrated examples back"
                #The examples have been integrated. Copy the integrated source files.
                redirect_cmd rsync --archive --recursive $EXAMPLES_DIR $EXAMPLES_TARGET_DIR \
                        --exclude=*snippet.h \
                        --exclude=*fragmented.h \
                        --exclude=*snippet.cc \
                        --exclude=*fragmented.cc \
                        --exclude=*$INTEGRATE_SNIPPETS_SCRIPT 
        fi
fi

cd "$REMEMBER_CUR_DIR"

echo "--> Copy reference manual"
if [ -f $REFMAN_SOURCE ]; then
        cp $REFMAN_SOURCE $REFMAN_TARGET
else
        echo "--> ! Could not find doxygen pdf document $REFMAN_SOURCE. Exiting"
        exit 1;
fi

if [ -d $MYTMPDIR_DOXY_BUILD/html ]; then
        redirect_cmd rsync --archive --recursive $MYTMPDIR_DOXY_BUILD/html/ $MYTMPDIR/${n}/doc/reference/doxygen_html_generated
else
        echo "Doxygen HTML was not generated. Tarball will not contain HTML reference documentation. Exiting."
        exit 1;
fi

echo "--> Checking line endings"

#check for files, that have windows file endings. Those are forbidden.
WINLINES=`find $MYTMPDIR/${n} -not -type d -exec file "{}" ";" | grep CRLF`

if [ -n "$WINLINES" ]; then
        echo "Detected Dos line endings in following files:"
        echo "$WINLINES"
        echo "Error: The project guidelines forbid Dos line endings. Exiting."
        exit 1;
fi

#sanity check... verify, that expected targets are there, otherwise abort...
if ! [ -f $MYTMPDIR/${n}/doc/examples/main.cc ]; then
        echo "--> ! Examples missing. Exiting."
        exit 1;
fi

if ! [ -f $MYTMPDIR/${n}/doc/tutorial/tutorial.pdf ]; then
        echo "--> ! Tutorial PDF missing. Exiting."
        exit 1;
fi

if ! [ -f $MYTMPDIR/${n}/doc/reference/reference.pdf ]; then
        echo "--> ! Reference PDF documentation missing. Exiting."
        exit 1;
fi

if ! [ -f $MYTMPDIR/${n}/doc/reference/doxygen_html_generated/index.html ]; then
        echo "--> ! Reference HTML documentation missing. Exiting."
        exit 1;
fi

#finally, build the tarball.
echo "--> Calling tar"
tar -czf $TARBALL_NAME -C $MYTMPDIR ${n} 

echo "--> Done. Created $TARBALL_NAME."
