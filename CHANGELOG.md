Embedded Multicore Building Blocks (EMB²)
=========================================


Version 1.0.0
-------------

### Features:
- Added support for heterogeneous systems in algorithms and dataflow
- Added ForLoop function for parallel index-based loops
- Added extension for MTAPI callback functions
- Added job and task attributes for scheduling based on problem size
- Added functions to retrieve context for OpenCL and CUDA plugins

### Changes and improvements:
- Added support for atomic operations provided by C11/C++11
- Optimized spinlock implementation
- Added default constructors for Group and Queue
- Added test for exceptions in mtapi_cpp
- Added initialization of affinity in execution policy constructors
- Added initialization of user_data pointer in task attributes
- Corrected return code for cancelled task in network plugin
- Improved error handling in MTAPI CPP interface
- Enabled reuse of main thread by default
- Added explicit finalization of allocated elements in MTAPI pool
- Removed spinlock from dataflow
- Added initialization of clock listeners in dataflow
- Changed dataflow so that sources are run within slice group
- Changed iterators in containers to return pairs and added documentation
- Added iterators to value pools and changed destruction of elements in object pools
- Added initializers in unit tests
- Resolved Coverity Scan warnings
- Resolved warning when compiling with GCC 7.1

### Bug fixes:
- Fixed resource leak in unit test for pool
- Fixed potential resource leak and remote task cancel handling in network plugin
- Fixed MTAPI_CPP affinity test on single-core machines
- Fixed handling of task error conditions encountered during execution
- Fixed wrong allocation size in MTAPI C implementation
- Fixed possible overflow in unit test for algorithms
- Fixed problem in unit test where mutex could be destroyed while still being locked

### Build system:
- Added option for generating shared libraries
- Added CMake package finder and use-script for integration of EMB² in applications
- Added output for all options in CMake build script
- Integrated Coverity Scan into Travis CI builds
- Fixed linker problem with CUDA plugin
- Fixed script that creates tarball and zip file
- Updated .travis.yml

### Documentation:
- Added sample application (video processing)
- Revised tutorial and converted to markdown format
- Included generation of HTML and EPUB files (in addition to PDF)
- Added example for CUDA plugin
- Improved Doxygen documentation
- Extended CONTRIBUTING file
- Updated README file
- Updated year in license headers
- Fixed typos


Version 0.5.0
-------------

### Features:
- Enhanced C++ interfaces for MTAPI task handling
- CUDA plugin for MTAPI task scheduler (experimental)
- Added support for worker thread priorities
- Added support for task priorities and affinities in dataflow_cpp

### Changes and improvements:
- Introduced threading analysis mode and added mutex-based atomics
- Reduced number of test iterations in threading analysis mode
- Replaced spinlocks with mutexes in threading analysis mode
- Added initialization and destroy methods for atomics
- Added initialization checks in debug mode for atomics
- Resolved AppVerifier warnings
- Added counter reset function
- Removed double initializations of counters
- Removed deletion of resources in timeout scenario
- Improved portability on POSIX systems
- Added big endian support for MTAPI attributes
- Removed legacy group constructor in MTAPI
- Resolved potential integer overflows on 32-bit targets
- Added missing <sys/select.h>, removed inclusion of <memory.h>
- Removed separate storage for internal task queues
- Removed spinlock workaround
- Resolved issue with Visual Studio 2012
- Removed unnecessary value copy
- Removed tasks_cpp component
- Switched C style cast to C++ style cast in containers_cpp
- Resolved problem with non-monotonic time readings and timeouts
- Moved processing of sources in dataflow_cpp to worker threads
- Added setter to enable main thread reuse
- Added functionality to start function objects directly as tasks
- Added tests for dataflow_cpp with and without main thread reuse
- Added test for detached tasks
- Added test to reproduce out-of-order execution of queued task when waiting for other tasks
- Improved tests for base_cpp, mtapi_cpp, containers_cpp, dataflow_cpp

### Bug fixes:
- Fixed Pthread API error reported by helgrind
- Fixed bug where notification of action was missing when a task finished
- Fixed bug causing ordered queue to be deleted before last task finished
- Fixed problem with network plugin and removed dead code
- Fixed problem with mtapi_action_delete which was not blocking until all associtated tasks were completely deleted
- Fixed race condition in task handling
- Fixed incomplete initialization with user provided NodeAttributes
- Fixed problem with ordered queues where tasks were executed out of order while waiting
- Fixed behavior of retained tasks
- Fixed bug in handling of canceled tasks causing the network test to hang
- Fixed handling of detached tasks in conjunction with retaining queues and multi-instance tasks
- Fixed automatic initialization in mtapi_cpp
- Fixed handling of tasks executed during a wait operation
- Fixed deletion of detached tasks
- Fixed bug in MTAPI scheduler related to dataflow_cpp

### Build system:
- Performed analysis using AppVerifier
- Fixed path problems in tarball and zip creation
- Added creation of zip release files
- Added message if documentation build is enabled but Doxygen is not found
- Updated Travis CI integration
- Fixed build on FreeBSD
- Added installation of PDB files in Visual Studio debug builds
- Added missing install directive for config header
- Resolved warning in hazard pointer implementation
- Changed CMake scripts (caching of install prefix)

### Documentation:
- Revised and updated README file
- Added CONTRIBUTING file
- Improved examples for mtapi_cpp
- Updated and extended Doxygen documentation
- Updated license headers in MTAPI plugin sources


Version 0.4.0
-------------

### Features:
- Added consistency checking functionality to dataflow
- Added C++ wrapper for base_c logging functions
- Reworked dataflow_cpp interface for easier usage

### Changes and improvements:
- Improved network plugin with better error checking and task cancellation support
- Revised dataflow_cpp so that network token count can now be set at runtime
- Added automatic determination of token count to dataflow_cpp
- Added checks for NULL pointers in C interface functions
- Extended mtapi_info_t to conform to the standard
- Fixed CodeSonar warnings across the code base
- Changed spinlock implementation to yield every 1024 spins now
- Changed asserts on interface visible parameters to execeptions
- Enabled reuse of main thread, which is configurable via node attributes

### Bug fixes:
- Fixed problem causing low performance on the Jetson TK1 board
- Fixed bug in mtapi_c causing a task wait to hang
- Fixes issue with the AMD APP SDK in the OpenCL plugin
- Fixed problem with automatic initialization in tasks_cpp
- Fixed memory leaks in tests

### Build system:
- Removed dependency on an installed OpenCL SDK
- Moved all MTAPI plugins into folder mtapi_plugins_c
- Resolved MSVC warnings and build problem

### Documentation:
- Updated README to reflect new directory structure
- Updated Doxygen documentation of dataflow_cpp for the improved interface
- Updated tutorial and examples to show the usage of dataflow_cpp


Version 0.3.2
-------------

### Features:
- Added central spinlock implementation for C/C++ and removed existing (decentral) ones

### Changes and improvements:
- Reworked and optimized hazard pointer implementation
- Adjusted interfaces of value pool and object pool
- Replaced C style casts

### Bug fixes:
- Fixed ordering of destruction and deallocation in value pool
- Fixed potential use of uninitialized memory in WaitFreeArrayValuePool and LockFreeTreeValuePool

### Build system:
- Given user the ability to append compiler flags
- Let CMake return an error on Doxygen warnings when WARNINGS_ARE_ERRORS is enabled
- Fixed cpplint warnings

### Documentation:
- Added mutex concept
- Corrected library ordering/names in README (section "Using the Library")
- Improved exception messages
- Fixed typos in condition_var_test.cc


Version 0.3.1
-------------

### Features:
- None

### Changes and improvements:
- Removed one function argument from algorithms::Invoke
- Added "explicit" specifier to base type constructor of Atomic<BaseType*>
- Added "const" qualifier to dereference operator and member access operator of AtomicPointer<>
- Changed AtomicBase<>::CompareAndSwap to atomically return expected value
- Replaced constant in dataflow_cpp_test_simple.cc with corresponding macro
- Added initialization of atomic variable in hazard_pointer_test.cc to avoid warning with GCC 5.1
- Changed initial value of allocated_object_from_different_thread
- Added tests for ID Pool and check for memory leaks
- Updated unit test for the UniqueLock::Swap

### Bug fixes:
- Fixed implementation of ID pool (provided fewer elements than specified by capacity)
- Fixed unsigned overflow bug in timed wait function of condition variables
- Fixed implementation of UniqueLock::Swap

### Build system:
- Improved CMake output for automatic initialization option
- Fixed cpplint and unsigned/signed warnings

### Documentation:
- Fixed documentation of UniqueLock class
- Updated README file


Version 0.3.0
-------------

### Features:
* MTAPI:
  - Implemented action plugin API
  - Implemented load balancing for distributed/heterogeneous systems
  - Implemented OpenCL action plugin
  - Implemented network action plugin
  - Added support for distributed/heterogeneous systems

### Changes and improvements:
* Base:
    - Moved tick types to internal namespace and added duration typedefs
* MTAPI:
  - Added multi-instance task support and test
  - Improved notification of worker threads
  - Moved interface for homogeneous systems to tasks_cpp
* Algorithms:
  - Restricted partitioners to random access iterators
  - Added unit tests for partitioners on large ranges
  - Refactored to use tasks_cpp
* Dataflow:
  - Removed spinlocks
  - Simplified registration of processes (only sources need to be added)
  - Increased number of task queues in unit test
  - Added assertion in unit test
  - Improved exception handling
  - Removed stray include
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