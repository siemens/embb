---
title: EMB² Code Quality Measures
date: 2017-05-16
---
Since the early days of EMB² development, we have put considerable effort on ensuring high code quality and stability. This includes zero compiler warnings at highest warning level, adherence to [Google’s coding guidelines](https://google.github.io/styleguide/cppguide.html) using [cpplint](https://github.com/google/styleguide/tree/gh-pages/cpplint), unit tests with more than 90% statement coverage, automatic rule checking using [cppcheck](http://cppcheck.sourceforge.net/), and dynamic threading analysis using [Helgrind](http://valgrind.org/). Most of the tools are regularly run (at least nightly) by our internal CI server. Additionally, every commit triggers a regular build using GCC and Clang on [Travis CI](https://travis-ci.org/siemens/embb).

With the upcoming major release (version 1.0.0), we are happy to announce additional measures that aim to improve the quality of EMB² even further:

* We follow the [best practices](https://bestpractices.coreinfrastructure.org/) of the [Linux Foundation](https://www.linuxfoundation.org/)’s [Core Infrastructure Initiative](https://www.coreinfrastructure.org/), which define a maturity model for open source software. Click [here](https://bestpractices.coreinfrastructure.org/projects/654) for a detailed assessment.
* We use [Coverity Scan](https://scan.coverity.com/projects/siemens-embb) for static analysis of the source code, where we achieve a defect density of 0.0 with only 13 false positive warnings (automatically triggered on every update of the master branch).
* We verify correctness of our data structures by [checking linearizability](https://github.com/ahorn/linearizability-checker) of the involved operations.
* We employ Microsoft’s AppVerifier tool, which is part of the [Windows SDK](https://developer.microsoft.com/de-de/windows/downloads/windows-10-sdk), prior to every release for [concurrency fuzzing](https://www.microsoft.com/en-us/research/project/cuzz-concurrency-fuzzing/), a technique that helps to find subtle bugs.
