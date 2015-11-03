Embedded Multicore Building Blocks (EMB²)
=========================================


Overview
--------

The Embedded Multicore Building Blocks (EMB²) are an easy to use yet powerful
and efficient C/C++ library for the development of parallel applications. EMB²
has been specifically designed for embedded systems and the typical
requirements that accompany them, such as real-time capability and constraints
on memory consumption. As a major advantage, low-level operations are hidden
in the library which relieves software developers from the burden of thread
management and synchronization. This not only improves productivity of
parallel software development, but also results in increased reliability and
performance of the applications.

EMB² is independent of the hardware architecture (x86, ARM, ...) and runs on
various platforms, from small devices to large systems containing numerous
processor cores. It builds on MTAPI, a standardized programming interface for
leveraging task parallelism in embedded systems containing symmetric or
asymmetric multicore processors. A core feature of MTAPI is low-overhead
scheduling of fine-grained tasks among the available cores during runtime.
Unlike existing libraries, EMB² supports task priorities and affinities, which
allows the creation of soft real-time systems. Additionally, the scheduling
strategy can be optimized for non-functional requirements such as minimal
latency and fairness.

Besides the task scheduler, EMB² provides basic parallel algorithms, concurrent
data structures, and skeletons for implementing stream processing applications
(see figure below). These building blocks are largely implemented in a
non-blocking fashion, thus preventing frequently encountered pitfalls like
lock contention, deadlocks, and priority inversion. As another advantage in
real-time systems, the algorithms and data structures give certain progress
guarantees. For example, wait-free data structures guarantee system-wide
progress which means that every operation completes within a finite number of
steps independently of any other concurrent operations on the same data
structure.

<img src="doc/images/embb.png" alt="Building blocks of EMB²" width="500"/>

Community and Contact
---------------------

Project home:
  - https://github.com/siemens/embb

Git:
  - https://github.com/siemens/embb.git (HTTP)
  - git@github.com:siemens/embb.git (SSH)

Mailing lists:
  - embb-announcements@googlegroups.com (announcements)
  - embb-dev@googlegroups.com (development)

Subscription:
  - https://groups.google.com/forum/#!forum/embb-announcements/join
  - https://groups.google.com/forum/#!forum/embb-dev/join

Contact:
  - embb.info@gmail.com or
  - tobias.schuele@siemens.com


License
-------

See the file "COPYING.md" in the project's root directory.


Requirements
------------

This project is based on the standards C99 (for C code) and C++03 (for C++
code) to be usable on a wide range of target systems. It has been tested on
the following OS/compiler/architecture combinations:

  - Linux (Ubuntu 12.04) / GCC 4.8.1 / x86, x86_64
  - Linux (Ubuntu 12.04) / Clang 3.0.0 / x86_64
  - Linux (Ubuntu 14.04) / GCC 4.8.2 / ARMv7
  - Windows
    * MSVC 12.0.21005.1 REL / x86, x86_64
    * MSVC 11.0.50727.42 VSLRSTAGE / x86, x86_64

Other compilers and operating systems may be supported without any changes to
the source code. The project includes unit tests that can be used to find out
whether a system not officially supported is suitable to run EMB². If there is
a requirement to support a system on which the unit tests do not pass, please
contact us: embb-dev@googlegroups.com.


Directory Structure
-------------------

EMB² consists of various building blocks. For some of them, there exist C and
C++ versions, others are only implemented in C++. The directory names are
postfixed with either "_cpp" or "_c" for the C++ and C versions, respectively.
Currently, EMB² contains the following components:

  - base: base_c, base_cpp
  - mtapi: mtapi_c, mtapi_network_c, mtapi_opencl_c, mtapi_cpp
  - tasks: tasks_cpp
  - algorithms: algorithms_cpp
  - dataflow: dataflow_cpp
  - containers: containers_cpp

Each component consists of an include, a src, and a test subfolder that contain
the header files, source files, and unit tests, respectively.

Component base_c contains abstractions for threading, synchronization, atomic
operations, and other functionalities. As the name indicates, the code is
implemented in C. Component base_cpp is mainly a C++ wrapper around the base_c
functions. Component mtapi_c is a task scheduler written in C and mtapi_cpp a
C++ wrapper for the scheduler (mtapi_network_c and mtapi_opencl_c are scheduler
plugins for distributed and OpenCL-based heterogeneous systems, respectively).
To simplify programming of homogeneous systems, tasks_cpp contains abstractions
to the MTAPI interfaces. Component algorithms_cpp provides high-level constructs
for typical parallelization tasks in C++, and dataflow_cpp generic skeletons for
the development of parallel stream-based applications. Finally, containers_cpp
provides data structures for storing objects in a thread-safe way.


Build and Installation
----------------------

Note: It is recommended to build from a release file and not from a repository
snapshot in order to get the documentation and the examples out-of-the box.
The release files can be found at https://github.com/siemens/embb/releases.

EMB² is built using CMake (version 2.8.9 or higher). CMake is a build file
generator which allows to abstract from the concrete build tools. To generate
and invoke the platform-specific build files, open a shell (on Windows, use
the Visual Studio developer shell to have the correct environment variables)
and change to the project's root directory. Create a subdirectory, where you
want to build the library, e.g., "build". Change to that subdirectory. It is
assumed that the project's root directory is now the parent directory.

### 1. Generation of native build files

Choose an appropriate build file generator for your system.

  - For Linux, GCC/Clang, x86/x86_64/ARM: "Unix Makefiles"
  - For Windows, MSVC of VS 2013, x86:    "Visual Studio 12"
  - For Windows, MSVC of VS 2013, x86_64: "Visual Studio 12 Win64"
  - For Windows, MSVC of VS 2012, x86:    "Visual Studio 11"
  - For Windows, MSVC of VS 2012, x86_64: "Visual Studio 11 Win64"

A list of all available generators can be displayed by typing "cmake" without
any options. The build files can be generated using the following command:

    cmake -G <generator> .. [OPTIONS]

Note that on Linux, the architecture (32/64 bit) cannot be selected by the
generator. However, the build mode (Release/Debug) can be specified using the
option -DCMAKE_BUILD_TYPE=[Release|Debug]. If no build mode is given on Linux,
the default (Release) is used. The Visual Studio generators create build files
for both modes (the selection is done at build time).

You may choose a custom compiler instead the default one by defining
CMAKE_CXX_COMPILER and/or CMAKE_C_COMPILER. For example, to use Clang on Linux
use:

    cmake .. -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang

In the same way you may cross compile to another platform. For example, to cross
compile to ARM Linux using GCC, you need to specify the cross compiler itself and
the target architecture as an argument to the compiler:

    cmake .. -DCMAKE_CXX_COMPILER=arm-linux-gnueabi-gcc++
             -DCMAKE_CXX_FLAGS=-march=armv7-a
             -DCMAKE_C_COMPILER=arm-linux-gnueabi-gcc
             -DCMAKE_C_FLAGS=-march=armv7-a

EMB² can be built with and without C++ exception handling, which has to be
specified on build file generation. When exceptions are turned off, an error
message is emitted and the program aborts in case of an exception within EMB².
To disable exceptions, add the option -DUSE_EXCEPTIONS=OFF.

Similarly, automatic initialization of the task scheduler by the MTAPI C++
interface can be disabled with -DUSE_AUTOMATIC_INITIALIZATION=OFF. This way,
unexpected delays after startup can be avoided, e.g. for timing measurements.

The tutorial of EMB² comes with example source files in doc/examples/. These
can be built with the other source files using CMake option -DBUILD_EXAMPLES=ON
in the generation step. Note, however, that the examples use C++11 features and
require a corresponding compiler.

Now you can generate the build files as shown by the following examples.

For a Linux Debug build with exception handling, type

    cmake -G "Unix Makefiles" .. -DCMAKE_BUILD_TYPE=Debug

For a default Linux build without automatic MTAPI C++ initialization, type

    cmake .. -DUSE_AUTOMATIC_INITIALIZATION=OFF

For a Windows build (VS 2013, x86) without exception handling, type

    cmake -G "Visual Studio 12" .. -DUSE_EXCEPTIONS=OFF

Note that "Visual Studio 12" refers to the version number of Visual Studio and
not to the year in which it was released (2013).

### 2. Compiling and linking

As the next step, you can compile the library using the generated build files.
On Linux, the build mode (Release|Debug) is already given in the build files,
whereas on Windows, it has to be specified now.

For a Linux build, type

    cmake --build .

For a Windows Release build, type

    cmake --build . --config Release

### 3. Running the tests

To check whether EMB² was compiled correctly, run the tests. The test
executables are contained in the subfolder "binaries".

On Linux, type

    binaries/run_tests.sh

On Windows, type

    binaries\run_tests.bat

If no error message occurs, EMB² is working fine.

### 4. Installation

The default installation path on Linux is

    /usr/local/

and on Windows

    C:\Program Files\embb-X.Y.Z\ or C:\Program Files (x86)\embb-X.Y.Z

depending on the target architecture.

If you want a different installation path, you can change it now by typing

    cmake -DINSTALL_PREFIX=YourCustomPath ..

The option "-DINSTALL_PREFIX=YourCustomPath" can also be given in Step 1.

To install the files, use the command

    cmake --build . --target install

which copies the contents of the "install" folder to the "bin", "lib", and
"include" folders in the installation path. For the default paths, the
installation has to be run with administrator / root privileges.


Using the Library
-----------------

To use EMB², the include files have to be made available during compilation of
your application and the libraries have to be added during linking.

### 1. Using C++

If you want to use the C++ functionalities of EMB², you have to link the
following libraries (names will be different on Windows and on Linux) in the
given order:

    embb_base, embb_base_cpp, embb_mtapi_c, embb_mtapi_cpp, embb_containers_cpp,
    embb_algorithms_cpp, embb_dataflow_cpp

The C++ header files can be included as follows:

    #include<embb/mtapi/mtapi.h>
    #include<embb/base/base.h>
    #include<embb/containers/containers.h>
    #include<embb/dataflow/dataflow.h>

### 2. Using C

The following libraries have to be linked in the given order:

    embb_base_c, mtapi_c

The C header files can be included as follows:

    #include<embb/mtapi/c/mtapi.h>  or  #include<mtapi.h>
    #include<embb/base/c/base.h>


Documentation
-------------

The release files of EMB² come with a tutorial, example programs, and a
reference manual (HTML) describing the APIs. All documentation is contained in
the "doc" folder. The root document of the HTML reference is
"doc/reference/index.html". Note that generated documentation files are not
under version control and hence not contained in the repository. As mentioned
above, it is therefore recommended to download one of the packaged release
files in order to have ready-to-use documentation.


Code Quality
------------

For the C++ parts of EMB², we respect most rules of the "Google C++ Style
Guide" which are checked using the cpplint tool. However, we ignore some
rules, as they are not applicable or yield false results for this project.
For example, we respect the include order of the Google Style Guide, but use
<> instead of "" for project includes, which confuses the cpplint tool.
Moreover, we do not tolerate compiler warnings and regularly check the source
code using Cppcheck, a static analysis tool for C++.


Known Bugs and Limitations
--------------------------

- For memory management reasons, the number of threads EMB² can deal with
  is bounded by a predefined but modifiable constant (see functions
  embb_thread_get_max_count() / embb_thread_set_max_count() and class
  embb::base::Thread).


Development and Contribution
----------------------------

The EMB² team welcomes all kinds of contributions, preferably as pull requests
or patches via the development mailing lists (see above). If possible, please
refer to a current snapshot of the development branch.

EMB² is supposed to be easily portable to platforms unsupported so far. Almost
all platform specific code is located in the base_c and base_cpp modules. All
existing platform specific code is fenced by EMBB_PLATFORM_* defines.

To distinguish between compilers, EMB² currently uses the following defines:

  - EMBB_PLATFORM_COMPILER_GNUC
  - EMBB_PLATFORM_COMPILER_MSVC
  - EMBB_PLATFORM_COMPILER_UNKNOWN

Different architectures are distinguished using:

  - EMBB_PLATFORM_ARCH_X86
  - EMBB_PLATFORM_ARCH_X86_32
  - EMBB_PLATFORM_ARCH_X86_64
  - EMBB_PLATFORM_ARCH_ARM
  - EMBB_PLATFORM_ARCH_UNKNOWN

Threading APIs are switched by:

  - EMBB_PLATFORM_THREADING_WINTHREADS
  - EMBB_PLATFORM_THREADING_POSIXTHREADS

Please use these defines for new platform specific code. If additional defines
are needed, they can be defined in the config.h or cmake_config.h.in files.


Important Notes
---------------

- The MTAPI C++ interface supports automatic initialization, which allows for
  easy usage of the MTAPI C++, Algorithms, and Dataflow components. For
  performance measurements, explicit initialization is strongly recommended
  since the measurements will otherwise include the initialization time of
  MTAPI.


Links
-----

  - Multicore Association:
    http://www.multicore-association.org
  - MTAPI:
    http://www.multicore-association.org/workgroup/mtapi.php
  - CMake:
    http://www.cmake.org/
  - Google C++ Style Guide:
    http://google-styleguide.googlecode.com/svn/trunk/cppguide.html
  - cpplint:
    http://google-styleguide.googlecode.com/svn/trunk/cpplint/
  - Cppcheck:
    http://cppcheck.sourceforge.net/
