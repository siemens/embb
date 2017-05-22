---
title: EMB² Version 0.5.0
date: 2016-12-22
---
Right in time for Christmas, we are proud to announce the largest update EMB² has seen so far. [Version 0.5.0](https://github.com/siemens/embb/blob/v0.5.0/CHANGELOG.md) includes several new features and improvements, most notably:

* Enhanced C++ interfaces for MTAPI task handling
* CUDA plugin for the task scheduler (experimental)
* Support for worker thread priorities
* Task priorities and affinities in dataflow networks
* Threading analysis mode using mutex-based atomics
* Improved documentation and examples

Starting with this release, we use Microsoft's AppVerifier tool for [concurrency fuzzing](https://www.microsoft.com/en-us/research/project/cuzz-concurrency-fuzzing/), a technique that explores different schedules in order to find bugs that depend on the actual execution order of threads.

Have fun!
