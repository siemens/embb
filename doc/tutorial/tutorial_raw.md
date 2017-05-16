# Embedded Multicore Building Blocks Tutorial

## Contents

[**Introduction**](#cha_introduction)  
&nbsp;&nbsp;[Overview](#sec_introduction_overview)  
&nbsp;&nbsp;[Outline](#sec_introduction_outline)  
&nbsp;&nbsp;[Functions, Functors, and Lambdas](#sec_introduction_function_objects)  

[**Algorithms**](#cha_algorithms)  
&nbsp;&nbsp;[Function Invocation](#sec_algorithms_invoke)  
&nbsp;&nbsp;[Sorting](#sec_algorithms_sorting)  
&nbsp;&nbsp;[Counting](#sec_algorithms_counting)  
&nbsp;&nbsp;[Foreach Loops](#sec_algorithms_foreach)  
&nbsp;&nbsp;[Reductions](#sec_algorithms_reductions)  
&nbsp;&nbsp;[Prefix Computations](#sec_algorithms_prefix)  

[**Dataflow**](#cha_dataflow)  
&nbsp;&nbsp;[Linear Pipelines](#sec_dataflow_linear_pipelines)  
&nbsp;&nbsp;[Nonlinear Pipelines](#sec_dataflow_nonlinear_pipelines)  

[**Containers**](#cha_containers)  
&nbsp;&nbsp;[Object Pools](#sec_containers_object_pools)  
&nbsp;&nbsp;[Stacks](#sec_containers_stacks)  
&nbsp;&nbsp;[Queues](#sec_containers_queues)  

[**MTAPI**](#cha_mtapi)  
&nbsp;&nbsp;[Foundations](#sec_mtapi_foundations)  
&nbsp;&nbsp;[C Interface](#sec_mtapi_c_interface)  
&nbsp;&nbsp;[C++ Interface](#sec_mtapi_cpp_interface)  
&nbsp;&nbsp;[Plugins](#sec_mtapi_plugins)  

[**Heterogeneous Systems**](#cha_heterogeneous_systems)  
&nbsp;&nbsp;[Algorithms](#sec_algorithms_heterogeneous_systems)  
&nbsp;&nbsp;[Dataflow](#sec_dataflow_heterogeneous_systems)  

[**Tutorial Application**](#cha_tutorial_application)  

[**Bibliography**](#cha_bibliography)  


## <a name="cha_introduction"></a>Introduction

### <a name="sec_introduction_overview"></a>Overview

The Embedded Multicore Building Blocks (EMB²) are an easy to use yet powerful and efficient C/C++ library for the development of parallel applications. EMB² has been specifically designed for embedded systems and the typical requirements that accompany them, such as real-time capability and constraints on memory consumption. As a major advantage, low-level operations are hidden in the library which relieves software developers from the burden of thread management and synchronization. This not only improves productivity of parallel software development, but also results in increased reliability and performance of the applications.

EMB² is independent of the hardware architecture (x86, ARM, ...) and runs on various platforms, from small devices to large systems containing numerous processor cores. It builds on MTAPI, a standardized programming interface for leveraging task parallelism in embedded systems containing symmetric or asymmetric (heterogeneous) multicore processors. A core feature of MTAPI is low-overhead scheduling of fine-grained tasks among the available cores during runtime. Unlike existing libraries, EMB² supports task priorities and affinities, which allows the creation of soft real-time systems. Additionally, the scheduling strategy can be optimized for non-functional requirements such as minimal latency and fairness.

Besides the task scheduler, EMB² provides basic parallel algorithms, concurrent data structures, and skeletons for implementing stream processing applications (see [Figure 1](#fig_embb)). These building blocks are largely implemented in a non-blocking fashion, thus preventing frequently encountered pitfalls like lock contention, deadlocks, and priority inversion. As another advantage in real-time systems, the algorithms and data structures give certain progress guarantees. For example, wait-free data structures guarantee system-wide progress which means that every operation completes within a finite number of steps independently of any other concurrent operations on the same data structure.

![Figure 1: Main Building Blocks of EMB²](md_fig_embb.png)
<a name="fig_embb"></a>**Figure 1**: Main Building Blocks of EMB²

### <a name="sec_introduction_outline"></a>Outline

The purpose of this document is to introduce the basic concepts of EMB² and to demonstrate typical application scenarios by means of simple examples. The tutorial is not intended to be complete in the sense that it describes every feature of EMB². For a detailed description of the API, please see the reference manual.

In the next subsection, we briefly describe the concept of [function objects](#sec_introduction_function_objects) which is essential for using EMB². We then present various parallel [algorithms](#cha_algorithms) and the [dataflow](#cha_dataflow) framework. After that, we explain the usage of [MTAPI](#cha_mtapi) and how to leverage [heterogeneous systems](#cha_heterogeneous_systems). The complete source code for the examples presented in the following can be found in the `examples` directory.

### <a name="sec_introduction_function_objects"></a>Functions, Functors, and Lambdas

Throughout this tutorial, we will encounter C++ types which model the C++ concept `FunctionObject`. The function object concept comprises function pointer, functor, and lambda types that are callable with suitable arguments by the function call syntax. Given a function object `f` and arguments `arg1`, `arg2`, `...`, the expression `f(arg1, arg2, ...)` is a valid function invocation. If you are already familiar with function objects, you can safely skip the rest of this section. Otherwise, it might be worth reading it to get an idea of what is meant when talking about a function objects.

Consider, for example, the transformation of an iterable range of data values. Specifically, consider a vector of integers initialized as follows:

    \\\inputlistingsnippet{../examples/stl_for_each/stl_for_each.cc:setup}

The range consists of the values `(1, 2, 3, 4, 5)`. To double each value, we could simply iterate over the vector as follows:

    \\\inputlistingsnippet{../examples/stl_for_each/stl_for_each.cc:manual}

The range then contains the values `(2, 4, 6, 8, 10)`. In order to demonstrate the concept of function objects, we are now going to use the `std::for_each` function defined in the `algorithm` header of the C++ Standard Library. This function accepts as argument a `UnaryFunction`, that is, a function object which takes only one argument. In case of `std::for_each`, the argument has to have the same type as the elements in the range, as these are passed to the unary function. In our example, the unary function’s task is to double the passed value. We could define a function for that purpose:

    \\\inputlistingsnippet{../examples/stl_for_each/stl_for_each.cc:function_define}

Since a function pointer models the concept of function objects, we can simply pass `&DoubleFunction` to `std::for_each`:

    \\\inputlistingsnippet{../examples/stl_for_each/stl_for_each.cc:function_run}

Another possibility is to define a functor

    \\\inputlistingsnippet{../examples/stl_for_each/stl_for_each.cc:functor_define}

and to pass an instance of this class to `std::for_each`:

    \\\inputlistingsnippet{../examples/stl_for_each/stl_for_each.cc:functor_run}

Functors as well as function pointers separate the actual implementation from its usage which can be helpful if the functionality is needed at different places. In many cases, however, it is easier to have the implementation of the function object at the same place as it is used. C++11 provides lambda expressions for that purpose which make our example more concise:

    \\\inputlistingsnippet{../examples/stl_for_each/stl_for_each.cc:lambda}

Of course, this example is too simple to really benefit from function objects and the algorithms contained in the C++ Standard Library. However, in combination with the parallelization features provided by EMB², function objects are very useful. Within this document, whenever a function object or one of its subtypes is required, one can use a function pointer, a functor, or a lambda. For simplicity, we will restrict ourselves to lambdas in subsequent examples, as they are most suitable for this tutorial.

## <a name="cha_algorithms"></a>Algorithms

The *Algorithms* building block of EMB² provides high-level constructs for typical parallelization tasks. They are similar to the functions provided by the C++ Standard Library, but contain additional functionality typical for embedded systems such as task priorities. Although the algorithms can be used in a black-box way, it is good to have a basic understanding of their implementation: The algorithms split computations to be performed in parallel into tasks which are executed by the MTAPI task scheduler (see chapter on [MTAPI](#cha_mtapi)). For that purpose, the tasks are stored in queues and mapped to a fixed number of worker threads at runtime.

_**Note:** The algorithms are implemented using the MTAPI C++ interface. Since MTAPI allocates the necessary data structures during initialization, the maximum number of tasks in flight is fixed. In case one of the algorithms exceeds this limit, an exception is thrown. By calling `embb::mtapi::Node::Initialize`, the maximum number of tasks and other limits can be customized. Explicit initialization also eliminates unexpected delays when measuring performance. See the section on the [MTAPI C++ Interface](#sec_mtapi_cpp_interface) for details._

### <a name="sec_algorithms_invoke"></a>Function Invocation

Let us start with the parallel execution of several work packages encapsulated in functions. Suppose that the following functions operate on different data sets and are thus independent of each other:

    \\\inputlistingsnippet{../examples/algorithms/invoke.cc:packages}

The functions can be executed in parallel using the `ParallelInvoke` construct provided by EMB²:

    \\\inputlistingsnippet{../examples/algorithms/invoke.cc:invokation}

Note that `ParallelInvoke` waits until all its arguments have finished execution.

Next, let us consider a more elaborate example. The following piece of code shows a sequential implementation of the quicksort algorithm, which we want to parallelize (do not care about the details of the `Partition` function for the moment):

    \\\inputlistingsnippet{../examples/algorithms/invoke.cc:quick_sort}

A straightforward approach to parallelize this algorithm is to execute the recursive calls to `Quicksort` in parallel. With `ParallelInvoke` and lambdas, it is as simple as that:

    \\\inputlistingsnippet{../examples/algorithms/invoke.cc:parallel_quick_sort}

The lambdas capture the `first`, `mid`, and `last` pointers to the range to be sorted and forward them to the recursive calls of quicksort. These are executed in parallel, where `Invoke` does not return before both have finished execution. The above implementation of parallel quicksort is not yet optimal. In particular, the creation of new tasks should be stopped when a certain lower bound on the size of the subranges has been reached. The subranges can then be sorted sequentially in order to reduce the overhead for task creation and management. Fortunately, EMB² already provides solutions for parallel sorting, which will be covered in the following section.

### <a name="sec_algorithms_sorting"></a>Sorting

For systems with constraints on memory consumption, the quicksort implementation provided by EMB² is usually the best choice, since it works in-place, which means that it does not require additional memory. Considering real-time systems, however, its worst-case runtime of _O(n<sup>2</sup>)_, where _n_ is the number of elements to be sorted, can be a problem. For this reason, EMB² also provides a parallel merge sort algorithm. Merge sort does not work in-place, but has a predictable runtime complexity of _ϴ(n log n)_. Assume we want to sort a vector of integers:

    \\\inputlistingsnippet{../examples/algorithms/sorting.cc:range_define}

Using quicksort, we simply write:

    \\\inputlistingsnippet{../examples/algorithms/sorting.cc:quick_sort}

The default invocation of `QuickSort` uses `std::less` with the iterators’ `value_type` as comparison operation. As a result, the range is sorted in ascending order. It is possible to provide a custom comparison operation, for example `std::greater`, by passing it as a function object to the algorithm. Sorting the elements in descending can be accomplished as follows:

    \\\inputlistingsnippet{../examples/algorithms/sorting.cc:custom_quick_sort}

The merge sort algorithm comes in two versions. The first version automatically allocates dynamic memory for temporary values when the algorithm is called. Its name is `MergeSortAllocate` and it has the same parameters as `QuickSort`. To enable the use of merge sort in environments that forbid dynamic memory allocation after initialization, the second version can be called with a pre-allocated temporary range of values:

    \\\inputlistingsnippet{../examples/algorithms/sorting.cc:merge_sort_preallocated}

The temporary range can be allocated at any time, e.g., during the initialization phase of the system.

### <a name="sec_algorithms_counting"></a>Counting

EMB² also provides functions for counting the number of elements in a range. Consider a range of integers from 0 to 3:

    \\\inputlistingsnippet{../examples/algorithms/counting.cc:setup}

To determine how often a specific value appears within the range, we could simply iterate over it and compare each element with the specified one. The `Count` function does this in parallel, where the first two arguments specify the range and the third one the element to be counted:

    \\\inputlistingsnippet{../examples/algorithms/counting.cc:count}

For the range given above, we have `count == 2`.

In case the comparison operation is not equality, we can employ the `CountIf` function. Here, the third argument is a unary predicate which evaluates to `true` for each element to be counted. The following example shows how to count the number of values greater than 0:

    \\\inputlistingsnippet{../examples/algorithms/counting.cc:if_count}

### <a name="sec_algorithms_foreach"></a>Foreach Loops

A frequently encountered task in parallel programming is to apply some operation to a range of values, as illustrated previously. In principle, one could apply the operation to all elements in parallel provided that there are no data dependencies. However, this results in unnecessary overhead if the number of elements is greater than the number of available processor cores _p_. A better solution is to partition the range into _p_ blocks and to process the elements of a block sequentially. With the `ForEach` construct provided by EMB², users do not have to care about the partitioning, since this is done automatically. Similar to the Standard Library’s `for_each` function, it is sufficient to pass the operation in form of a function object. The following piece of code shows how to double the elements of a range in parallel:

    \\\inputlistingsnippet{../examples/algorithms/for_each.cc:doubling}

There is also a `ForLoop` variant that accepts integers as limits of the range:

    \\\inputlistingsnippet{../examples/algorithms/for_each.cc:loop_doubling}

In the above code snippet, the results of the computation overwrite the input. If the input has to be left unchanged, the results must be written to a separate output range. Thus, the operation requires two ranges. EMB² supports such scenarios by the `ZipIterator`, which wraps two iterators into one. Consider the following revised example:

    \\\inputlistingsnippet{../examples/algorithms/for_each.cc:zip_setup}

Using the `Zip` function as a convenient way to create a zip iterator, the doubling of elements can be performed as follows:

    \\\inputlistingsnippet{../examples/algorithms/for_each.cc:zip_doubling}

The argument to the lambda function is a `ZipPair` with the iterators’ reference value as template parameters. The elements pointed to by the zip iterator can be accessed via `First()` and `Second()`, similar to `std::pair`.

### <a name="sec_algorithms_reductions"></a>Reductions

As mentioned in the previous section, the `ForEach` construct requires the loop iterations do be independent of each other. However, this is not always the case. Imagine we want to sum up the values of a range, e.g., a vector of integers:

    \\\inputlistingsnippet{../examples/algorithms/reduce.cc:range_init}

Sequentially, this can be done by a simple loop:

    \\\inputlistingsnippet{../examples/algorithms/reduce.cc:sequential}

One might be tempted to sum up the elements in parallel using a foreach loop. The problem is that parallel accesses to `sum` must be synchronized to avoid race conditions, which in fact sequentializes the loop. A more efficient approach is to compute intermediate sums for each block of the range and to sum them up at the end. For such purposes, EMB² provides the function `Reduce`:

    \\\inputlistingsnippet{../examples/algorithms/reduce.cc:parallel}

The third argument to `Reduce` is the neutral element of the reduction operation, i.e., the element that does not change the result. In case of addition (`std::plus`), the neutral element is 0. If we wanted to compute the product of the vector elements, the neutral element would be 1.

Next, let us consider the parallel computation of a dot product. Given two input ranges, we want to multiply each pair of input elements and sum up the products. The second input range is given as follows:

    \\\inputlistingsnippet{../examples/algorithms/reduce.cc:second_range_init}

The reduction consists of two steps: First, the input ranges are transformed and then, the reduction is performed on the transformed range. For that purpose, the `Reduce` function allows to specify a transformation function object. By default, this is the identity functor which does not modify the input range. To implement the dot product, we can use the `Zip` function and a lambda function for computing the transformed range:

    \\\inputlistingsnippet{../examples/algorithms/reduce.cc:dot_product}

### <a name="sec_algorithms_prefix"></a>Prefix Computations

Prefix computations (or scans) can be viewed as a generalization of reductions. They transform an input range _x<sub>i</sub> ϵ X_ into an output range _y<sub>i</sub> ϵ Y_ with _i=1,...,n_ such that

_y<sub>0</sub> = id · x<sub>0</sub>_  
_y<sub>1</sub> = y<sub>0</sub> · x<sub>1</sub>_  
&nbsp;&nbsp;⁞  
_y<sub>i</sub> = y<sub>i-1</sub> · x<sub>i</sub>_  
&nbsp;&nbsp;⁞  
_y<sub>n</sub> = y<sub>n-1</sub> · x<sub>n</sub>_,  

where _id_ is the identity (neutral element) with respect to the operation _·: X_ <sup><sub>x</sub></sup> _X → Y_. As an example, consider the following range:

    \\\inputlistingsnippet{../examples/algorithms/scan.cc:setup}

Computing the prefix sums of `input_range` sequentially is easy:

    \\\inputlistingsnippet{../examples/algorithms/scan.cc:sequential_prefix_sum}

Note the dependency on loop iteration _i-1_ to compute the result in iteration _i_. A special two-pass algorithm is used in the function `Scan` to perform prefix computations in parallel. Using `Scan` to compute the prefix sums, we get:

    \\\inputlistingsnippet{../examples/algorithms/scan.cc:prefix_sum}

As in the case of reductions, the neutral element has to be given explicitly. Also, a transformation function can be passed as additional argument to `Scan`. The elements of the input range are then transformed before passed to the prefix operation.


## <a name="cha_dataflow"></a>Dataflow

EMB² provides generic skeletons for the development of parallel stream-based applications. These skeletons are based on dataflow networks, a model of computation widely employed in different domains like digital signal processing and imaging due to its simplicity and flexibility. As a major advantage, these networks are deterministic which significantly simplifies testing and debugging. This is particularly important in embedded systems, where high demands are put on correctness and reliability. Moreover, they are inherently parallel and lend themselves well for execution on a multicore processor. In fact, they can be viewed as a generalization of pipelining, a frequently encountered parallel pattern.

_**Note:** Dataflow networks are internally implemented using the MTAPI C++ interface. Since MTAPI does not allocate memory after initialization, the number of tasks and other resources are limited. By calling `embb::mtapi::Node::Initialize`, these limits can be customized. Explicit initialization also eliminates unexpected delays when measuring performance. See the section on the [MTAPI C++ Interface](#sec_mtapi_cpp_interface) for details._

### <a name="sec_dataflow_linear_pipelines"></a>Linear Pipelines

Before we go into detail, we demonstrate the basic concepts of dataflow networks by means of a simple application which finds and replaces strings in a file. Let us start with the sequential implementation. The program shown in [Listing 1](#lst_replace_seq) reads a file line by line and replaces each occurrence of a given string with a new string.

```cpp
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

using namespace std;

// replace all ocurrences of 'what' in 'str' with 'with'
void repl(string& str, const string &what,
          const string& with) {
  string::size_type pos = 0;
  while((pos = str.find(what, pos)) != string::npos) {
    str.replace(pos, what.length(), with);
    pos += with.length();
  }
}

int main(int argc, char *argv[]) {
  // check and read command line arguments
  if(argc != 4) {
    cerr << "Usage: replace <what> <with> <file>" << endl;
    exit(EXIT_FAILURE);
  }
  const string what(argv[1]), with(argv[2]);

  // open input file
  ifstream file(argv[3]);
  if(!file) {
    cerr << "Cannot open file " << argv[3] << endl;
    exit(EXIT_FAILURE);
  }

  // read input file line by line and replace strings
  string str;
  while(getline(file, str)) {
    repl(str, what, with);
    cout << str << endl;
  }

  // close file and exit
  file.close();
  exit(EXIT_SUCCESS);
}
```

<a name="lst_replace_seq"></a>**Listing 1**: Sequential program for replacing strings in a file

The main part consists of the `while` loop which performs three steps:

1. read a line from `file` and store it in the string `str`
2. replace each occurrence of `what` in `str` with `with`
3. write the resulting string to `cout`

To run this program on a multicore processor, we may execute the above steps in a pipelined fashion. In this way, a new line can be read from the hard disk while the previous one is still being processed. Likewise, processing a string and writing the result to standard output can be performed in parallel. Thus, the pipeline may consist of three stages as depicted in [Figure 2](#fig_replace_par).

![Figure 2: Pipeline for replacing strings in a file](md_fig_replace_par.png)
<a name="fig_replace_par"></a>**Figure 2**: Pipeline for replacing strings in a file

This pipeline can be easily implemented using the dataflow networks. As the first step, we have to include the `dataflow.h` header file:

    \\\inputlistingsnippet{../examples/dataflow/dataflow_linear.cc:dataflow_include}

Then, we have to construct a network. A network consists of a set of processes that are connected by communication channels. EMB² provides a class `Network` that handles data routing and scheduling of your processes:

    \\\inputlistingsnippet{../examples/dataflow/dataflow_linear.cc:dataflow_network}

We need to prepare the network for the desired maximum number of elements that can be in the network at a time. The number of elements is limited to avoid that the network is flooded with new elements before the previous elements have been processed. In a linear pipeline, for example, this may happen if the source is faster than the sink. For our example we assume that at most four elements may be processed simultaneously: one in the source, one in the sink, and two in the middle stage.

    \\\inputlistingsnippet{../examples/dataflow/dataflow_linear.cc:dataflow_make}

Finding an optimal value depends on the application and usually requires some experimentation. In general, large values boost the throughput but also increase the latency. Conversely, small values reduce the latency but may lead to a drop of performance in terms of throughput.

As the next step, we have to construct the processes shown in [Figure 2](#fig_replace_par). The easiest way to construct a process is to wrap the user-defined code in a lambda function and to pass it to the network. The network constructs an object for that process and executes the lambda function whenever new data is available. There are several methods for constructing processes depending on their type. The process **read** is a *source* process, since it produces data (by reading it from the specified file) but does not consume any data. Source processes are constructed from a function object

    \\\inputlistingsnippet{../examples/dataflow/dataflow_linear.cc:dataflow_source_function}

like this:

    \\\inputlistingsnippet{../examples/dataflow/dataflow_linear.cc:dataflow_declare_source}

Note the template argument `std::string` to `Source`. This tells that the process has exactly one *port* of type `std::string` and that this port is used to transmit data to other processes. The user-defined code can access the ports via the parameters of the function. Thus, each parameter corresponds to exactly one port. In our example, the result of the process is stored in a variable `str`, which is passed by reference.

The replacement of the strings can be done by a *parallel* process, which means that multiple invocations of the process may be executed simultaneously. In general, processes that neither have any side effects nor maintain a state can safely be executed in parallel. This helps to avoid bottlenecks that arise when some processes are faster than others. Suppose, for example, that **replace** requires up to 50 ms to execute, whereas **read** and **write** each require 10 ms to execute. If only one invocation of **replace** could be executed at a time, the throughput would be at most 20 elements per second. Since **replace** is a parallel process, however, the network may start a new invocation every 10 ms. Hence, up to five invocations may be executed in parallel, yielding a throughput of 100 elements per second. To compensate for variations in the runtime of parallel stages, they may be executed *out-of-order*. As a result, the order in which the elements of a stream enter and leave parallel stages is not necessarily preserved. In our example, the runtime of **replace** may vary significantly due to the fact that not all lines have the same length and that the number of replacements depends on the content. Before we go into more detail, let us first consider the following function

    \\\inputlistingsnippet{../examples/dataflow/dataflow_linear.cc:dataflow_replace_function}

and how to construct the corresponding **replace** process:

    \\\inputlistingsnippet{../examples/dataflow/dataflow_linear.cc:dataflow_declare_replace}

The template parameter `Network::Inputs<std::string>` specifies that the process has one port serving as input. Analogously, `Network::Outputs<std::string>` specifies that there is one port serving as output.

Since the last process (**write**) does not have any outputs, we make it a *Sink*. Unlike parallel processes, sinks are always executed *in-order*. EMB² takes care that the elements are automatically reordered according to their original order in the stream. This way, the externally visible behavior is preserved even if some parallel stages may be executed out-of-order. The function

    \\\inputlistingsnippet{../examples/dataflow/dataflow_linear.cc:dataflow_sink_function}

is used to construct the sink:

    \\\inputlistingsnippet{../examples/dataflow/dataflow_linear.cc:dataflow_declare_sink}

_**Note:** If you parallelize an application using EMB² and your compiler emits a lengthy error message containing lots of templates, it is very likely that for at least one process, the ports and their directions do not match the signature of the given function._

As the last step, we have to connect the processes (ports). This is straightforward using the C++ stream operator:

    \\\inputlistingsnippet{../examples/dataflow/dataflow_linear.cc:dataflow_connect}

Then, we can start the network:

    \\\inputlistingsnippet{../examples/dataflow/dataflow_linear.cc:dataflow_run}

Note that you will probably not observe a speedup when you run this program on a multicore processor. One reason for this is that I/O operations like reading a file from the hard disk and writing the output to the screen are typically a bottleneck. Moreover, the amount of work done in the middle stage of the pipeline (**replace**) is rather low. To outweigh the overhead for parallel execution, the amount of work must be much higher. In image processing, for example, a single pipeline stage may process a complete image. To sum up, we have chosen this example for its simplicity, not for its efficiency.

### <a name="sec_dataflow_nonlinear_pipelines"></a>Nonlinear Pipelines

Some applications exhibit a more complex structure than the linear pipeline presented in the previous section. Typical examples are applications where the result of a pipeline stage is used by more than one successor stage. Such pipelines are said to be nonlinear. In principle, every nonlinear pipeline can be transformed to a linear one as depicted in [Figure 3](#fig_linearization). However, this increases the latency and complicates the implementation due to data that must be passed through intermediate stages. In [Figure 3](#fig_linearization), for example, the data transferred from stage A to stage C must be passed through stage B in the linearized implementation.

![Figure 3: Nonlinear pipeline and linearized variant](md_fig_linearization.png)
<a name="fig_linearization"></a>**Figure 3**: Nonlinear pipeline and linearized variant

Nonlinear pipelines can be implemented as they are using EMB², i.e., there is need not linearize them. As an example, let us consider the implementation of a sorting network. Sorting networks consist of a set of interconnected comparators and are used to sort sequences of data items. As depicted in [Figure 4](#fig_comparator), each comparator sorts a pair of values by putting the smaller value to one output, and the larger one to the other output. Thus, a comparator can be viewed as a switch that transfers the values at the inputs to the outputs, either directly or by swapping them (cf. [Figure 5](#fig_example_comparator)).

![Figure 4: Block diagram of a comparator](md_fig_comparator.png)
<a name="fig_comparator"></a>**Figure 4**: Block diagram of a comparator

![Figure 5: Example for the operating principle of a comparator](md_fig_example_comparator.png)
<a name="fig_example_comparator"></a>**Figure 5**: Example for the operating principle of a comparator

[Figure 6](#fig_sorting_network) shows a sorting network with four inputs/outputs and five comparators. The numbers at the interconnections exemplify a “run” of the network. As can be seen from [Figure 6](#fig_sorting_network), the comparators _C<sub>1</sub>-C<sub>4</sub>_ “sink” the largest value to the bottom and “float” the smallest value to the top. The final comparator _C<sub>5</sub>_ simply sorts out the middle two values. This way it is guaranteed that the values at the outputs occur in ascending order.

![Figure 6: Sorting network with four inputs=outputs and five comparators](md_fig_sorting_network.png)
<a name="fig_sorting_network"></a>**Figure 6**: Sorting network with four inputs/outputs and five comparators

Let us now consider the implementation of the sorting network using EMB². As in the previous example, we need three types of processes: one or more sources that produce a stream of data items, a total number of five processes that implement the comparators, and one or more sinks that consume the sorted sequences. The processes should be generic so that they can be used with different types. For example, in one application we might use the network to sort integers, and in another application to sort floating point values.

The following listing shows the implementation of the source processes using classes instead of functions (a complete implementation can be found in the examples directory):

    \\\inputlistingsnippet{../examples/dataflow/dataflow_nonlinear.cc:dataflow_producer}

The class-based approach has several advantages besides the use of templates: Firstly, the creation of multiple processes is straightforward. Secondly, one can derive other processes from a given base class such as `Producer`. Thirdly, it eases migration of existing code. For example, if you want to use an object of an existing class `foo` as a process, you might derive a class `bar` from `foo` implementing any missing functionality.

To feed our sorting network `nw` with four streams of integer values, we may write:

    \\\inputlistingsnippet{../examples/dataflow/dataflow_nonlinear.cc:dataflow_declare_add_sources}

The code for the comparators looks like this:

    \\\inputlistingsnippet{../examples/dataflow/dataflow_nonlinear.cc:dataflow_comparator}

Since the comparators neither have any side effects nor maintain a state, we allow multiple invocations to be executed in parallel.

To check whether the resulting values are sorted, we use a single sink with four inputs:

    \\\inputlistingsnippet{../examples/dataflow/dataflow_nonlinear.cc:dataflow_consumer}

We could also have a sink for each output of the sorting network. There is no restriction on the number of sources and sinks a network may have.


## <a name="cha_containers"></a>Containers

Containers are essential for storing objects in an organized way. Unfortunately, the containers provided by the C++ Standard Library are not thread-safe. Attempts to read and write elements concurrently may corrupt the stored data. While such undefined behavior can be avoided by synchronizing all accesses using a mutex, this largely limits the available parallelism.

The containers provided by EMB² enable a high degree of parallelism by design. They are implemented in a lock-free or wait-free fashion, thus avoiding any blocking operations. This way, multiple threads or tasks may access a container concurrently without suffering from typical side effects like convoying. Wait-free algorithms even guarantee that an operation completes within a bounded number of steps. Consequently, threads are immune to starvation which is critical for real-time systems.

In embedded systems, memory is often preallocated in the initialization phase to minimize the effort for memory management during operation and to prevent unpredictable out-of-memory errors. EMB² containers have a fixed capacity and allocate the required memory at construction time. Consequently, they can be used in safety-critical application, where dynamic memory allocation after initialization is forbidden.

### <a name="sec_containers_object_pools"></a>Object Pools

An object pool allocates a fixed number of objects at construction. Objects can then be allocated from the pool and returned for later reuse. When implementing lock-free or wait-free algorithms, the underlying memory allocation scheme also has to be lock-free or wait-free, respectively. However, memory allocation functions such as `new` and `delete` usually do not give any progress guarantees. To solve this problem, EMB² provides lock-free and wait-free object pools.

[Listing 2](#lst_object_pool_lst1) shows an example, where we create a pool containing five objects of type `int`. As the second step, we allocate five objects from the pool and store the obtained pointers in a temporary array. Finally, we deallocate them by calling `Free` on each pointer.

    \\\inputlistingsnippet{../examples/containers/object_pool.cc:object_pool_1}

<a name="lst_object_pool_lst1"></a>**Listing 2**: Object pool – initialization, allocation, and deallocation

For allocating and deallocating objects, the object pool’s implementation relies on a value pool which keeps track of the objects in use. If the value pool is implemented in a lock-free manner, the object pool is lock-free as well (analogously for wait-free pools). Currently, EMB² provides two value pools: `WaitFreeArrayValuePool` and `LockFreeTreeValuePool`. Normally (if nothing else is specified), the wait-free pool is used. For having a lock-free object pool instead, one has to specify the corresponding value pool as additional template parameter. If we replace the first line of the previous example with the following lines, the object pool is not wait-free anymore but lock-free (the values are of type `int` and initialized with `0`):

    \\\inputlistingsnippet{../examples/containers/object_pool.cc:object_pool_2}

This will result in a speed-up for most applications, but progress guarantees are weaker.

### <a name="sec_containers_stacks"></a>Stacks

As the name indicates, the class template `LockFreeStack` implements a lock-free stack which stores elements according to the LIFO (Last-In, First-Out) principle. The stack provides two methods, `TryPush` and `TryPop`, both returning a Boolean value indicating success of the operation: `TryPop` returns `false` if the stack is empty, and `TryPush` returns `false` if the stack is full. If successful, `TryPop` returns the element removed from the stack via reference. [Listing 3](#lst_stack_lst1) shows a simple example. First, we create a stack of integers with a capacity of 10 elements (due to necessary over-provisioning of memory in thread-safe memory management, the stack might be able to hold more than 10 elements, but is guaranteed to be able to hold at least 10 elements). Then, we try to pop an element from the empty stack, which has to fail. In the subsequent for-loop, we fill the stack with the values 0...4. Afterwards, we pop five values from the stack into variable `j`. According to the LIFO semantics, the values are popped in reverse order, i.e., we get the sequence 4...0, which is checked by the assertion.

    \\\inputlistingsnippet{../examples/containers/stack.cc:stack}

<a name="lst_stack_lst1"></a>**Listing 3**: Stack - initialization, push, and pop

### <a name="sec_containers_queues"></a>Queues

There are currently two FIFO (First-In, First-Out) queue implementations in EMB², `LockFreeMPMCQueue` and `WaitFreeSPSCQueue`. The former can deal with multiple producers and multiple consumers (MPMC), whereas the latter is restricted to a single producer and a single consumer (SPSC). The interfaces are the same for both queues. The Boolean return value of the methods `TryEnqueue` and `TryDequeue` indicates success (`false` if the queue is full or empty, respectively).

[Listing 4](#lst_queue_lst1) shows an example for the `LockFreeMPMCQueue`. First, we create a queue with element type `int` and a capacity of (at least) 10 elements. Then, we try to dequeue an element from the empty queue, which has to fail. In the subsequent for-loop, we fill the queue with the values 0...4. Afterwards, we dequeue five values from the queue into variable `j`. According to the FIFO semantics, the values are dequeued in the same order as they were enqueued, i.e., we get the sequence 0...4, which is checked by the assertion.

    \\\inputlistingsnippet{../examples/containers/queues.cc:queue}

<a name="lst_queue_lst1"></a>**Listing 4**: Queue – initialization, enqueue, and dequeue


## <a name="cha_mtapi"></a>MTAPI

Leveraging the power of multicore processors requires to split computations into fine-grained tasks that can be executed in parallel. Threads are usually too heavy-weight for that purpose, since context switches consume a significant amount of time. Moreover, programming with threads is complex and error-prone due to typical pitfalls such as race conditions and deadlocks. To solve these problems, efficient task scheduling techniques have been developed which dynamically distribute the available tasks among a fixed number of worker threads. To reduce overhead, there is usually exactly one worker thread for each processor core.

While task schedulers are nowadays widely employed, especially in desktop and server applications, they are typically limited to a single operating system running on a homogeneous multicore processor. System-wide task management in heterogeneous embedded systems must be realized explicitly with low-level communication mechanisms. MTAPI [[1]](#bib_mtapi) addresses those issues by providing an API which allows parallel embedded software to be designed in a straightforward way, covering homogeneous and heterogeneous multicore architectures, as well as accelerators such as GPUs or FPGAs. As a major advantage, it abstracts from the hardware details and lets software developers focus on the application. Moreover, MTAPI takes into account typical requirements of embedded systems such as real-time constraints and predictable memory consumption.

The remainder of this chapter is structured as follows: The next section explains the basic terms and concepts of MTAPI as given in the specification [[1]](#bib_mtapi). The section on the [MTAPI C Interface](#sec_mtapi_c_interface) describes the C API using a simple example taken from [[1]](#bib_mtapi). Finally, the section on the [MTAPI C++ Interface](#sec_mtapi_cpp_interface) outlines the use of MTAPI in C++ applications. Note that the C++ interface is provided by EMB² for convenience but it is not part of the standard. Readers who are familiar with MTAPI or just want to get impression on how to use MTAPI in heterogeneous systems may skip this chapter on go directly to [Heterogeneous Systems](#cha_heterogeneous_systems).

### <a name="sec_mtapi_foundations"></a>Foundations

#### Domains

An MTAPI system is composed of one or more MTAPI domains. An MTAPI domain is a unique system global entity. Each MTAPI domain comprises a set of MTAPI nodes. An MTAPI node may only belong to one MTAPI domain, while an MTAPI domain may contain one or more MTAPI nodes. This allows the programmer to use MTAPI domains as namespaces for all kinds of IDs (e.g., nodes, actions, queues, etc.).

#### Nodes

An MTAPI node is an independent unit of execution, such as a process, thread, thread pool, processor, hardware accelerator, or instance of an operating system. A given MTAPI implementation specifies what constitutes a node for that implementation.

The intent is to avoid a mixture of node definitions in the same implementation (or in different domains within an implementation). If a node is defined as a unit of execution with its private address space (like a process), then a core with a single unprotected address space OS is equivalent to a node, whereas a core with a virtual memory OS can host multiple nodes.

On a shared memory SMP processor, a node can be defined as a subset of cores. A quad-core processor, for example, could be divided into two nodes, one node representing three cores and one node representing the fourth core reserved exclusively for certain tasks. The definition of a node is flexible because this allows applications to be written in the most portable fashion supported by the underlying hardware, while at the same time supporting more general-purpose multicore and many-core devices.

The definition allows portability of software at the interface level (e.g., the functional interface between nodes). However, the software implementation of a particular node cannot (and often should not) necessarily be preserved across a multicore SoC product line (or across product lines from different silicon providers) because a given node’s functionality may be provided in different ways, depending on the chosen multicore SoC.

#### Tasks

A task represents the computation associated with the data to be processed and is executed concurrently to the code starting it. The main API functions are `mtapi_task_start()` and `mtapi_task_wait()`. The semantics are similar to the corresponding thread functions (e.g., `pthread_create` / `pthread_join` in POSIX Threads). The lifetime of a task is limited; it can be started only once.

#### Actions

In order to cope with heterogeneous systems and computations implemented in hardware, a task is not directly associated with an entry function as it is done in other task-parallel APIs. Instead, it is associated with at least one action object representing the calculation. The association is indirect: one or more actions implement a job, one job is associated with a task. If the action is implemented in software, this is either a function on the same node (which can represent the same processor or core) or a function implemented on a different node that does not share memory with the core starting the task.

Starting a task consists of three steps:

1. Create the action object with a job ID (software-implemented actions only).
2. Obtain a job reference.
3. Start the task using the job reference.

#### Synchronization

The basic synchronization mechanism provided by MTAPI is waiting for task completion. Calling `mtapi_task_wait()` with a task handle blocks the current thread or task until the task referenced by the handle has completed. Depending on the implementation, the calling thread can be used for executing other tasks while waiting for the task to be completed. In order to synchronize with a set of tasks, every task can be associated with a task group. The methods `mtapi_group_wait_all()` and `mtapi_group_wait_any()` wait for a group of tasks or completion of any task in the group, respectively.

#### Queues

Queues are used for guaranteeing sequential order of execution of tasks. A common use case is packet processing in the communication domain: for every connection all packets must be processed sequentially, while the packets of different connections can be processed in parallel to each other.

Sequential execution is accomplished by using a queue for every connection and queuing all packets of one connection into the same queue. In some systems, queues are implemented in hardware, otherwise MTAPI implements software queues. MTAPI is designed for handling thousands of queues that are processed in parallel.

The procedure for setting up and using a queue is as follows:

1. Create the action object (software-implemented actions only).
2. Obtain a job reference.
3. Create a queue object and attach the job to the queue
   (software-implemented queues only).
4. Obtain a queue handle if the queue was created on a different node,
   or if the queue is hardware-implemented.
5. Use the queue: enqueue the work using the queue.

Another important purpose of queues is that different queues can express different scheduling attributes for the same job. For example, in contrast to order-preserving queues, non-order-preserving queues can be used for load-balancing purposes between different computation nodes. In this case, the queue must be associated with more than one action implementing the same task on different nodes (i.e., different processors or cores implementing different instruction set architectures). If a queue is configured this way, the order will not be preserved.

#### Attributes

Attributes are provided as a means to extend the API. Different implementations may define and support additional attributes beyond those predefined by the API. To foster portability and implementation flexibility, attributes are maintained in an opaque data object that may not be examined directly by the user. Each object (e.g., task, action, queue) has an attributes data object associated with it, and many attributes have a small set of predefined values that must be supported by MTAPI implementations. The user may initialize, get, and set these attributes. For default behavior, it is not necessary to call the initialize, get, and set attribute functions. However, to get non-default behavior, the typical four-step process is:

1. Declare an attributes object of the `mtapi_<object>_attributes_t` data type.
2. `mtapi_<object>attr_init()`: Returns an attributes object with all
   attributes set to their default values.
3. `mtapi_<object>attr_set()` (Repeat for all attributes to be set): Assigns a
   value to the specified attribute of the specified attributes object.
4. `mtapi_<object>_create()`: Passes the attributes object modified in the
   previous step as a parameter when creating the object.

At any time, the user can call `mtapi_<object>_get_attribute()` to query the value of an attribute. After an object has been created, some objects allow to change attributes by calling `mtapi_<object>_set_attribute()`.

### <a name="sec_mtapi_c_interface"></a>C Interface

The calculation of Fibonacci numbers is a simple example for a recursive algorithm that can easily be parallelized. [Listing 5](#lst_mtapi_fibonacci_sequential) shows a sequential version:

```cpp
int fib(int n) {
  int x,y;
  if (n < 2) {
    return n;
  } else {
    x = fib(n - 1);
    y = fib(n - 2);
    return x + y;
  }
}

int fibonacci(int n) {
  return fib(n);
}

void main(void) {
  int n = 6;
  int result = fibonacci(n);
  printf("fib(%i) = %i\n", n, result);
}
```

<a name="lst_mtapi_fibonacci_sequential"></a>**Listing 5**: Sequential program for computing Fibonacci numbers

This algorithm can be parallelized by spawning a task for one of the recursive calls (`fib(n - 1)`, for example). When doing this with MTAPI, an action function that represents `fib(int n)` is needed. It has the following signature:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c.cc:mtapi_c_action_signature}

Within the action function, the arguments should be checked, since the user might supply a buffer that is too small:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c.cc:mtapi_c_validate_arguments}

Here, `mtapi_context_status_set()` is used to report errors. The error code will be returned by `mtapi_task_wait()`. Also, care has to be taken when using the result buffer. The user might not want to use the result and supply a `NULL` pointer or accidentally a buffer that is too small:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c.cc:mtapi_c_validate_result_buffer}

At this point, calculation of the result can commence. First, the terminating condition of the recursion is checked:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c.cc:mtapi_c_terminating_condition}

After that, the first part of the computation is launched as a task using `mtapi_task_start()` (the action function is registered with the job `FIBONACCI_JOB` in the `fibonacci()` function and the resulting handle is stored in the global variable `mtapi_job_hndl_t fibonacciJob`):

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c.cc:mtapi_c_calc_task}

The second part can be executed directly:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c.cc:mtapi_c_calc_direct}

Then, completion of the MTAPI task has to be waited for by calling `mtapi_task_wait()`:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c.cc:mtapi_c_wait_task}

Finally, the results can be added and written into the result buffer:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c.cc:mtapi_c_write_back}

The `fibonacci()` function gets a bit more complicated now. The MTAPI runtime has to be initialized first by (optionally) initializing node attributes and then calling `mtapi_initialize()`:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c.cc:mtapi_c_initialize}

Then, the action function needs to be associated to a job. By calling `mtapi_action_create()`, the action function is registered with the job `FIBONACCI_JOB`. The job handle of this job is stored in the global variable `mtapi_job_hndl_t fibonacciJob` so that it can be accessed by the action function later on:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c.cc:mtapi_c_register_action}

Now that the action is registered with a job, the root task can be started with `mtapi_task_start()`:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c.cc:mtapi_c_start_task}

After everything is done, the action is deleted (`mtapi_action_delete()`) and the runtime is shut down (`mtapi_finalize()`):

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c.cc:mtapi_c_finalize}

### <a name="sec_mtapi_cpp_interface"></a>C++ Interface

As mentioned previously, EMB² provides C++ wrappers for the MTAPI C interface. The full interface provides functions for all MTAPI related tasks and even supports heterogeneous systems. For ease of use, a simpler version for SMP systems is also provided.

#### Full Interface

The signature of an action function for the C++ interface is the same as for the C interface:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_cpp.cc:mtapi_cpp_action_signature}

Checking argument and result buffer sizes is the same as in the C example. Also, the terminating condition of the recursion still needs to be checked:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_cpp.cc:mtapi_cpp_terminating_condition}

After that, the first part of the computation is launched as an MTAPI task using `embb::mtapi::Node::Start()` (the action function is registered with the job `FIBONACCI_JOB` in the `fibonacci()` function and the resulting handle is stored in the global variable `embb::mtapi::Job fibonacciJob`):

    \\\inputlistingsnippet{../examples/mtapi/mtapi_cpp.cc:mtapi_cpp_calc_task}

The second part can be executed directly:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_cpp.cc:mtapi_cpp_calc_direct}

Then, completion of the MTAPI task has to be waited for using `embb::mtapi::Task::Wait()`:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_cpp.cc:mtapi_cpp_wait_task}

Finally, the two parts can be added and written into the result buffer:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_cpp.cc:mtapi_cpp_write_back}

Note that there is no need to do error checking everywhere, since errors are reported as exceptions. In this example there is only a single try/catch block in the main function:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_cpp.cc:mtapi_cpp_main}

The `fibonacci()` function is about the same as in the C version. The MTAPI runtime needs to be initialized first:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_cpp.cc:mtapi_cpp_initialize}

Then, the node instance can be fetched:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_cpp.cc:mtapi_cpp_get_node}

After that, the action function needs to be associated to a job. By instantiating an `embb::mtap::Action` object, the action function is registered with the job `FIBONACCI_JOB`. The job is stored in the global variable `embb::mtapi::Job fibonacciJob` so that it can be accessed by the action function later on:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_cpp.cc:mtapi_cpp_register_action}

Now that the action is registered and the job is initialized, the root task can be started:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_cpp.cc:mtapi_cpp_start_task}

Again, the started task has to be waited for (using `embb::mtapi::Task::Wait()`) before the result can be returned.

The registered action will be unregistered when it goes out of scope. The runtime needs to be shut down by calling:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_cpp.cc:mtapi_cpp_finalize}

#### Simplified Interface for SMP actions

The signature of an action function for the simplified API (SMP systems) looks like this:

```cpp
void simpleActionFunction(
  TaskContext & task_context
) {
  // something useful
}
```

The action function does not need to be registered with a job. Instead, a preregistered job is used that expects an `embb::base::Function<void, embb::mtapi::TaskContext &>` object. Therefore, a task can be scheduled directly using only the function above:

```cpp
embb::mtapi::Task task = node.Start(simpleActionFunction);
```

### <a name="sec_mtapi_plugins"></a>Plugins

The implementation of MTAPI provides an extension to allow for custom actions that are not executed by the scheduler for software actions as described in the previous sections. Three plugins are delivered with EMB², one for supporting distributed systems through TCP/IP networking and the other two for OpenCL or CUDA-capable GPUs.

#### Plugin API

The plugin API essentially consists of a single function contained in the `mtapi_ext.h` header file:

```cpp
mtapi_ext_plugin_action_create()
```

This function is used to associate the plugin action with a specific job ID:

```cpp
mtapi_action_hndl_t mtapi_ext_plugin_action_create(
  MTAPI_IN mtapi_job_id_t job_id,
  MTAPI_IN mtapi_ext_plugin_task_start_function_t task_start_function,
  MTAPI_IN mtapi_ext_plugin_task_cancel_function_t task_cancel_function,
  MTAPI_IN mtapi_ext_plugin_action_finalize_function_t action_finalize_function,
  MTAPI_IN void* plugin_data,
  MTAPI_IN void* node_local_data,
  MTAPI_IN mtapi_size_t node_local_data_size,
  MTAPI_IN mtapi_action_attributes_t* attributes,
  MTAPI_OUT mtapi_status_t* status
);
```

The plugin action is implemented through three callbacks: task start, task cancel, and action finalize.

`task_start_function` is called when the user requests execution of the plugin action by calling `mtapi_task_start()` or `mtapi_task_enqueue()`. To those functions the fact that they operate on a plugin action is transparent, they only require the handle of the job the action was registered with.

`task_cancel_function` is called when the user requests cancelation of a task by calling `mtapi_task_cancel()` or by calling `mtapi_queue_disable()` on a non-retaining queue.

`action_finalize_function` is called when the node is finalized and the action is deleted, or when the user explicitly deletes the action by calling `mtapi_action_delete()`.

For illustration, our example plugin will provide a no-op action. The task start callback in that case looks like this:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c_plugin.cc:mtapi_c_plugin_task_start_cb}

The scheduling operation is responsible for bringing the task to execution. This might involve instructing some hardware to execute the task or pushing the task into a queue for execution by a separate worker thread. Here, however, the task is executed directly:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c_plugin.cc:mtapi_c_plugin_task_schedule}

Since the task gets executed right away, it cannot be canceled and the task cancel callback implementation is empty:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c_plugin.cc:mtapi_c_plugin_task_cancel_cb}

The plugin action did not acquire any resources, so the action finalize callback is empty as well:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c_plugin.cc:mtapi_c_plugin_action_finalize_cb}

Now that the callbacks are in place, the action can be registered with a job after the node was initialized using `mtapi_initialize()`:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c_plugin.cc:mtapi_c_plugin_action_create}

The job handle can now be obtained the normal MTAPI way. The fact that there is a plugin working behind the scenes is transparent:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c_plugin.cc:mtapi_c_plugin_get_job}

Using the job handle, tasks can be started like normal MTAPI tasks:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c_plugin.cc:mtapi_c_plugin_task_do_start}

This call will lead to the invocation of the `plugin_task_start` callback function, where the plugin implementer is responsible for bringing the task to execution.

#### Network

The MTAPI network plugin provides a means to distribute tasks over a TCP/IP network. As an example, the following vector addition action is used:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c_network.cc:mtapi_network_c_action_function}

It adds two float vectors and a float from node local data, and writes the result into the result float vector. In the example, code the vectors will hold `kElements` floats each.

To use the network plugin, its header file needs to be included first:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c_network.cc:mtapi_network_c_header}

After initializing the node using `mtapi_initialize()`, the plugin itself needs to be initialized:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c_network.cc:mtapi_network_c_plugin_initialize}

This will set up a listening socket on the localhost interface (127.0.0.1) at port 12345. The socket will allow a maximum of five connections and has a maximum transfer buffer size of `kElements * 4 * 3 + 32`. This buffer size needs to be large enough to fit at least the argument and result buffer sizes at once. The example uses three vectors of `kElements` floats using `kElements * sizeof(float) * 3` bytes.

Since the example connects to itself on localhost, the “remote” action needs to be registered with the `NETWORK_REMOTE_JOB`:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c_network.cc:mtapi_network_c_remote_action_create}

After that, the local network action is created that maps `NETWORK_LOCAL_JOB` to `NETWORK_REMOTE_JOB` through the network:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c_network.cc:mtapi_network_c_local_action_create}

Now, `NETWORK_LOCAL_JOB` can be used to execute tasks by simply calling `mtapi_task_start()`. Their parameters will be transmitted through a socket connection and are consumed by the network plugin worker thread. The thread will start a task using the `NETWORK_REMOTE_JOB`. When this task is finished, the results will be collected and sent back through the network. Again, the network plugin thread will receive the results, provide them to the `NETWORK_LOCAL_JOB` task and mark that task as finished.

When all work is done, the plugin needs to be finalized. This will stop the plugin worker thread and close the sockets:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c_network.cc:mtapi_network_c_plugin_finalize}

After that, the node may be finalized by calling `mtapi_finalize()`.

#### OpenCL

The MTAPI OpenCL plugin allows the user to leverage the computational power of an OpenCL accelerator, if one is available in the system.

Let us reuse the vector addition example from the network plugin. However, the action function is an OpenCL kernel now:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c_opencl.cc:mtapi_opencl_c_kernel}

The OpenCL plugin header file needs to be included first:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c_opencl.cc:mtapi_opencl_c_header}

As with the network plugin, the OpenCL plugin needs to be initialized after the node has been initialized:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c_opencl.cc:mtapi_opencl_c_plugin_initialize}

Then, the plugin action can be registered with the `OPENCL_JOB`:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c_opencl.cc:mtapi_opencl_c_action_create}

The kernel source and the name of the kernel to use (`AddVector`) need to be specified while creating the action. The kernel will be compiled using the OpenCL runtime and the provided node local data will be transferred to the accelerator memory. The local work size is the number of threads that will share OpenCL local memory, in this case 32. The element size tells the OpenCL plugin how many bytes a single element in the result buffer consumes, in this case 4, as a single result is a single float. The OpenCL plugin will launch `result_buffer_size/element_size` OpenCL threads to calculate the result.

Now, the `OPENCL_JOB` can be used like a normal MTAPI job to start tasks.

After all work is done, the plugin needs to be finalized. This will free all memory on the accelerator and delete the corresponding OpenCL context:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c_opencl.cc:mtapi_opencl_c_plugin_finalize}

#### CUDA

Similar to the OpenCL plugin, the CUDA plugin can be used to start tasks on an Nvidia GPU.

The vector addition example looks slightly different in CUDA:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c_cuda_kernel.cu:mtapi_cuda_c_kernel}

The kernel needs to be precompiled and will be transformed into a header file containing the resulting binary in a `char const *` array named `imageBytes`.

As with the OpenCL plugin, the CUDA plugin header file needs to be included first:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c_cuda.cc:mtapi_cuda_c_header}

Then, the CUDA plugin needs to be initialized after the node has been initialized:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c_cuda.cc:mtapi_cuda_c_plugin_initialize}

Now, the plugin action can be registered with the `CUDA_JOB`:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c_cuda.cc:mtapi_cuda_c_action_create}

The precompiled kernel binary and the name of the kernel to use need to be specified while creating the action. The kernel and node local data provided are transferred to the accelerator memory. The local work size is the number of threads that will share CUDA local memory, in this case 32. The element size tells the CUDA plugin how many bytes a single element in the result buffer consumes, in this case 4, as a single result is a single float. The CUDA plugin will launch `result_buffer_size/element_size` CUDA threads to calculate the result.

Now, the `CUDA_JOB` can be used like a normal MTAPI job to start tasks.

After all work is done, the plugin needs to be finalized. This will free all memory on the accelerator and delete the corresponding CUDA context:

    \\\inputlistingsnippet{../examples/mtapi/mtapi_c_cuda.cc:mtapi_cuda_c_plugin_finalize}


## <a name="cha_heterogeneous_systems"></a>Heterogeneous Systems

### <a name="sec_algorithms_heterogeneous_systems"></a>Algorithms

All of the algorithms provided by EMB² can also be used on heterogeneous systems. This allows to transparently offload work on different kinds of compute units, thus leveraging the available hardware resources in an optimal way. In the following, we focus on the key concepts&mdash;for more detailed information, please see the source code contained in the `examples` directory.

In addition to functions, functors or lambdas, the algorithms accept MTAPI job handles that implement the intended functionality. The action functions will be given structures containing the arguments and results according to the signatures used above. For the sake of simplicity, CPU actions are used to simulate a heterogeneous system. The CPU actions are functions with the following signature:

```cpp
void Action(
  const void* args,
  mtapi_size_t args_size,
  void* result_buffer,
  mtapi_size_t result_buffer_size,
  const void* node_local_data,
  mtapi_size_t node_local_data_size,
  mtapi_task_context_t* task_context
);
```

A node handle is retrieved and used by the following examples like this:

    \\\inputlistingsnippet{../examples/algorithms/heterogeneous.cc:get_node}

#### Invoke

First, we consider `Invoke` to start two jobs in parallel. For that purpose, we define two action functions `InvokeA` and `InvokeB` that have no parameters and just increment a global value (`a` and `b`, respectively). For `InvokeA`, we have:

    \\\inputlistingsnippet{../examples/algorithms/heterogeneous.cc:invoke_action}

The actions are associated with the job IDs `JOB_A` and `JOB_B`. The job handles are retrieved as follows:

    \\\inputlistingsnippet{../examples/algorithms/heterogeneous.cc:invoke_jobs}

After that, the jobs can be started:

    \\\inputlistingsnippet{../examples/algorithms/heterogeneous.cc:invoke_call}

The global variables `a` and `b` are now both set to `1`.

#### Sorting

To use `QuickSort`, we need a comparison function. `DescendingCompare` has two arguments of type `int` and one result of type `bool`. Since the function signature is fixed, we pack the arguments into a struct:

    \\\inputlistingsnippet{../examples/algorithms/heterogeneous.cc:struct_input_lhs_rhs}

The same holds for the result:

    \\\inputlistingsnippet{../examples/algorithms/heterogeneous.cc:struct_output_bool}

`args` needs to be casted to `InT` and `result_buffer` to `OutT`:

    \\\inputlistingsnippet{../examples/algorithms/heterogeneous.cc:cast_parameters}

Now, the arguments can be accessed, compared and the result can be written:

    \\\inputlistingsnippet{../examples/algorithms/heterogeneous.cc:sort_action_body}

`DescendingCompare` is associated with the job ID `JOB_COMPARE` and the job handle can be retrieved as follows:

    \\\inputlistingsnippet{../examples/algorithms/heterogeneous.cc:sort_job}

Then, we prepare a vector with `int`'s to be sorted:

    \\\inputlistingsnippet{../examples/algorithms/heterogeneous.cc:sort_data}

Finally, we call `QuickSort`

    \\\inputlistingsnippet{../examples/algorithms/heterogeneous.cc:sort_call}

and the `int`'s in the vector are now in descending order.

#### Counting

The predicate supplied to `CountIf` can be implemented by an action function `CheckZero` that takes one argument of type `int` and returns one result of type `bool`. The argument is again packed into a struct:

    \\\inputlistingsnippet{../examples/algorithms/heterogeneous.cc:struct_input_val}

The result struct is the same as in the sorting example. Also, `args` and `result_buffer` need to be casted to `InT` and `OutT`. Then, the body of `CheckZero` is simply:

    \\\inputlistingsnippet{../examples/algorithms/heterogeneous.cc:count_action_body}

After retrieving the job handle

    \\\inputlistingsnippet{../examples/algorithms/heterogeneous.cc:count_job}

we prepare a vector with `int`'s to count (if they are zero):

    \\\inputlistingsnippet{../examples/algorithms/heterogeneous.cc:count_data}

Finally, we call `CountIf`

    \\\inputlistingsnippet{../examples/algorithms/heterogeneous.cc:count_call}

which returns the number of zeros in the vector.

#### Foreach Loops

`ForEach` accepts functions taking a reference to an iterator as argument in order to work on the referenced object. Consider, for example, an action function `Double` that doubles a given value. It has one argument of type `int` and one result of type `int`. The argument struct is thus the same as in the counting example. The result resides in a struct containing one `int`:

    \\\inputlistingsnippet{../examples/algorithms/heterogeneous.cc:struct_output_int}

`args` and `result_buffer` need to be casted to `InT` and `OutT` once more. The body of `Double` is defined as follows:

    \\\inputlistingsnippet{../examples/algorithms/heterogeneous.cc:double_action_body}

After retrieving the job handle

    \\\inputlistingsnippet{../examples/algorithms/heterogeneous.cc:foreach_job}

we can call `ForEach` with this handle:

    \\\inputlistingsnippet{../examples/algorithms/heterogeneous.cc:foreach_call}

#### Reductions and Prefix Computations

`Reduce` and `Scan` use a reduction and a transformation function. The reduction function has two arguments and one result that all have the same type. The transformation function has one argument and one result with potentially different types. In our example, they are all of type `int`. For simplicity, we reuse the `Double` action and `JOB_DOUBLE` from the previous example as our transformation function. For the reduction function, we introduce an action `Add` with the following simple body:

    \\\inputlistingsnippet{../examples/algorithms/heterogeneous.cc:add_action_body}

The job handles are retrieved as follows:

    \\\inputlistingsnippet{../examples/algorithms/heterogeneous.cc:reduce_jobs}

Next, we create a vector of `int`'s:

    \\\inputlistingsnippet{../examples/algorithms/heterogeneous.cc:reduce_data}

The elements of the vector can then be reduced by:

    \\\inputlistingsnippet{../examples/algorithms/heterogeneous.cc:reduce_call}

The prefix sum is computed similarly:

    \\\inputlistingsnippet{../examples/algorithms/heterogeneous.cc:scan_call}


### <a name="sec_dataflow_heterogeneous_systems"></a>Dataflow

Dataflow networks can be used on heterogeneous systems as well. In addition to functions and functors, dataflow sources, sinks, and processes accept MTAPI job handles implementing the intended functionality. The action functions will be given structures containing the arguments and results according to the signatures used previously. To simulate a heterogeneous system, CPU actions with the following signature are used:

```cpp
void Action(
  const void* args,
  mtapi_size_t args_size,
  void* result_buffer,
  mtapi_size_t result_buffer_size,
  const void* node_local_data,
  mtapi_size_t node_local_data_size,
  mtapi_task_context_t* task_context
);
```

Suppose, for example, we want to double the integers from 0 to 9 and sum them up. For that purpose, we define three action functions, one for a source called `Generate`, one for a process called `Double`, and one for a sink called `Accumulate`. Each of them is associated with a different job.

#### Generate

The source function generates integers from 0 to 9. It receives no arguments and returns a `bool` that indicates whether generating integers shall continue, and an `int` that represents the generated value. The results are packed into a struct:

    \\\inputlistingsnippet{../examples/dataflow/dataflow_heterogeneous.cc:source_output_struct_int}

The `result_buffer` pointer needs to be casted to `OutT`:

    \\\inputlistingsnippet{../examples/dataflow/dataflow_heterogeneous.cc:cast_output}

Then, the body of the function is:

    \\\inputlistingsnippet{../examples/dataflow/dataflow_heterogeneous.cc:source_action_body}

#### Double

The process function expects and `int` and returns the double of its value. Both, the argument and the result are packed into a struct:

    \\\inputlistingsnippet{../examples/dataflow/dataflow_heterogeneous.cc:input_struct_int}

    \\\inputlistingsnippet{../examples/dataflow/dataflow_heterogeneous.cc:output_struct_int}

As usual, `args` as well as `result_buffer` need to be casted:

    \\\inputlistingsnippet{../examples/dataflow/dataflow_heterogeneous.cc:cast_input}
    \\\inputlistingsnippet{../examples/dataflow/dataflow_heterogeneous.cc:cast_output}

Finally, the actual calculation is simply:

    \\\inputlistingsnippet{../examples/dataflow/dataflow_heterogeneous.cc:double_action_body}

#### Accumulate

The sink is supposed to add up all incoming values. It returns no result and expects a value of type `int` which is packed into a struct:

    \\\inputlistingsnippet{../examples/dataflow/dataflow_heterogeneous.cc:input_struct_int}

After casting `args` to `InT`, the inputs can be accumulated:

    \\\inputlistingsnippet{../examples/dataflow/dataflow_heterogeneous.cc:cast_input}
    \\\inputlistingsnippet{../examples/dataflow/dataflow_heterogeneous.cc:sink_action_body}

#### Network

In the main function, we first retrieve the node handle:

    \\\inputlistingsnippet{../examples/dataflow/dataflow_heterogeneous.cc:get_node}

Then, we obtain the job handles:

    \\\inputlistingsnippet{../examples/dataflow/dataflow_heterogeneous.cc:get_jobs}

After that, we define the network and its processes:

    \\\inputlistingsnippet{../examples/dataflow/dataflow_heterogeneous.cc:define_net}

Finally, we connect the processes and run the network:

    \\\inputlistingsnippet{../examples/dataflow/dataflow_heterogeneous.cc:net_connect}
    \\\inputlistingsnippet{../examples/dataflow/dataflow_heterogeneous.cc:net_run}


## <a name="cha_tutorial_application"></a>Tutorial Application

To apply the concepts detailed above in practice, we create a video processing application in the following paragraphs. The application is supposed to accept a video file on the command line, apply some filter and write an output video to a given file. For handling video files, we are going to use ffmpeg. For details on how to build and run the application refer to the README.md file in doc/tutorial/application.

The application consists of 5 parts, the main application, the filters, the input video handler, the frame format converter and the output video builder.

Three parts relate to ffmpeg video de- and encoding. The input video handler opens a given video file and is used to read consecutive frames from the stream until there are no more frames. The frame format converter is used to convert from the source color format to RGB and vice versa, since all filter processing is done in RGB color space. The output video builder encodes the resulting image stream back into a video file. We will not cover these three in detail.

We provide several example filters to use during processing. The filters come in three flavors: sequential, parallel using the algorithms library and as OpenCL kernels.

Finally, the main application binds the other components into a working whole.

### Filters

The filters are just loops iterating over the pixels and applying their operation to them. Here is a simple color to greyscale filter:

    \\\inputlistingsnippet{application/src/filters.cc:serial_filter}

Parallelizing it is straightforward using the algorithms building block. We just replace the for loops by a single `ForLoop` operation and retrieve the `x` and `y` pixel positions from the single counter:

    \\\inputlistingsnippet{application/src/filters.cc:parallel_filter}

Now the filter runs in parallel, but the control flow is still running serially and limits scalability.

### Control Flow

Applying video decoding, format conversion, filtering, another format conversion and video encoding for a single frame yields the following sequence of calls:

    \\\inputlistingsnippet{application/main.cc:serial_execution}

This sequence can be seen as a pipeline and therefore we can parallelize it using the dataflow building block. Each of the operations is wrapped into a source, process or sink and the resulting objects are connected and deployed into a network:

    \\\inputlistingsnippet{application/main.cc:pipelined_execution}

Scalability is now significantly improved, as no part of the application runs serially anymore.

#### Heterogeneous Systems

Some systems feature additional accelerators, e.g. a GPU, to further improve processing speed. Using OpenCL we can leverage the power of such an accelerator to speed up the processing of the filters. The filter itself is implemented in OpenCL:

    \\\inputlistingsnippet{application/src/filters.cc:opencl_filter}

The kernel needs to be wrapped into a MTAPI action:

    \\\inputlistingsnippet{application/main.cc:make_opencl_action}

Now the filter can be used like any job. Note however the additional cost imposed by the need to copy the frame and the parameters of the filter into a single buffer:

    \\\inputlistingsnippet{application/main.cc:call_opencl_action}

When wrapped into a dataflow process it can even be used as a part of the pipeline outlined before:

    \\\inputlistingsnippet{application/main.cc:opencl_pipeline}


## <a name="cha_bibliography"></a>Bibliography

<a name="bib_mtapi"></a>[1] Multicore Task Management API (MTAPI) Specification V1.0, The Multicore Association, March 2013.
