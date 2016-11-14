---
title: Usage
faq:
  - question: Where do I find the documentation?
    answer: The <code>doc</code> folder in the root directory contains a tutorial (<code>doc/tutorial/tutorial.pdf</code>), the reference manual in HTML format (<code>doc/reference/index.html</code>) as well as in PDF format (<code>doc/reference/reference.pdf</code>), and a number of examples (<code>doc/examples</code>). Note that both the documentation and the examples are only available in the release files. If you pull from the repository, you have to build them on your own (see the <a href="https://github.com/siemens/embb/blob/master/README.md">README.md</a> file for more information).
  - question: Why do I get a linker error when building my project?
    answer: "Make sure that you link all necessary libraries in the following order:
    <ul>
      <li>Windows: embb_dataflow_cpp.lib, embb_algorithms_cpp.lib, embb_containers_cpp.lib, embb_mtapi_cpp.lib, embb_mtapi_c.lib, embb_base_cpp.lib, embb_base_c.lib</li>
      <li>Linux: libembb_dataflow_cpp.a, libembb_algorithms_cpp.a, libembb_containers_cpp.a, libembb_mtapi_cpp.a, libembb_mtapi_c.a, libembb_base_cpp.a, libembb_base_c.a</li>
    </ul>"
  - question: Why is the number of threads limited?
    answer: To avoid dynamic memory allocation during operation, the number of threads EMB² can deal with is bounded by a predefined but modifiable constant (see functions <code>embb_thread_get_max_count()</code>, <code>embb_thread_set_max_count()</code> and class <code>embb::base::Thread</code>). As usual in task-based programming models, however, explicit thread creation is only recommended in rare cases, e.g., for I/O or graphical user interfaces. For all other purposes, it is most efficient to rely on the implicitly created worker threads of the task scheduler.
  - question: My application does not get faster. What can I do?
    answer: First of all, make sure that your application initializes the task scheduler explicitly. Otherwise, automatic initialization will take place which results in significant overhead during the first call of many EMB² functions, and thus, distorts timing measurements. Secondly, take into account that the speedup is limited by the sequential parts of an application according to <a href="https://en.wikipedia.org/wiki/Amdahl%27s_law">Amdahl's Law</a>. For example, even if 75% of your application (in terms of sequential runtime) are parallelized, the theoretical maximum speedup is four. Thirdly, check whether the parallel parts are CPU or memory bound. Typical examples for the latters are simple vector operations where each arithmetic operation involves a memory access. In such cases, the speedup is limited by the memory bandwith of the hardware.
  - question: What is an execution policy?
    answer: An execution policy specifies a task's priority and affinity. The latter can be used to restrict the set of cores on which a task may be executed.
  - question: How can I get help?
    answer: Create an <a href="https://github.com/siemens/embb/issues">issue</a> on GitHub labelled with 'question' (preferred way if you want to let the community know) or contact us <a href="mailto:embb.info@gmail.com">directly</a>.
---
