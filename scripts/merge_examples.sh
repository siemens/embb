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
        echo "This script is used by jenkins, to merge examples from examples_raw";
        echo "into examples. Should not be called manually.";
        echo ""
	echo "Example call (from the scripts directory as working directory):";
        echo "$0 -d ../";
        echo "";
        echo "Usage: $0 [-d <root project dir>]" 1>&2; exit 1;
}

#check if all dependencies are fulfilled
for DEPENDENCY in rsync mktemp cd grep cmake echo python realpath mkdir git
do
        command -v $DEPENDENCY >/dev/null 2>&1 || { echo >&2 "This script requires $DEPENDENCY but it's not installed. Exiting."; exit 1; }
done

#get command line options
while getopts "d:vq" o; do
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

#sanity check, the user specified directory should contain a CMakeLists.txt file.
if [ ! -f "$CMAKEFILE" ]; then
        echo "--> ! Error, could no locate CMakeLists.txt. Perhaps you specified a wrong directory?"
        echo ""
        usage
fi


#temporary directory for building other things (e.g. Latex or integrating snippets into examples)
MYTMPDIR_BUILD=`mktemp -d`

echo "--> Creating temporary directory $MYTMPDIR_BUILD"
#install traps, deleting the temporary directories when exiting
function finish {
rm -rf $MYTMPDIR_BUILD
}

trap finish EXIT

PROJECT_DIR_FULLPATH=`realpath ${d}`

echo "--> Calling rsync to temporary folder ($MYTMPDIR_BUILD)"

#doing a rsync to another temporary folder, which will be used to build things, like e.g. the tutorial pdf.
redirect_cmd rsync \
        --archive --recursive ${d} $MYTMPDIR_BUILD 

echo "--> Integrating Example Snippets"
REMEMBER_CUR_DIR=$(pwd)

EXAMPLES_DIR="$MYTMPDIR_BUILD/doc/examples_raw"
INTEGRATE_SNIPPETS_SCRIPT="insert_snippets.py"
EXAMPLES_TARGET_DIR="$PROJECT_DIR_FULLPATH/doc/examples"

if [ -f $EXAMPLES_DIR/$INTEGRATE_SNIPPETS_SCRIPT ]; then
        cd "$EXAMPLES_DIR"


        echo "---> Calling integrate script"
        python insert_snippets.py 
      
	if [[ $? = 0 ]]; then
    		echo "success"
	else
    		echo "failure: $?"
		exit 1
	fi
 
	if [ ! -d $EXAMPLES_TARGET_DIR ]; then
		echo "---> Examples target dir does not exist. Creating..."
		redirect_cmd mkdir $EXAMPLES_TARGET_DIR
	fi

        if [ -d $EXAMPLES_TARGET_DIR ]; then
                echo "---> Copy integrated examples back"
                #The examples have been integrated. Copy the integrated source files back.
                redirect_cmd rsync --delete --archive --recursive "$EXAMPLES_DIR/" "$EXAMPLES_TARGET_DIR/" \
                        --exclude=*snippet.h \
                        --exclude=*fragmented.h \
                        --exclude=*snippet.cc \
                        --exclude=*fragmented.cc \
                        --exclude=*$INTEGRATE_SNIPPETS_SCRIPT
		# for commiting, we must be in the project dir
		cd "$PROJECT_DIR_FULLPATH"
	
		redirect_cmd git add -u $EXAMPLES_TARGET_DIR
		redirect_cmd git add $EXAMPLES_TARGET_DIR
		redirect_cmd git commit -m 'Integrating examples_raw to examples using merge_examples.sh script.'
        fi
fi

cd "$REMEMBER_CUR_DIR"

echo "--> Done."
