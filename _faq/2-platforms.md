---
title: Platforms
faq:
  - question: On which platforms does EMB² run?
    answer: "
    <ul>
      <li>Operating systems: EMB² runs on POSIX-compliant platforms as well as on Windows. Since all platform-dependent code is encapsulated in an abstraction layer (base library), the whole library can be ported to other operating systems with moderate effort (or even run bare metal).</li>
      <li>Compilers and build environment: EMB² uses CMake and requires a C/C++ compiler supporting at least the standards C99 and C++03. We regularly build with GCC, Clang, and MSVC.</li>
      <li>Processor architectures: With a recent compiler supporting at least C11/C++11, EMB² can be built on most hardware platforms (using option <code>-DUSE_C11_AND_CXX11=ON</code>). Also, EMB² provides an own implementation of atomic operations for x86 and ARM to be usable with older compilers (C99/C++03).
    </ul>"
  - question: If you target embedded systems, why do you support Windows?
    answer: While Windows is not very common in embedded systems, it is often used for development, server applications, and human machine interfaces (e.g., panels).
  - question: How can I port EMB² to a platform not yet supported out of the box?
    answer: Platform-specific code is contained in the base library and fenced using <code>EMBB_PLATFORM_*</code> defines. To port the code, add appropriate implementations for your platform. Please see <a href="https://github.com/siemens/embb/blob/master/CONTRIBUTING.md">CONTRIBUTING.md</a> for more details.
  - question: Is EMB² restricted to small embedded systems or can I also use it for server applications?
    answer: Although EMB² has been designed for embedded systems, it is not restricted to small controllers or the like. You can also use it to get the most out of &ldquo;big irons&rdquo;.
---
