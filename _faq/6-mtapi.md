---
title: MTAPI
faq:
  - question: What is the difference between Tasks, Jobs, and Actions?
    answer: A Job is a piece of work, e.g. a function, with a unique identifier. An Action is an implementation of a Job and may be hardware or software-defined. Each Job can be implemented by one or more Actions. A Task represents execution of a Job resulting in the invocation of an Action with some data to be processed.
  - question: What are plugins good for?
    answer: Plugins are a technique provided by EMB² to deal with heterogeneous systems in a flexible and transparant way. They are similar to device drivers in the sense that they abstract from the hardware via unified interfaces.
  - question: Which plugins are shipped with EMB²?
    answer: At the time of this writing, EMB² provides plugins for OpenCL, CUDA, and distributed systems connected over network (sockets). We are continuously working on additional plugins&mdash;please contact us if your hardware is not yet supported.
  - question: Is the network plugin suitable for large, HPC-like clusters?
    answer: No, the main purpose of the network plugin is to enable seamless computing on systems consisting of a moderate number of devices without shared memory. Sample use cases include interacting controllers, e.g., in building or industrial automation, local meshes of IoT devices, and many others.
  - question: Can I implement my own schedulers?
    answer: "Yes. The task scheduler is implemented in <code>mtapi_c/src/embb_mtapi_scheduler_t.c</code>. Currently, there are two task stealing strategies (from different queues):
    <ul>
      <li><code>embb_mtapi_scheduler_get_next_task_vhpf</code>: high priority first</li>
      <li><code>embb_mtapi_scheduler_get_next_task_lf</code>: local queues first</li>
    </ul>
    A new strategy can be implemented by extending <code>embb_mtapi_scheduler_mode_enum</code> in <code>mtapi_c/src/embb_mtapi_scheduler_t.h</code> and by adding a call to the corresponding function in <code>embb_mtapi_scheduler_get_next_task</code>. Distribution of tasks between multiple nodes is currently implemented in a round-robin fashion (see <code>embb_mtapi_scheduler_schedule_task</code>)."
---
