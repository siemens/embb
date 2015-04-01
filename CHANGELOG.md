Embedded Multicore Building Blocks (EMB²)
=========================================

Version 0.3.0
-------------

### Features:
- mtapi_c:
    - Implemented action plugin API
    - Implemented load balancing for distributed/heterogeneous systems
    - Implemented OpenCL action plugin
    - Implemented network action plugin
- mtapi_cpp:
    - Added support for distributed/heterogeneous systems

### Changes and improvements:
- mtapi_c:
    - Added multi-instance task support and test
    - Improved notification of worker threads
- mtapi_cpp:
    - Moved interface for homogeneous systems to tasks_cpp
- base_cpp:
    - Moved tick types to internal namespace and added duration typedefs
- dataflow_cpp:
    - Removed spinlocks
    - Simplified registration of processes (only sources need to be added)
    - Increased number of task queues in unit test
    - Added assertion in unit test
    - Improved exception handling
    - Removed stray include
    - Refactored to use tasks_cpp
- algorithms_cpp:
    - Restricted partitioners to random access iterators
    - Added unit tests for partitioners on large ranges
    - Refactored to use tasks_cpp

### Bug fixes:
- Fixed unit test for dataflow_cpp
- Fixed wait-free SPSC queue

### Build system:
- Fixed compilation for newer CMake Versions (>= 3.1)
- Changed task test to avoid Jenkins timeout
- Changed CMakeLists to avoid error if policy is unknown
- Added mtapi_network_c and mtapi_opencl_c to root CMakeLists
- Added tasks_cpp to test scripts / batch file
- Fixed cpplint warnings

### Documentation:
- Extended tutorial and examples regarding support for distributed/heterogeneous systems including plugins and new task interface
- Added Doxygen documentation for mtapi_opencl_c and mtapi_network_c
- Added Doxygen documentation to mtapi_ext.h
- Updated README and removed limitation to homogeneous systems
- Added missing concurrency tags in mtapi_cpp


Version 0.2.3
-------------

### Features:
- None

### Changes and improvements:
- Changed use of partitioners in ForEach, Reduce, Scan, Count, and MergeSort
- Added guard definition to QuickSort and MergeSort requiring random access iterators as inputs
- Unified behavior of algorithms on empty input
- Cleaned up MergeSort and Scan
- Extended computation of number of cores to take into account affinities
- Changed MTAPI_CHECK_STATUS in examples to use exit() instead of abort()
- Added overload for std::exception::what() in embb::base::Exception
- Added missing include in execution_policy.cc
- Added tests for Thread::ID (base_cpp), ExecutionPolicy (mtapi_cpp), and error cases in mtapi_c
- Added tests on empty and negative input ranges in algorithms

### Bug fixes:
- Fixed freeing of temporary buffer in MergeSortAllocate
- Fixed minor bugs in mtapi_c
- Fixed paths in Doxyfile.in template

### Build system:
- Added option to CMake to toggle automatic initialization of MTAPI C++ interface
- Changed run_tests_cygwin script to work with /bin/sh
- Modified create_tarball.sh script for completely automatic tarball creation
- Removed cppcheck warnings
- Removed cpplint warnings
- Updated partest

### Documentation:
- Added paragraphs in tutorial and README regarding performance impact of automatic initialization of MTAPI C++ interface
- Removed automatic collapsing of trees in Doxygen documentation due to incompatibility with latest versions of Doxygen
- Modified reference manual to consistently use function object concept
- Added description of default node attributes in mtapi_c and fixed typo in mtapi_cpp documentation
- Modified paragraph on documentation in README and fixed typo


Version 0.2.2
-------------

### Features:
- None

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

### Build system:
- Removed cppcheck warnings
- Removed cpplint warnings

### Documentation:
- Improved documentation of default values and priorities
- Revised template argument names in base_cpp and containers_cpp
- Moved ExecutionPolicy to CPP_MTAPI and Identity to CPP_ALGORITHMS Doxygen group
- Fixed description of token limit in tutorial
- Added Clang support to README
- Mentioned the ability to cross compile in README
- Added link to release files in README
- Minor corrections in README
- Updated copyright notice
- Changed year in tutorial.tex to 2015


Version 0.2.1
-------------

### Features:
- Added embb_core_count_available() implementation for FreeBSD.

### Changes and improvements:
- None

### Bug fixes:
- Fixed implementation of atomic operations on ARM.
- Fixed bug in HelpScan routine of hazard pointer implementation.
- Replaced inclusion of non-standard header malloc.h with stdlib.h.

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
- Corrected license in COPYING file (BSD 2-clause).
- Changed default Doxygen level for API to two.
- Added check for Doxyfile.in (if not present, do not add Doxygen target).
- Added PDF image of building blocks and updated tutorial.tex.
- Fixed typos and wording.


Version 0.2.0
-------------

- Initial version of open source release