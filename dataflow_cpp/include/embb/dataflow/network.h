/*
 * Copyright (c) 2014-2017, Siemens AG. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef EMBB_DATAFLOW_NETWORK_H_
#define EMBB_DATAFLOW_NETWORK_H_

#if EMBB_DATAFLOW_TRACE_SIGNAL_HISTORY
#include <vector>
#endif

#include <embb/base/atomic.h>
#include <embb/base/thread.h>

#include <embb/dataflow/internal/select.h>
#include <embb/dataflow/internal/switch.h>
#include <embb/dataflow/internal/constant_source.h>
#include <embb/dataflow/internal/source.h>
#include <embb/dataflow/internal/process.h>
#include <embb/dataflow/internal/sink.h>

#include <embb/dataflow/internal/scheduler_sequential.h>
#include <embb/dataflow/internal/scheduler_mtapi.h>

namespace embb {
namespace dataflow {

#ifdef DOXYGEN

/**
 * Represents a set of processes that are connected by communication channels.
 *
 * \ingroup CPP_DATAFLOW
 */
class Network {
 public:
  /**
   * Constructs an empty network.
   * \note The number of concurrent tokens will automatically be derived from
   * the structure of the network on the first call to operator(), and the
   * corresponding resources will be allocated then.<br/>
   * When using parallel algorithms inside a dataflow network, the task
   * limit may be exceeded. In that case, increase the task limit of the MTAPI
   * node.
   */
  Network() {}

  /**
   * Constructs an empty network.
   * \param slices Number of concurrent tokens allowed in the network.
   * \note The number of slices might be reduced internally if the task
   * limit of the underlying MTAPI node would be exceeded.<br/>
   * When using parallel algorithms inside a dataflow network, the task
   * limit may be exceeded. In that case, increase the task limit of the MTAPI
   * node.
   */
  explicit Network(int slices) {}

  /**
   * Constructs an empty network.
   * \param policy Default execution policy of the processes in the network.
   * \note The number of concurrent tokens will automatically be derived from
   * the structure of the network on the first call to operator(), and the
   * corresponding resources will be allocated then.<br/>
   * When using parallel algorithms inside a dataflow network, the task
   * limit may be exceeded. In that case, increase the task limit of the MTAPI
   * node.
   */
  explicit Network(embb::mtapi::ExecutionPolicy const & policy) {}

  /**
   * Constructs an empty network.
   * \param slices Number of concurrent tokens allowed in the network.
   * \param policy Default execution policy of the processes in the network.
   * \note The number of slices might be reduced internally if the task
   * limit of the underlying MTAPI node would be exceeded.<br/>
   * When using parallel algorithms inside a dataflow network, the task
   * limit may be exceeded. In that case, increase the task limit of the MTAPI
   * node.
   */
  Network(int slices, embb::mtapi::ExecutionPolicy const & policy) {}

  /**
   * Input port class.
   */
  template <typename Type>
  class In {
  };

  /**
   * Output port class.
   */
  template <typename Type>
  class Out {
   public:
    /**
     * Input port class that can be connected to this output port.
     */
    typedef In<Type> InType;

    /**
     * Connects this output port to the input port \c input.
     * If the input port already was connected to a different
     * output an ErrorException is thrown.
     * \param input The input port to connect to.
     */
    void Connect(InType & input);

    /**
     * Connects this output port to the input port \c input.
     * If the input port already was connected to a different
     * output an ErrorException is thrown.
     * \param input The input port to connect to.
     */
    void operator >> (InType & input);
  };

  /**
   * Provides the input port types for a process.
   * \tparam T1 Type of first port.
   * \tparam T2 Optional type of second port.
   * \tparam T3 Optional type of third port.
   * \tparam T4 Optional type of fourth port.
   * \tparam T5 Optional type of fifth port.
   */
  template <typename T1, typename T2 = embb::base::internal::Nil,
    typename T3 = embb::base::internal::Nil,
    typename T4 = embb::base::internal::Nil,
    typename T5 = embb::base::internal::Nil>
  struct Inputs {
    /**
     * Type list used to derive input port types from Index.
     * \tparam Index The index of the input port type to query.
     */
    template <int Index>
    struct Types {
      /**
       * Result of an input port type query.
       * T_Index is T1 if Index is 0, T2 if Index is 1 and so on.
       */
      typedef In<T_Index> Result;
    };

    /**
     * \returns Reference to input port at Index.
     */
    template <int Index>
    typename Types<Index>::Result & Get();
  };

  /**
   * Provides the output port types for a process.
   * \tparam T1 Type of first port.
   * \tparam T2 Optional type of second port.
   * \tparam T3 Optional type of third port.
   * \tparam T4 Optional type of fourth port.
   * \tparam T5 Optional type of fifth port.
   */
  template <typename T1, typename T2 = embb::base::internal::Nil,
    typename T3 = embb::base::internal::Nil,
    typename T4 = embb::base::internal::Nil,
    typename T5 = embb::base::internal::Nil>
  struct Outputs {
    /**
     * Type list used to derive output port types from Index.
     * \tparam Index The index of the output port type to query.
     */
    template <int Index>
    struct Types {
      /**
       * Result of an output port type query.
       * T_Index is T1 if Index is 0, T2 if Index is 1 and so on.
       */
      typedef Out<T_Index> Result;
    };

    /**
     * \returns Reference to output port at Index.
     */
    template <int Index>
    typename Types<Index>::Result & Get();
  };

  /**
   * Generic serial process template.
   *
   * Implements a generic serial process in the network that may have one to
   * four input ports and one to four output ports but no more that five total
   * ports.
   * Tokens are processed in order.
   *
   * \see Source, ParallelProcess, Sink, Switch, Select
   *
   * \tparam Inputs Inputs of the process.
   * \tparam Outputs Outputs of the process.
   */
  template <class Inputs, class Outputs>
  class SerialProcess {
   public:
    /**
     * Function type to use when processing tokens.
     */
    typedef embb::base::Function<void, INPUT_TYPE_LIST, OUTPUT_TYPE_LIST>
      FunctionType;

    /**
     * Input port type list.
     */
    typedef Inputs<INPUT_TYPE_LIST> InputsType;

    /**
     * Output port type list.
     */
    typedef Outputs<OUTPUT_TYPE_LIST> OutputsType;

    /**
     * Constructs a SerialProcess with a user specified processing function.
     * \param network The network this node is going to be part of.
     * \param function The Function to call to process a token.
     */
    SerialProcess(Network & network, FunctionType function);

    /**
     * Constructs a SerialProcess with a user specified embb::mtapi::Job.
     * The Job must be associated with an action function accepting a struct
     * containing copies of the inputs as its argument buffer and a struct
     * containing the outputs as its result buffer.
     * \param network The network this node is going to be part of.
     * \param job The embb::mtapi::Job to process a token.
     */
    SerialProcess(Network & network, embb::mtapi::Job job);

    /**
     * Constructs a SerialProcess with a user specified processing function.
     * \param network The network this node is going to be part of.
     * \param function The Function to call to process a token.
     * \param policy The execution policy of the process.
     */
    SerialProcess(Network & network, FunctionType function,
      embb::mtapi::ExecutionPolicy const & policy);

    /**
     * Constructs a SerialProcess with a user specified embb::mtapi::Job.
     * The Job must be associated with an action function accepting a struct
     * containing copies of the inputs as its argument buffer and a struct
     * containing the outputs as its result buffer.
     * \param network The network this node is going to be part of.
     * \param job The embb::mtapi::Job to process a token.
     * \param policy The execution policy of the process.
     */
    SerialProcess(Network & network, embb::mtapi::Job job,
      embb::mtapi::ExecutionPolicy const & policy);

    /**
     * \returns \c true if the SerialProcess has any inputs, \c false
     *          otherwise.
     */
    virtual bool HasInputs() const;

    /**
     * \returns Reference to a list of all input ports.
     */
    InputsType & GetInputs();

    /**
     * \returns Input port at Index.
     */
    template <int Index>
    typename InputsType::Types<Index>::Result & GetInput();

    /**
     * \returns \c true if the SerialProcess has any outputs, \c false
     *          otherwise.
     */
    virtual bool HasOutputs() const;

    /**
     * \returns Reference to a list of all output ports.
     */
    OutputsType & GetOutputs();

    /**
     * \returns Output port at Index.
     */
    template <int Index>
    typename OutputsType::Types<Index>::Result & GetOutput();

    /**
     * Connects output port 0 to input port 0 of \c target.
     * \param target Process to connect to.
     * \tparam T Type of target process.
     */
    template <typename T>
    void operator >> (T & target);
  };

  /**
   * Generic parallel process template.
   *
   * Implements a generic parallel process in the network that may have one to
   * four input ports and one to four output ports but no more that five total
   * ports.
   * Tokens are processed as soon as all inputs for that token are complete.
   *
   * \see Source, SerialProcess, Sink, Switch, Select
   *
   * \tparam Inputs Inputs of the process.
   * \tparam Outputs Outputs of the process.
   */
  template <class Inputs, class Outputs>
  class ParallelProcess {
   public:
    /**
     * Function type to use when processing tokens.
     */
    typedef embb::base::Function<void, INPUT_TYPE_LIST, OUTPUT_TYPE_LIST>
      FunctionType;

    /**
     * Input port type list.
     */
    typedef Inputs<INPUT_TYPE_LIST> InputsType;

    /**
     * Output port type list.
     */
    typedef Outputs<OUTPUT_TYPE_LIST> OutputsType;

    /**
     * Constructs a ParallelProcess with a user specified processing function.
     * \param network The network this node is going to be part of.
     * \param function The Function to call to process a token.
     */
    ParallelProcess(Network & network, FunctionType function);

    /**
     * Constructs a ParallelProcess with a user specified embb::mtapi::Job.
     * The Job must be associated with an action function accepting a struct
     * containing copies of the inputs as its argument buffer and a struct
     * containing the outputs as its result buffer.
     * \param network The network this node is going to be part of.
     * \param job The embb::mtapi::Job to process a token.
     */
    ParallelProcess(Network & network, embb::mtapi::Job job);

    /**
     * Constructs a ParallelProcess with a user specified processing function.
     * \param network The network this node is going to be part of.
     * \param function The Function to call to process a token.
     * \param policy The execution policy of the process.
     */
    ParallelProcess(Network & network, FunctionType function,
      embb::mtapi::ExecutionPolicy const & policy);

    /**
     * Constructs a ParallelProcess with a user specified embb::mtapi::Job.
     * The Job must be associated with an action function accepting a struct
     * containing copies of the inputs as its argument buffer and a struct
     * containing the outputs as its result buffer.
     * \param network The network this node is going to be part of.
     * \param job The embb::mtapi::Job to process a token.
     * \param policy The execution policy of the process.
     */
    ParallelProcess(Network & network, embb::mtapi::Job job,
      embb::mtapi::ExecutionPolicy const & policy);

    /**
     * \returns \c true if the ParallelProcess has any inputs, \c false
     *          otherwise.
     */
    virtual bool HasInputs() const;

    /**
     * \returns Reference to a list of all input ports.
     */
    InputsType & GetInputs();

    /**
     * \returns Input port at Index.
     */
    template <int Index>
    typename InputsType::Types<Index>::Result & GetInput();

    /**
     * \returns \c true if the ParallelProcess has any outputs, \c false
     *          otherwise.
     */
    virtual bool HasOutputs() const;

    /**
     * \returns Reference to a list of all output ports.
     */
    OutputsType & GetOutputs();

    /**
     * \returns Output port at Index.
     */
    template <int Index>
    typename OutputsType::Types<Index>::Result & GetOutput();

    /**
     * Connects output port 0 to input port 0 of \c target.
     * \param target Process to connect to.
     * \tparam T Type of target process.
     */
    template <typename T>
    void operator >> (T & target);
  };

  /**
   * Switch process template.
   *
   * A switch has 2 inputs and 2 outputs. Input port 0 is of type boolean and
   * selects to which output port the value of input port 1 of type \c Type
   * is sent. If input port 0 is set to true the value goes to output port 0
   * and to output port 1 otherwise.
   * Tokens are processed as soon as all inputs for that token are complete.
   *
   * \see Select
   *
   * \tparam Type The type of input port 1 and output port 0 and 1.
   */
  template<typename Type>
  class Switch {
   public:
    /**
     * Function type to use when processing tokens.
     */
    typedef embb::base::Function<void, bool, Type, Type &> FunctionType;

    /**
     * Input port type list.
     */
    typedef Inputs<bool, Type> InputsType;

    /**
     * Output port type list.
     */
    typedef Outputs<Type> OutputsType;

    /**
     * Constructs a Switch process.
     * \param network The network this node is going to be part of.
     */
    explicit Select(Network & network);

    /**
     * Constructs a Switch process.
     * \param network The network this node is going to be part of.
     * \param policy The execution policy of the process.
     */
    Select(Network & network, embb::mtapi::ExecutionPolicy const & policy);

    /**
     * \returns Always \c true.
     */
    virtual bool HasInputs() const;

    /**
     * \returns Reference to a list of all input ports.
     */
    InputsType & GetInputs();

    /**
     * \returns Input port at Index.
     */
    template <int Index>
    typename InputsType::Types<Index>::Result & GetInput();

    /**
     * \returns Always \c true.
     */
    virtual bool HasOutputs() const;

    /**
     * \returns Reference to a list of all output ports.
     */
    OutputsType & GetOutputs();

    /**
     * \returns Output port at Index.
     */
    template <int Index>
    typename OutputsType::Types<Index>::Result & GetOutput();

    /**
     * Connects output port 0 to input port 0 of \c target.
     * \param target Process to connect to.
     * \tparam T Type of target process.
     */
    template <typename T>
    void operator >> (T & target);
  };

  /**
   * Select process template.
   *
   * A select has 3 inputs and 1 output. Input port 0 is of type boolean and
   * selects which of input port 1 or 2 (of type \c Type) is sent to output
   * port 0 (of type \c Type). If input port 0 is set to true the value of
   * input port 1 is selected, otherwise the value of input port 2 is taken.
   * Tokens are processed as soon as all inputs for that token are complete.
   *
   * \see Switch
   *
   * \tparam Type The type of input port 1 and 2 and output port 0.
   */
  template<typename Type>
  class Select {
   public:
    /**
     * Function type to use when processing tokens.
     */
    typedef embb::base::Function<void, bool, Type, Type, Type &> FunctionType;

    /**
     * Input port type list.
     */
    typedef Inputs<bool, Type, Type> InputsType;

    /**
     * Output port type list.
     */
    typedef Outputs<Type> OutputsType;

    /**
     * Constructs a Select process.
     * \param network The network this node is going to be part of.
     */
    explicit Select(Network & network);

    /**
     * Constructs a Select process.
     * \param network The network this node is going to be part of.
     * \param policy The execution policy of the process.
     */
    Select(Network & network, embb::mtapi::ExecutionPolicy const & policy);

    /**
     * \returns Always \c true.
     */
    virtual bool HasInputs() const;

    /**
     * \returns Reference to a list of all input ports.
     */
    InputsType & GetInputs();

    /**
     * \returns Input port at Index.
     */
    template <int Index>
    typename InputsType::Types<Index>::Result & GetInput();

    /**
     * \returns Always \c true.
     */
    virtual bool HasOutputs() const;

    /**
     * \returns Reference to a list of all output ports.
     */
    OutputsType & GetOutputs();

    /**
     * \returns Output port at Index.
     */
    template <int Index>
    typename OutputsType::Types<Index>::Result & GetOutput();

    /**
     * Connects output port 0 to input port 0 of \c target.
     * \param target Process to connect to.
     * \tparam T Type of target process.
     */
    template <typename T>
    void operator >> (T & target);
  };

  /**
   * Sink process template.
   *
   * A sink marks the end of a particular processing chain. It can have one to
   * five input ports and no output ports.
   * Tokens are processed in order by the sink, regardless in which order they
   * arrive at the input ports.
   *
   * \see Source, SerialProcess, ParallelProcess
   *
   * \tparam I1 Type of first input port.
   * \tparam I2 Optional type of second input port.
   * \tparam I3 Optional type of third input port.
   * \tparam I4 Optional type of fourth input port.
   * \tparam I5 Optional type of fifth input port.
   */
  template<typename I1, typename I2 = embb::base::internal::Nil,
    typename I3 = embb::base::internal::Nil,
    typename I4 = embb::base::internal::Nil,
    typename I5 = embb::base::internal::Nil>
  class Sink {
   public:
    /**
     * Function type to use when processing tokens.
     */
    typedef embb::base::Function<void, INPUT_TYPE_LIST> FunctionType;

    /**
     * Input port type list.
     */
    typedef Inputs<INPUT_TYPE_LIST> InputsType;

    /**
     * Constructs a Sink with a user specified processing function.
     * \param network The network this node is going to be part of.
     * \param function The Function to call to process a token.
     */
    Sink(Network & network, FunctionType function);

    /**
     * Constructs a Sink with a user specified embb::mtapi::Job.
     * The Job must be associated with an action function accepting a struct
     * containing copies of the inputs as its argument buffer and a
     * null pointer as its result buffer.
     * \param network The network this node is going to be part of.
     * \param job The embb::mtapi::Job to process a token.
     */
    Sink(Network & network, embb::mtapi::Job job);

    /**
     * Constructs a Sink with a user specified processing function.
     * \param network The network this node is going to be part of.
     * \param function The Function to call to process a token.
     * \param policy The execution policy of the process.
     */
    Sink(Network & network, FunctionType function,
      embb::mtapi::ExecutionPolicy const & policy);

    /**
     * Constructs a Sink with a user specified embb::mtapi::Job.
     * The Job must be associated with an action function accepting a struct
     * containing copies of the inputs as its argument buffer and a
     * null pointer as its result buffer.
     * \param network The network this node is going to be part of.
     * \param job The embb::mtapi::Job to process a token.
     * \param policy The execution policy of the process.
     */
    Sink(Network & network, embb::mtapi::Job job,
      embb::mtapi::ExecutionPolicy const & policy);

    /**
     * \returns Always \c true.
     */
    virtual bool HasInputs() const;

    /**
     * \returns Reference to a list of all input ports.
     */
    InputsType & GetInputs();

    /**
     * \returns Input port at Index.
     */
    template <int Index>
    typename InputsType::Types<Index>::Result & GetInput();

    /**
     * \returns Always \c false.
     */
    virtual bool HasOutputs() const;
  };

  /**
   * Source process template.
   *
   * A source marks the start of a processing chain. It can have one to five
   * output ports and no input ports.
   * Tokens are emitted in order by the source.
   *
   * \see SerialProcess, ParallelProcess, Sink
   *
   * \tparam O1 Type of first output port.
   * \tparam O2 Optional type of second output port.
   * \tparam O3 Optional type of third output port.
   * \tparam O4 Optional type of fourth output port.
   * \tparam O5 Optional type of fifth output port.
   */
  template<typename O1, typename O2 = embb::base::internal::Nil,
    typename O3 = embb::base::internal::Nil,
    typename O4 = embb::base::internal::Nil,
    typename O5 = embb::base::internal::Nil>
  class Source {
   public:
    /**
     * Function type to use when processing tokens.
     */
    typedef embb::base::Function<void, OUTPUT_TYPE_LIST> FunctionType;

    /**
     * Output port type list.
     */
    typedef Outputs<OUTPUT_TYPE_LIST> OutputsType;

    /**
     * Constructs a Source with a user specified processing function.
     * \param network The network this node is going to be part of.
     * \param function The Function to call to emit a token.
     */
    Source(Network & network, FunctionType function);

    /**
     * Constructs a Source with a user specified embb::mtapi::Job.
     * The Job must be associated with an action function accepting a
     * null pointer as its argument buffer and a struct
     * containing the outputs as its result buffer.
     * \param network The network this node is going to be part of.
     * \param job The embb::mtapi::Job to process a token.
     */
    Source(Network & network, embb::mtapi::Job job);

    /**
     * Constructs a Source with a user specified processing function.
     * \param network The network this node is going to be part of.
     * \param function The Function to call to emit a token.
     * \param policy The execution policy of the process.
     */
    Source(Network & network, FunctionType function,
      embb::mtapi::ExecutionPolicy const & policy);

    /**
     * Constructs a Source with a user specified embb::mtapi::Job.
     * The Job must be associated with an action function accepting a
     * null pointer as its argument buffer and a struct
     * containing the outputs as its result buffer.
     * \param network The network this node is going to be part of.
     * \param job The embb::mtapi::Job to process a token.
     * \param policy The execution policy of the process.
     */
    Source(Network & network, embb::mtapi::Job job,
      embb::mtapi::ExecutionPolicy const & policy);

    /**
     * \returns Always \c false.
     */
    virtual bool HasInputs() const;

    /**
     * \returns Always \c true.
     */
    virtual bool HasOutputs() const;

    /**
     * \returns Reference to a list of all output ports.
     */
    OutputsType & GetOutputs();

    /**
     * \returns Output port at INDEX.
     */
    template <int Index>
    typename OutputsType::Types<Index>::Result & GetOutput();

    /**
     * Connects output port 0 to input port 0 of \c target.
     * \param target Process to connect to.
     * \tparam T Type of target process.
     */
    template <typename T>
    void operator >> (T & target);
  };

  /**
   * Constant source process template.
   *
   * A constant source has one output port and emits a constant value given
   * at construction time for each token.
   *
   * \tparam Type The type of output port 0.
   */
  template<typename Type>
  class ConstantSource {
   public:
    /**
     * Output port type list.
     */
    typedef Outputs<OUTPUT_TYPE_LIST> OutputsType;

    /**
     * Constructs a ConstantSource with a value to emit on each token.
     * \param network The network this node is going to be part of.
     * \param value The value to emit.
     */
    ConstantSource(Network & network, Type value);

    /**
     * Constructs a ConstantSource with a value to emit on each token.
     * \param network The network this node is going to be part of.
     * \param value The value to emit.
     * \param policy The execution policy of the process.
     */
    ConstantSource(Network & network, Type value,
      embb::mtapi::ExecutionPolicy const & policy);

    /**
     * \returns Always \c false.
     */
    virtual bool HasInputs() const;

    /**
     * \returns Always \c true.
     */
    virtual bool HasOutputs() const;

    /**
     * \returns Reference to a list of all output ports.
     */
    OutputsType & GetOutputs();

    /**
     * \returns Output port at Index.
     */
    template <int Index>
    typename OutputsType::Types<Index>::Result & GetOutput();

    /**
     * Connects output port 0 to input port 0 of \c target.
     * \param target Process to connect to.
     * \tparam T Type of target process.
     */
    template <typename T>
    void operator >> (T & target);
  };

  /**
   * Checks whether the network is completely connected and free of cycles.
   * \returns \c true if everything is in order, \c false if not.
   * \note Executing an invalid network results in an exception. For this
   * reason, it is recommended to first check the network using IsValid().
   */
  bool IsValid();

  /**
   * Executes the network until one of the the sources returns \c false.
   * \note If the network was default constructed, the number of concurrent
   * tokens will automatically be derived from the structure of the network 
   * on the first call of the operator, and the corresponding resources will
   * be allocated then.
   * \note Executing an invalid network results in an exception. For this
   * reason, it is recommended to first check the network using IsValid().
   */
  void operator () ();
};

#else

class Network : public internal::ClockListener {
 public:
  Network()
    : sink_counter_(NULL), sink_count_(0)
    , slices_(0), sched_(NULL), policy_() {
    // empty
  }

  explicit Network(int slices)
    : sink_counter_(NULL), sink_count_(0)
    , slices_(slices), sched_(NULL), policy_() {
    PrepareSlices();
  }

  explicit Network(embb::mtapi::ExecutionPolicy const & policy)
    : sink_counter_(NULL), sink_count_(0)
    , slices_(0), sched_(NULL), policy_(policy) {
  }

  Network(int slices, embb::mtapi::ExecutionPolicy const & policy)
    : sink_counter_(NULL), sink_count_(0)
    , slices_(slices), sched_(NULL), policy_(policy) {
    PrepareSlices();
  }

  ~Network() {
    if (NULL != sched_) {
      embb::base::Allocation::Delete(sched_);
      sched_ = NULL;
    }
    if (NULL != sink_counter_) {
      for (int ii = 0; ii < slices_; ii++) {
        sink_counter_[ii].~Atomic<int>();
      }
      embb::base::Allocation::Free(sink_counter_);
      sink_counter_ = NULL;
    }
  }

  template <typename T1,
    typename T2 = embb::base::internal::Nil,
    typename T3 = embb::base::internal::Nil,
    typename T4 = embb::base::internal::Nil,
    typename T5 = embb::base::internal::Nil>
  class Inputs {
    // empty
  };

  template <typename T1,
    typename T2 = embb::base::internal::Nil,
    typename T3 = embb::base::internal::Nil,
    typename T4 = embb::base::internal::Nil,
    typename T5 = embb::base::internal::Nil>
  class Outputs {
    // empty
  };

  template <class Inputs, class Outputs> class SerialProcess;

  template <
    typename I1, typename I2, typename I3, typename I4, typename I5,
    typename O1, typename O2, typename O3, typename O4, typename O5>
  class SerialProcess< Inputs<I1, I2, I3, I4, I5>,
    Outputs<O1, O2, O3, O4, O5> >
    : public internal::Process< true,
        internal::Inputs<I1, I2, I3, I4, I5>,
        internal::Outputs<O1, O2, O3, O4, O5> > {
   public:
    typedef typename internal::Process< true,
      internal::Inputs<I1, I2, I3, I4, I5>,
      internal::Outputs<O1, O2, O3, O4, O5> >::FunctionType
        FunctionType;

    SerialProcess(Network & network, FunctionType function)
      : internal::Process< true,
          internal::Inputs<I1, I2, I3, I4, I5>,
          internal::Outputs<O1, O2, O3, O4, O5> >(
            network.sched_, function) {
      this->SetPolicy(network.policy_);
      network.processes_.push_back(this);
    }

    SerialProcess(Network & network, FunctionType function,
      embb::mtapi::ExecutionPolicy const & policy)
      : internal::Process< true,
      internal::Inputs<I1, I2, I3, I4, I5>,
      internal::Outputs<O1, O2, O3, O4, O5> >(
        network.sched_, function) {
      this->SetPolicy(policy);
      network.processes_.push_back(this);
    }

    SerialProcess(Network & network, embb::mtapi::Job job)
      : internal::Process< true,
      internal::Inputs<I1, I2, I3, I4, I5>,
      internal::Outputs<O1, O2, O3, O4, O5> >(
        network.sched_, job) {
      network.processes_.push_back(this);
    }
  };

  template <class Inputs, class Outputs> class ParallelProcess;

  template <
    typename I1, typename I2, typename I3, typename I4, typename I5,
    typename O1, typename O2, typename O3, typename O4, typename O5>
  class ParallelProcess< Inputs<I1, I2, I3, I4, I5>,
    Outputs<O1, O2, O3, O4, O5> >
    : public internal::Process< false,
        internal::Inputs<I1, I2, I3, I4, I5>,
        internal::Outputs<O1, O2, O3, O4, O5> >{
   public:
    typedef typename internal::Process< false,
      internal::Inputs<I1, I2, I3, I4, I5>,
      internal::Outputs<O1, O2, O3, O4, O5> >::FunctionType
        FunctionType;

    ParallelProcess(Network & network, FunctionType function)
      : internal::Process< false,
          internal::Inputs<I1, I2, I3, I4, I5>,
          internal::Outputs<O1, O2, O3, O4, O5> >(
            network.sched_, function) {
      this->SetPolicy(network.policy_);
      network.processes_.push_back(this);
    }

    ParallelProcess(Network & network, FunctionType function,
      embb::mtapi::ExecutionPolicy const & policy)
      : internal::Process< false,
      internal::Inputs<I1, I2, I3, I4, I5>,
      internal::Outputs<O1, O2, O3, O4, O5> >(
        network.sched_, function) {
      this->SetPolicy(policy);
      network.processes_.push_back(this);
    }

    ParallelProcess(Network & network, embb::mtapi::Job job)
      : internal::Process< false,
      internal::Inputs<I1, I2, I3, I4, I5>,
      internal::Outputs<O1, O2, O3, O4, O5> >(
        network.sched_, job) {
      network.processes_.push_back(this);
    }
  };

  template<typename Type>
  class Switch : public internal::Switch<Type> {
   public:
    explicit Switch(Network & network)
      : internal::Switch<Type>(network.sched_) {
      this->SetPolicy(network.policy_);
      network.processes_.push_back(this);
    }

    Switch(Network & network, embb::mtapi::ExecutionPolicy const & policy)
      : internal::Switch<Type>(network.sched_) {
      this->SetPolicy(policy);
      network.processes_.push_back(this);
    }
  };

  template<typename Type>
  class Select : public internal::Select<Type> {
   public:
    explicit Select(Network & network)
      : internal::Select<Type>(network.sched_) {
      this->SetPolicy(network.policy_);
      network.processes_.push_back(this);
    }

    Select(Network & network, embb::mtapi::ExecutionPolicy const & policy)
      : internal::Select<Type>(network.sched_) {
      this->SetPolicy(policy);
      network.processes_.push_back(this);
    }
  };

  template<typename I1, typename I2 = embb::base::internal::Nil,
    typename I3 = embb::base::internal::Nil,
    typename I4 = embb::base::internal::Nil,
    typename I5 = embb::base::internal::Nil>
  class Sink : public internal::Sink<
    internal::Inputs<I1, I2, I3, I4, I5> > {
   public:
    typedef typename internal::Sink<
      internal::Inputs<I1, I2, I3, I4, I5> >::FunctionType FunctionType;

    Sink(Network & network, FunctionType function)
      : internal::Sink<
          internal::Inputs<I1, I2, I3, I4, I5> >(
            network.sched_, &network, function) {
      this->SetPolicy(network.policy_);
      network.sinks_.push_back(this);
      network.sink_count_++;
    }

    Sink(Network & network, FunctionType function,
      embb::mtapi::ExecutionPolicy const & policy)
      : internal::Sink<
      internal::Inputs<I1, I2, I3, I4, I5> >(
        network.sched_, &network, function) {
      this->SetPolicy(policy);
      network.sinks_.push_back(this);
      network.sink_count_++;
    }

    Sink(Network & network, embb::mtapi::Job job)
      : internal::Sink<
      internal::Inputs<I1, I2, I3, I4, I5> >(
        network.sched_, &network, job) {
      network.sinks_.push_back(this);
      network.sink_count_++;
    }
  };

  template<typename O1, typename O2 = embb::base::internal::Nil,
    typename O3 = embb::base::internal::Nil,
    typename O4 = embb::base::internal::Nil,
    typename O5 = embb::base::internal::Nil>
  class Source : public internal::Source<
    internal::Outputs<O1, O2, O3, O4, O5> > {
   public:
    typedef typename internal::Source<
      internal::Outputs<O1, O2, O3, O4, O5> >::FunctionType
        FunctionType;

    Source(Network & network, FunctionType function)
      : internal::Source<
          internal::Outputs<O1, O2, O3, O4, O5> >(network.sched_, function) {
      this->SetPolicy(network.policy_);
      network.sources_.push_back(this);
    }

    Source(Network & network, FunctionType function,
      embb::mtapi::ExecutionPolicy const & policy)
      : internal::Source<
      internal::Outputs<O1, O2, O3, O4, O5> >(network.sched_, function) {
      this->SetPolicy(policy);
      network.sources_.push_back(this);
    }

    Source(Network & network, embb::mtapi::Job job)
      : internal::Source<
      internal::Outputs< O1, O2, O3, O4, O5> >(network.sched_, job) {
      network.sources_.push_back(this);
    }
  };

  template<typename Type>
  class ConstantSource : public internal::ConstantSource<Type> {
   public:
    ConstantSource(Network & network, Type value)
      : internal::ConstantSource<Type>(network.sched_, value) {
      this->SetPolicy(network.policy_);
      network.sources_.push_back(this);
    }

    ConstantSource(Network & network, Type value,
      embb::mtapi::ExecutionPolicy const & policy)
      : internal::ConstantSource<Type>(network.sched_, value) {
      this->SetPolicy(policy);
      network.sources_.push_back(this);
    }
  };

  bool IsValid() {
    bool valid = true;
    // check connectivity
    for (size_t ii = 0; ii < sources_.size() && valid; ii++) {
      valid = valid && sources_[ii]->IsFullyConnected();
    }
    for (size_t ii = 0; ii < processes_.size() && valid; ii++) {
      valid = valid && processes_[ii]->IsFullyConnected();
    }
    for (size_t ii = 0; ii < sinks_.size() && valid; ii++) {
      valid = valid && sinks_[ii]->IsFullyConnected();
    }
    // check for cycles
    for (size_t ii = 0; ii < processes_.size() && valid; ii++) {
      valid = valid && !processes_[ii]->HasCycle();
    }
    return valid;
  }

  void operator () () {
    if (0 >= slices_) {
      slices_ = static_cast<int>(
        sources_.size() +
        sinks_.size());
      for (size_t ii = 0; ii < processes_.size(); ii++) {
        int tt = processes_[ii]->IsSequential() ? 1 :
          static_cast<int>(embb_core_count_available());
        slices_ += tt;
      }
      PrepareSlices();
      for (size_t ii = 0; ii < sources_.size(); ii++) {
        sources_[ii]->SetScheduler(sched_);
      }
      for (size_t ii = 0; ii < processes_.size(); ii++) {
        processes_[ii]->SetScheduler(sched_);
      }
      for (size_t ii = 0; ii < sinks_.size(); ii++) {
        sinks_[ii]->SetScheduler(sched_);
      }
    }

    int clock = 0;
    while (clock >= 0) {
      const int idx = clock % slices_;
      while (sink_counter_[idx] > 0) {
        sched_->YieldToScheduler();
      }
      if (!SpawnClock(clock))
        break;
      clock++;
    }

    int ii = clock - slices_ + 1;
    if (ii < 0) ii = 0;
    for (; ii < clock; ii++) {
      const int idx = ii % slices_;
      while (sink_counter_[idx] > 0) {
        sched_->YieldToScheduler();
      }
    }
  }

  /**
   * Internal.
   * \internal
   * Gets called when a token has reached all sinks and frees up the
   * corresponding slot, thus allowing a new token to be emitted.
   */
  virtual void OnClock(int clock) {
    const int idx = clock % slices_;
    assert(sink_counter_[idx] > 0);
    --sink_counter_[idx];
  }

 private:
  std::vector<internal::Node*> processes_;
  std::vector<internal::Node*> sources_;
  std::vector<internal::Node*> sinks_;
  embb::base::Atomic<int> * sink_counter_;
  int sink_count_;
  int slices_;
  internal::Scheduler * sched_;
  embb::mtapi::ExecutionPolicy policy_;

#if EMBB_DATAFLOW_TRACE_SIGNAL_HISTORY
  std::vector<int> spawn_history_[Slices];
#endif

  bool SpawnClock(int clock) {
    const int idx = clock % slices_;
#if EMBB_DATAFLOW_TRACE_SIGNAL_HISTORY
    spawn_history_[idx].push_back(clock);
#endif
    assert(sink_counter_[idx] == 0);
    sink_counter_[idx] = sink_count_;
    for (size_t kk = 0; kk < sources_.size(); kk++) {
      sources_[kk]->Start(clock);
    }
    bool result = true;
    for (size_t kk = 0; kk < sources_.size(); kk++) {
      result &= sources_[kk]->Wait(clock);
    }
    return result;
  }

  void PrepareSlices() {
    sched_ = embb::base::Allocation::New<internal::SchedulerMTAPI>(slices_);
    if (sched_->GetSlices() != slices_) {
      slices_ = sched_->GetSlices();
    }
    sink_counter_ = reinterpret_cast<embb::base::Atomic<int>*>(
      embb::base::Allocation::Allocate(
        sizeof(embb::base::Atomic<int>)*slices_));
    for (int ii = 0; ii < slices_; ii++) {
      new(sink_counter_ + ii) embb::base::Atomic<int>(0);
    }
  }
};

#endif // DOXYGEN

} // namespace dataflow
} // namespace embb

#endif // EMBB_DATAFLOW_NETWORK_H_
