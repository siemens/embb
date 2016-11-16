---
title: General
faq:
  - question: What is the difference between EMB² and similar libraries?
    answer: Unlike most existing libraries, EMB² has been specifically designed for embedded systems. This includes fine-grained control over the hardware, support for task priorities and affinities, which is important for (soft) real-time applications, predictable memory consumption (no dynamic memory allocation after startup), lock- and wait-free data structures that guarantee, among other things, thread progress and signal / interrupt / termination safety, support for heterogeneous systems (SoCs), and independence of the hardware architecture.
  - question: What about OpenMP?
    answer: While OpenMP is useful for exploiting data parallelism (loops), it lacks higher-level patterns, e.g. for stream processing, and does not provide any concurrent data structures. Moreover, having its origins in high performance computing, most implementations do not take into account requirements from the embedded domain.
  - question: What is the difference between EMB² and MTAPI?
    answer: <a href="http://www.multicore-association.org/workgroup/mtapi.php">MTAPI</a> is a standard for task management in embedded multicore systems defined by the <a href="http://www.multicore-association.org/">Multicore Assocation</a>. EMB² builds on MTAPI but provides more features, namely parallel algorithms, patterns for stream processing, and concurrent containers.
  - question: Is EMB² implemented in C or C++?
    answer: The base library, which abstracts from the underlying platform, and MTAPI are implemented in C. For better usability, EMB² also provides comfortable C++ wrappers for both. The parallel algorithms, dataflow patterns, and concurrent containers are implemented in C++.
---
