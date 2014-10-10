Embedded Multicore Building Blocks (EMB²)
=========================================

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