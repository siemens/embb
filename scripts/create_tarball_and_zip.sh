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

#function for printing usage
usage() { 
        echo "Create a tarball and a zip of the project. Specify the project root with the";
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

#used as wrapper for switching between verbose and normal mode
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
ZIP_NAME="${n}.zip"

#booleans for checking what has to be created
CREATE_TARBALL=true;
CREATE_ZIP=true;

#verify that tarball name doesn't contain forbidden characters
if ! [[ $TARBALL_NAME =~ ^[a-zA-Z0-9|\.|\_|-]+$ ]]; then
        echo "--> Want to create tarball with name $TARBALL_NAME." >&2
        echo '--> ! Filename not valid, only a-z, A-Z, .,- and _ characters are allowed' >&2 # write to stderr
        CREATE_TARBALL=false
fi

#verify that zip name doesn't contain forbidden characters
if ! [[ $ZIP_NAME =~ ^[a-zA-Z0-9|\.|\_|-]+$ ]]; then
        echo "--> Want to create zip with name $TARBALL_NAME." >&2
        echo '--> ! Filename not valid, only a-z, A-Z, .,- and _ characters are allowed' >&2 # write to stderr
        CREATE_ZIP=false
fi

if [ -z "${q}" ]; then
        #in interactive mode, ask the user if the tarball shall be created with this filename
        echo "--> Do you wish to create a tarball with the name $TARBALL_NAME in the current directory?"
        select yn in "Yes" "No"; do
                case $yn in
                Yes ) break;;
                No ) echo "Leaving tarball creation"; CREATE_TARBALL=false; break;;
                esac
        done
else
        echo "--> Tarball with name $TARBALL_NAME will be created."
fi


#check if file with the tarball_name already exists. In interactive mode, ask the user if this file shall be deleted. Otherwise do not create tarball.
if [ \( -f "$TARBALL_NAME" \) -a \( "$CREATE_TARBALL" = true \) ]; then
        if [ -z "${q}" ]; then
                echo "--> File $TARBALL_NAME exists. Delete file?"
                select yn in "Yes" "No"; do
                        case $yn in
                        Yes ) break;;
                        No ) echo "Leaving tarball creation"; CREATE_TARBALL=false; break;;
                        esac
                done
		if [ "$CREATE_TARBALL" = true ] ; then
			rm $TARBALL_NAME
                        if [ -f "$TARBALL_NAME" ]; then
                        	echo "Could not delete $TARBALL_NAME"
                        	CREATE_TARBALL=false
                	fi
		fi
        else
                echo "--> ! File $TARBALL_NAME exists. Delete first. Exiting tarball creation."
                CREATE_TARBALL=false;
        fi
fi

if [ -z "${q}" ]; then
        #in interactive mode, ask the user if the zip shall be created with this filename
        echo "--> Do you wish to create a zip with the name $ZIP_NAME in the current directory?"
        select yn in "Yes" "No"; do
                case $yn in
                Yes ) break;;
                No ) echo "Leaving zip creation"; CREATE_ZIP=false; break;;
                esac
        done
else
        echo "--> Zip with name $ZIP_NAME will be created."
fi

#check if file with the zip_name already exists. In interactive mode, ask the user if this file shall be deleted. Otherwise do not create zip.
if [ \( -f "$ZIP_NAME" \) -a \( "$CREATE_ZIP" = true \) ]; then
        if [ -z "${q}" ]; then
                echo "--> File $ZIP_NAME exists. Delete file?"
                select yn in "Yes" "No"; do
                        case $yn in
                        Yes ) break;;
                        No ) echo "Leaving zip creation"; CREATE_ZIP=false; break;
                        esac
                done
		if [ "$CREATE_ZIP" = true ]; then
                	rm $ZIP_NAME
			if [ -f "$ZIP_NAME" ]; then
                        	echo "Could not delete $ZIP_NAME"
                        	CREATE_ZIP=false
                	fi
                fi
        else
                echo "--> ! File $ZIP_NAME exists. Delete first. Exiting zip creation."
		CREATE_ZIP=false
        fi
fi

#exit if tarball and zip need not / cannot be created
if [ \( "$CREATE_ZIP" = false \) -a \( "$CREATE_TARBALL" = false \) ] ; then
	echo "Exiting, no files to generate";
	exit 1;
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
redirect_cmd cmake "$PROJECT_DIR_FULLPATH" -DBUILD_DOCUMENTATION=ON
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

#this is the rsync to the folder from which the tarball will be created later. Exclude everything that should not be in the tarball. Also exclude things that are generated somewhere else like examples.
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
        --exclude "doc/tutorial/tutorial_raw.md" \
        --exclude "doc/tutorial/*.py" \
        --exclude "doc/tutorial/*.png" \
        --exclude "doc/tutorial/*.css" \
        --exclude "doc/tutorial/*.yml" \
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

#doing an rsync to another temporary folder, which will be used to build things like the tutorial.
redirect_cmd rsync \
        --archive --recursive ${d} $MYTMPDIR_BUILD 

echo "--> Generating Tutorial"
TUTORIAL_SOURCE_DIR="$MYTMPDIR_BUILD/doc/tutorial"
TUTORIAL_TARGET_DIR="$MYTMPDIR/${n}/doc/tutorial"
REMEMBER_CUR_DIR=$(pwd)

if [ -f "$TUTORIAL_SOURCE_DIR/tutorial_raw.md" ]; then
        cd "$TUTORIAL_SOURCE_DIR"
        if [ -f "$TUTORIAL_SOURCE_DIR/bake_tutorial.py" ]; then
		# "redirect_cmd" prefix missing, as output of bake_tutorial.py is redirected
		# itself.
		# TODO: give output file as script input parameter. Modify bake script accordingly.
                python bake_tutorial.py >> tutorial.md

		redirect_cmd sed -i -e 's/<\/\?sub>/~/g' tutorial.md
		redirect_cmd sed -i -e 's/<\/\?sup>/\^/g' tutorial.md
		redirect_cmd pandoc tutorial.md --latex-engine=xelatex -V papersize=a4 -V geometry:margin=3cm -o tutorial.pdf
		redirect_cmd pandoc tutorial.md --css=pandoc.css --self-contained -o tutorial.html
		redirect_cmd pandoc tutorial.md tutorial.yml --epub-stylesheet=pandoc.css -o tutorial.epub
        fi

	if [ -f "$TUTORIAL_SOURCE_DIR/tutorial.pdf" ]; then
                cp $TUTORIAL_SOURCE_DIR/tutorial.pdf $TUTORIAL_TARGET_DIR/
        fi

	if [ -f "$TUTORIAL_SOURCE_DIR/tutorial.epub" ]; then
                cp $TUTORIAL_SOURCE_DIR/tutorial.epub $TUTORIAL_TARGET_DIR/
        fi

	if [ -f "$TUTORIAL_SOURCE_DIR/tutorial.html" ]; then
                cp $TUTORIAL_SOURCE_DIR/tutorial.html $TUTORIAL_TARGET_DIR/
        fi
fi
cd "$REMEMBER_CUR_DIR"

REFMAN_TARGET="$MYTMPDIR/${n}/doc/reference/reference.pdf"
REFMAN_SOURCE="$MYTMPDIR_DOXY_BUILD/latex/refman.pdf"

echo "--> Copying Examples"
REMEMBER_CUR_DIR=$(pwd)

EXAMPLES_DIR="$MYTMPDIR_BUILD/doc/examples"
EXAMPLES_TARGET_DIR="$MYTMPDIR/${n}/doc/"

cd "$EXAMPLES_DIR"
if [ -d $EXAMPLES_TARGET_DIR ]; then
        #Copy the example source files.
        redirect_cmd rsync --archive --recursive $EXAMPLES_DIR $EXAMPLES_TARGET_DIR
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

#check for files that have windows file endings. Those are forbidden.
WINLINES=`find $MYTMPDIR/${n} -not -type d -exec file "{}" ";" | grep CRLF`

if [ -n "$WINLINES" ]; then
        echo "Detected Dos line endings in following files:"
        echo "$WINLINES"
        echo "Error: The project guidelines forbid Dos line endings. Exiting."
        exit 1;
fi

#sanity check... verify that expected targets are there, otherwise abort...
if ! [ -f $MYTMPDIR/${n}/doc/examples/main.cc ]; then
        echo "--> ! Examples missing. Exiting."
        exit 1;
fi

if ! [ -f $MYTMPDIR/${n}/doc/tutorial/tutorial.html ]; then
        echo "--> ! Tutorial HTML missing. Exiting."
        exit 1;
fi

if ! [ -f $MYTMPDIR/${n}/doc/tutorial/tutorial.pdf ]; then
        echo "--> ! Tutorial PDF missing. Exiting."
        exit 1;
fi

if ! [ -f $MYTMPDIR/${n}/doc/tutorial/tutorial.epub ]; then
        echo "--> ! Tutorial EPUB missing. Exiting."
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

if [ -f $MYTMPDIR/${n}/myfile.properties ]; then
        rm $MYTMPDIR/${n}/myfile.properties
fi

#build the tarball, if CREATE_TARBALL is true.
if [ "$CREATE_TARBALL" = true ]; then
        echo "--> Calling tar"
        tar -czf $TARBALL_NAME -C $MYTMPDIR ${n}
        echo "--> Done. Created $TARBALL_NAME."
fi

#build the zip, if CREATE_ZIP is true
if [ "$CREATE_ZIP" = true ]; then
        echo "--> Calling zip"
        cd $MYTMPDIR
        zip -r -q $ZIP_NAME ./*
        cd -
        mv $MYTMPDIR/$ZIP_NAME .
        echo "--> Done. Created $ZIP_NAME."
fi
