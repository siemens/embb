#!/usr/bin/env bash

# Copyright (c) 2017, embedded brains GmbH. All rights reserved.
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

# Use this script to build the EMBB for all multilibs of a GCC cross-compiler
# installation.  You must invoke this script in the EMBB source base directory.
# The first parameter must be the target of the cross-compiler, e.g. arm-eabi.
# The cross-compiler must be in your $PATH.
#
#   ./scripts/build_gcc_multilibs.sh <TARGET>

set -x
target="$1"

CC="$target-gcc"
CXX="$target-g++"
if which "$CXX" ; then
	builddir="build-gcc-mulilibs"
	prefix=`"$CXX" --verbose 2>&1 | grep 'Configured with' | sed 's/.*prefix=//' | sed 's/ .*//'`
	prefix="$prefix/$target"

	for i in `"$CXX" --print-multi-lib` ; do
		multilibdir=`echo $i | sed 's/;.*//'`
		CFLAGS=`echo $i | sed 's/.*;//' | sed 's/@/ -/g'`
		CFLAGS="$CFLAGS -O2 -g"
		rm -rf "$builddir"
		mkdir "$builddir"
		cd "$builddir"
		cmake .. \
			-DCMAKE_CXX_COMPILER="$CXX" \
			-DCMAKE_C_COMPILER="$CC" \
			-DCMAKE_CXX_FLAGS="$CFLAGS" \
			-DCMAKE_C_FLAGS="$CFLAGS" \
			-DUSE_C11_AND_CXX11=ON \
			-DCMAKE_SYSTEM_NAME="Generic" \
			-DINSTALL_PREFIX="$prefix" \
			-DBUILD_TESTS="OFF"
		make -j `nproc`
		make install DESTDIR=x
		cp -r "x/$prefix/lib/"* "$prefix/lib/$multilibdir"
		cd ..
	done

	cp -r "$builddir/x/$prefix/include/"* "$prefix/include"
	rm -rf "$builddir"
fi
