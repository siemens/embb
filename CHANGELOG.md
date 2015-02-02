Embedded Multicore Building Blocks (EMB²)
=========================================

Version 0.2.2
-------------

### Bug fixes:
- Fixed 64bit problem in atomics
- Fixed bug in dataflow_cpp causing network to hang
- Fixed bug in conversion of core_set
- Fixed fetch-and-add implementation to support armv7-a
- Fixed missing freeing of mutex attributes in case of error
- Fixed bug where closure was allocated with Allocation::New but deleted with operator delete
- Fixed inconsistent naming of unit test cases
- Fixed memory allocation in hazard pointer implementation by replacing calls to new and delete with EMB²-specific functions
- Fixed memory leak in tests for containers
- Fixed affinity implementation for FreeBSD

### Changes and improvements:
- Added checks for memory leaks in tests
- Added block size support in MergeSort
- Renamed all platform specific defines to EMBB_PLATFORM_*
- Changed all checks for platform specific defines to checks for EMBB_* defines
- Replaced C++11 initializer lists in examples with C++03 compliant statements
- Extended unit tests for MPMC queue with checks for relative order
- Added check for result of pthread_attr_destroy
- Added assert in embb_tss_get
- Moved ExecutionPolicy from algorithms to mtapi_cpp, removed Affinity

### Features:
- None

### Build system:
- Removed cppcheck warnings
- Removed cpplint warnings

### Documentation:
- Improved documentation of default values and priorities
- Revised template argument names in base_cpp and containers_cpp
- Moved ExecutionPolicy and Identity to CPP_ALGORITHMS Doxygen group
- Fixed description of token limit in tutorial
- Added Clang support to README
- Mentioned the ability to cross compile in README
- Added link to release files in README
- Minor corrections in README
- Updated copyright notice
- Changed year in tutorial.tex to 2015


Version 0.2.1
-------------

### Bug fixes:
- Fixed implementation of atomic operations on ARM.
- Fixed bug in HelpScan routine of hazard pointer implementation.
- Replaced inclusion of non-standard header malloc.h with stdlib.h.

### Features:
- Added embb_core_count_available() implementation for FreeBSD.

### Build system:
- Added initial support for Clang.
- Added initial support for Travis.
- Fixed "Policy CMP0026 is not set" warning of CMake >= 3.0.2.
- Made build independent of build-dir / source-dir location.
- Fixed remaining GCC, Clang, MSVC, and cpplint warnings.
- Replaced /bin/bash with /usr/bin/env bash in scripts.
- Added executable flag to shell scripts.
- Omitted Doxygen "Makefiles" from tarball.
- Included patchlevel in name of installation directory on Linux.
- Added LaTeX .bbl and .blg files to ignore list.

### Documentation:
- Updated README file, created CHANGELOG file, and added markdown support.
- Corrected license in COPYING.md (BSD 2-clause).
- Changed default Doxygen level for API to two.
- Added check for Doxyfile.in (if not present, do not add Doxygen target).
- Added PDF image of building blocks and updated tutorial.tex.
- Fixed typos and wording.


Version 0.2.0
-------------

- Initial version of open source release