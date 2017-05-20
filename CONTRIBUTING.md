Guidelines for Developing and Contributing Code
===============================================

Introduction
------------

The EMB² team welcomes all kinds of feedback and contributions. Please don't hesitate to contact us if you have any questions, comments, bug reports, suggestions for improvement, extensions or the like (see [README.md](https://github.com/siemens/embb/blob/master/README.md) for general contact information). In the following, we give an overview the main development principles and sketch how to port EMB² to other platforms. Moreover, we describe our coding and documentation guidelines that should be adhered to when contributing code.

Development
-----------

### Directory Structure

EMB² consists of several components (modules) which are organized as follows:

```
/                           -- Repository root
  CMakeLists.txt            -- Main CMake buildfile, calls component CMake buildfiles
  CMakeCommon/              -- Custom CMake functions
  doc/                      -- Documentation (tutorial, reference manual, examples)
  scripts/                  -- Scripts for packaging, running tests, ...
  COMPONENT_A/              -- Component name (e.g., 'base_c' or 'mtapi_cpp')
    CMakeLists.txt          -- Buildfile for component, called from main buildfile
    include/                -- Include directory of the component
      embb/                 -- Users shall only include files below this directory
        COMPONENT_A/        -- Component name (without suffix '_c' or '_cpp')
          C++ main headers  -- To be included by users of the C++ API
          internal/         -- Internal headers included from C++ main headers
          c/                -- C headers (main and internal), optional for C++ components
    src/                    -- Source files (including non-public headers)
    test/                   -- Unit test sources
  COMPONENT_B/              -- Other component
    ...
```

If you add a directory, e.g., for a new plugin, please don't forget to update all relevant `CMakeLists.txt` files as well as `doc/reference/Doxyfile.in` and `scripts/run_cpplint.sh`.

### Branches

There are two predefined branches in the Git repository:

- `master`: This branch contains the latest stable version of EMB², i.e., the source code has been reviewed and all tests pass successfully.
- `development`: Implementation takes place in this branch. In contrast to feature branches (see below), the source code in this branch has to be compilable. When new features are stable, the development branch is merged back into the master branch.

In addition to these two branches, there may be arbitrarily many feature branches for implementing new functionality or fixing bugs. There are no requirements on the source code in these branches. After finishing the implementation, a feature branch is merged into the development branch (make sure that the source code is still compilable afterwards).

### Contributing

Please report bugs, feature requests, etc. via GitHub (https://github.com/siemens/embb/issues). Alternatively, e.g. in case of vulnerabilities, send an email to embb.info@gmail.com. Bug fixes, extensions, etc. can be contributed as pull requests via GitHub or as patches via mail. If possible, refer to a current snapshot of the master branch and create pull requests against the *development* branch. Moreover, please include regression tests or additional unit tests that check new functionality. When signing-off a contribution (using your real name), you declare the following:

```
Developer's Certificate of Origin 1.1

By making a contribution to this project, I certify that:

(a) The contribution was created in whole or in part by me and I
    have the right to submit it under the open source license
    indicated in the file; or

(b) The contribution is based upon previous work that, to the best
    of my knowledge, is covered under an appropriate open source
    license and I have the right under that license to submit that
    work with modifications, whether created in whole or in part
    by me, under the same open source license (unless I am
    permitted to submit under a different license), as indicated
    in the file; or

(c) The contribution was provided directly to me by some other
    person who certified (a), (b) or (c) and I have not modified
    it.

(d) I understand and agree that this project and the contribution
    are public and that a record of the contribution (including all
    personal information I submit with it, including my sign-off) is
    maintained indefinitely and may be redistributed consistent with
    this project or the open source license(s) involved.
```

### Porting

EMB² is easily portable to platforms unsupported so far. Almost all platform specific code is located in the `base_c` and `base_cpp` directories, and platform specific code is fenced using `EMBB_PLATFORM_*` defines.

To distinguish between compilers, EMB² currently uses the following defines:

- EMBB_PLATFORM_COMPILER_GNUC
- EMBB_PLATFORM_COMPILER_MSVC
- EMBB_PLATFORM_COMPILER_UNKNOWN

Different architectures are distinguished using:

- EMBB_PLATFORM_ARCH_C11
- EMBB_PLATFORM_ARCH_CXX11
- EMBB_PLATFORM_ARCH_X86
- EMBB_PLATFORM_ARCH_X86_32
- EMBB_PLATFORM_ARCH_X86_64
- EMBB_PLATFORM_ARCH_ARM
- EMBB_PLATFORM_ARCH_UNKNOWN

Threading APIs are switched by:

- EMBB_PLATFORM_THREADING_WINTHREADS
- EMBB_PLATFORM_THREADING_POSIXTHREADS

Please use these defines for new platform specific code. If additional defines are needed, they can be included in the `config.h` or `cmake_config.h.in` files.

A list of macros to check the underlying platform, compiler versions, etc. can be found here: http://sourceforge.net/p/predef/wiki/Home/

Coding Guidelines
-----------------

### General

- Restrict dynamic memory allocation to object construction time. A (bounded)
queue, for example, shall only allocate memory in the constructor but not during
operation, i.e., in the methods for pushing and popping elements.
- Use assertions to catch bugs (always think what could theoretically happen).
- Use exceptions to catch invalid user input (by the `EMBB_THROW` macro).
- Use concepts instead of interfaces unless virtual functions are necessary.
- Use `const` whenever it makes sense.
- Use pointers only if they can be `NULL`, otherwise use const/non-const references.
- Use `size_t` if the number of elements, indices, etc. depends on a pointer (`size_t` has the same size as a pointer).
- For iterators, use `first` and `last` (as in STL), not `begin` and `end`.
- Use the same order of functions etc. in the source files as in the corresponding header files.
- Be aware of false sharing and align objects when appropriate.
- Disable construction, copy construction, and assignment whenever it makes sense by declaring the corresponding functions private without giving a definition.
- For headers, use `#include <...>` instead of `#include "..."`.
- Include paths have the format `#include <embb/component_name/...>`, e.g., `#include <embb/base/internal/some_header.h>`.
- In C code, use the prefix `embb_component_` for globally visible symbols. For example, the thread creation function is named `embb_base_ThreadCreate`.
- Similarly, use the prefix `EMBB_COMPONENT_` for preprocessor macros.

### Tool Support

- All source files in the repository must have LF (Unix) line endings. Git can take care of this (using the following option, newly checked-in files and changes to them will automatically be converted from CRLF to LF if necessary):
```
  git config --global core.autocrlf input
```
- For the C++ parts of EMB², we follow [Google's C++ style guide](https://google.github.io/styleguide/cppguide.html) which can be checked using the [cpplint](https://raw.githubusercontent.com/google/styleguide/gh-pages/cpplint/cpplint.py) tool. However, we ignore some rules as they are not applicable or yield false results for this project. For example, we respect the include order of the Google style guide, but use <> instead of "" for project includes (see above). To check whether your code adheres to the style guide, use the `run_cpplint.sh` script containted in the `scripts` folder. You may use [clang-format](http://clang.llvm.org/docs/ClangFormat.html) with option `-style=Google` to pretty print your code but be aware that line breaking of Doxygen comments may not work properly.
- Moreover, we regularly check the code using [Cppcheck](http://cppcheck.sourceforge.net/), a light-weight static analysis tool for C/C++. To run Cppcheck on all files in a certain directory, call it as follows:
```
  cppcheck --enable=warning,style,performance,portability --inconclusive <directory>
```
- We do not accept compiler warnings with a few exceptions when using MSVC (see below). By default, warnings are enabled using `-Wall -Wextra` (GCC) or `/Wall` (MSVC). To make sure that no warnings are overlooked, you can treat warnings as errors by setting `WARNINGS_ARE_ERRORS` to `ON`, for example:
```
  cmake -g "Unix Makefiles" .. -DWARNINGS_ARE_ERRORS=ON
```
- Use the following scheme to disable inappropriate MSVC warnings:
```c++
#ifdef EMBB_COMPILER_MSVC
// Suppress <brief description> warning
#pragma warning(push)
#pragma warning(disable : 4265) // 4265 is an example warning number
#endif
// Code that produces the warning (should consist of only very few lines)
#ifdef EMBB_COMPILER_MSVC
#pragma warning(pop) // Reset warning 4265
#endif
```

Documentation Guidelines
------------------------

### General

The source code is documented using [Doxygen](http::www.doxygen.org/). Please adhere to the following rules:

- Document at least all entities visible to the user (API).
- Member variables need only be documented if their names are not self-explanatory.
- Check whether Doxygen emits any warnings or errors (e.g., undocumented functions).
- Enable spell checking in your IDE and proofread the documentation generated by Doxygen.
- Use full stops at the end of complete sentences (and only there).
- The first sentence ending with a full stop is parsed as brief description by Doxygen.
- Use `\` instead of `@` for Doxygen commands.
- Typeset code fragments including constants such 'true' and 'false' in typewriter font using `\c` (example: `returns \c true if ...`).
- Use `<tt>...</tt>` instead of `\c` for complex expressions that include, for example, braces (otherwise, the code might not be formatted correctly).
- Use `@code` for multiple lines of code (examples etc.).
- Document parameters in place (after the parameter) using `/**< [in,out,in/out] ... documentation ... */`
- Refer to functions by adding braces after the name (example: `Fun()` but not just `Fun`).
- Explicit or implicit dynamic memory allocation must be documented using the `\memory` command (see below).

### Special Commands

Use special commands to specify properties important in embedded systems:

- `\memory`: Use if and only if a function/method dynamically allocates memory. Give a short comment and optionally specify the asymptotic memory consumption.
- `\notthreadsafe`, `\threadsafe`, `\lockfree`, `\waitfree`: Always use one (!) of these commands to specify the behaviour related to concurrent execution. Note that `\lockfree` includes `\threadsafe` and `\waitfree` includes `\lockfree`.
- `\threadsafe` means that a shared state (e.g., the member variables of an object, but also pointers/references passed as arguments to a function) is accessed in a synchronized way. This implies that a C function that gets `const` pointers as arguments is not thread-safe if there are other functions that can modify the arguments concurrently. Similarly, if a method doesn't modify the state of an object, but other methods are able to do so, the method is not thread-safe.
- `\lockfree` means that at least one thread is always guaranteed to make progress, and `\waitfree` means that all threads are guaranteed to always make progress. A more detailed classification can be found in "M. Herlihy and N. Shavit. *On the nature of progress*. Principles of Distributed Systems (OPODIS'11), Springer-Verlag, 2011".

### Structure

The following sequence of descriptions and commands shall be obeyed to achieve a consistent layout of the documentation:

1. Brief description ending with a full stop (without `\brief`)
2. More detailed description [optional]
3. `\pre`: Preconditions that must hold when calling the function [optional]
4. `\post`: Postconditions that holld after calling the function [optional]
5. `\return`: Description of return value [optional]
6. `\throws`: Thrown exceptions (repeat for each exception) [optional]
7. `\memory`: Dynamic memory allocation (see above) [optional]
8. `\notthreadsafe`, `\threadsafe`, `\lockfree`, `\waitfree`: Thread safety and progress guarantees (see above)
9. `\note`: Additional notes/comments [optional]
10. `\see`: Links to other related functions, types, etc. [optional]
11. `\tparam`: Template parameters [optional]

### Example

The example shown below demonstrates how to document a class according to the given rules:

```c++
/**
 * Concurrent queue.
 * \tparam Type Type of the queue elements
 */
template<typename Type>
class Queue {
 public:
  /**
   * Creates a queue with the specified capacity.
   * \memory Allocates \c capacity elements of type \c Type.
   * \notthreadsafe
   */
  Queue(
    size_t capacity
    /**< [IN] Capacity of the queue */
  );

  /**
   * Returns the capacity of the queue.
   * \return Number of elements the queue can hold
   * \waitfree
   */
  size_t GetCapacity();

  /**
   * Tries to enqueue an element.
   * \return \c true if the element could be enqueued, otherwise \c false
   * \threadsafe
   */
  bool TryEnqueue(
    Type const & element
    /**< [IN] Const reference to the element that shall be enqueued */
  );
};
```
