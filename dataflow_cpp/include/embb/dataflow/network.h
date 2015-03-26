/*
 * Copyright (c) 2014-2015, Siemens AG. All rights reserved.
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
 * Represents a set of processes, that are connected by communication channels.
 *
 * \tparam Slices Number of concurrently processed tokens.
 * \ingroup CPP_DATAFLOW
 */
template <int Slices>
class Network {
 public:
  /**
   * Constructs an empty network.
   */
  Network() {}

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
     * \param function The Function to call to process a token.
     */
    explicit SerialProcess(FunctionType function);

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
    * \param function The Function to call to process a token.
    */
    explicit ParallelProcess(FunctionType function);

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
     * \param function The Function to call to process a token.
     */
    explicit Sink(FunctionType function);

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
     * \param function The Function to call to emit a token.
     */
    explicit Source(FunctionType function);

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
   * Adds a new source process to the network.
   * \param source The source process to add.
   */
  template<typename O1, typename O2, typename O3, typename O4, typename O5>
  void AddSource(Source<O1, O2, O3, O4, O5> & source);

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
     * \param value The value to emit.
     */
    explicit ConstantSource(Type value);

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
   * Adds a new constant source process to the network.
   * \param source The constant source process to add.
   */
  template<typename Type>
  void AddSource(ConstantSource<Type> & source);

  /**
   * Executes the network until one of the the sources returns \c false.
   */
  void operator () ();
};

#else

template <int Slices>
class Network : public internal::ClockListener {
 public:
  Network() {}

  template <typename T1, typename T2 = embb::base::internal::Nil,
    typename T3 = embb::base::internal::Nil,
    typename T4 = embb::base::internal::Nil,
    typename T5 = embb::base::internal::Nil>
  struct Inputs {
    typedef internal::Inputs<Slices, T1, T2, T3, T4, T5> Type;
  };

  template <typename T1, typename T2 = embb::base::internal::Nil,
    typename T3 = embb::base::internal::Nil,
    typename T4 = embb::base::internal::Nil,
    typename T5 = embb::base::internal::Nil>
  struct Outputs {
    typedef internal::Outputs<Slices, T1, T2, T3, T4, T5> Type;
  };

  template <class Inputs, class Outputs> class SerialProcess;

  template <
    typename I1, typename I2, typename I3, typename I4, typename I5,
    typename O1, typename O2, typename O3, typename O4, typename O5>
  class SerialProcess< internal::Inputs<Slices, I1, I2, I3, I4, I5>,
    internal::Outputs<Slices, O1, O2, O3, O4, O5> >
    : public internal::Process< Slices, true,
        internal::Inputs<Slices, I1, I2, I3, I4, I5>,
        internal::Outputs<Slices, O1, O2, O3, O4, O5> > {
   public:
    typedef typename internal::Process< Slices, true,
      internal::Inputs<Slices, I1, I2, I3, I4, I5>,
      internal::Outputs<Slices, O1, O2, O3, O4, O5> >::FunctionType
        FunctionType;
    explicit SerialProcess(FunctionType function)
      : internal::Process< Slices, true,
          internal::Inputs<Slices, I1, I2, I3, I4, I5>,
          internal::Outputs<Slices, O1, O2, O3, O4, O5> >(function) {
      //empty
    }
  };

  template <class Inputs, class Outputs> class ParallelProcess;

  template <
    typename I1, typename I2, typename I3, typename I4, typename I5,
    typename O1, typename O2, typename O3, typename O4, typename O5>
  class ParallelProcess< internal::Inputs<Slices, I1, I2, I3, I4, I5>,
    internal::Outputs<Slices, O1, O2, O3, O4, O5> >
    : public internal::Process< Slices, false,
        internal::Inputs<Slices, I1, I2, I3, I4, I5>,
        internal::Outputs<Slices, O1, O2, O3, O4, O5> >{
   public:
    typedef typename internal::Process< Slices, false,
      internal::Inputs<Slices, I1, I2, I3, I4, I5>,
      internal::Outputs<Slices, O1, O2, O3, O4, O5> >::FunctionType
        FunctionType;
    explicit ParallelProcess(FunctionType function)
      : internal::Process< Slices, false,
          internal::Inputs<Slices, I1, I2, I3, I4, I5>,
          internal::Outputs<Slices, O1, O2, O3, O4, O5> >(function) {
      //empty
    }
  };

  template<typename Type>
  class Switch : public internal::Switch<Slices, Type> {
   public:
  };

  template<typename Type>
  class Select : public internal::Select<Slices, Type> {
   public:
  };

  template<typename I1, typename I2 = embb::base::internal::Nil,
    typename I3 = embb::base::internal::Nil,
    typename I4 = embb::base::internal::Nil,
    typename I5 = embb::base::internal::Nil>
  class Sink : public internal::Sink<Slices,
    internal::Inputs<Slices, I1, I2, I3, I4, I5> > {
   public:
    typedef typename internal::Sink<Slices,
      internal::Inputs<Slices, I1, I2, I3, I4, I5> >::FunctionType FunctionType;

    explicit Sink(FunctionType function)
      : internal::Sink<Slices,
          internal::Inputs<Slices, I1, I2, I3, I4, I5> >(function) {
      //empty
    }
  };

  template<typename O1, typename O2 = embb::base::internal::Nil,
    typename O3 = embb::base::internal::Nil,
    typename O4 = embb::base::internal::Nil,
    typename O5 = embb::base::internal::Nil>
  class Source : public internal::Source<Slices,
    internal::Outputs<Slices, O1, O2, O3, O4, O5> > {
   public:
    typedef typename internal::Source<Slices,
      internal::Outputs<Slices, O1, O2, O3, O4, O5> >::FunctionType
        FunctionType;

    explicit Source(FunctionType function)
      : internal::Source<Slices,
          internal::Outputs<Slices, O1, O2, O3, O4, O5> >(function) {
      //empty
    }
  };

  template<typename O1, typename O2, typename O3, typename O4, typename O5>
  void AddSource(Source<O1, O2, O3, O4, O5> & source) {
    sources_.push_back(&source);
  }

  template<typename Type>
  class ConstantSource : public internal::ConstantSource<Slices, Type> {
   public:
    explicit ConstantSource(Type value)
      : internal::ConstantSource<Slices, Type>(value) {
      //empty
    }
  };

  template<typename Type>
  void AddSource(ConstantSource<Type> & source) {
    sources_.push_back(&source);
  }

  void operator () () {
    internal::SchedulerSequential sched_seq;
    internal::SchedulerMTAPI<Slices> sched_mtapi;
    internal::Scheduler * sched = &sched_mtapi;

    internal::InitData init_data;
    init_data.sched = sched;
    init_data.sink_listener = this;

    sink_count_ = 0;
    for (size_t it = 0; it < sources_.size(); it++)
      sources_[it]->Init(&init_data);

    for (int ii = 0; ii < Slices; ii++) sink_counter_[ii] = 0;

    int clock = 0;
    while (clock >= 0) {
      const int idx = clock % Slices;
      while (sink_counter_[idx] > 0) embb::base::Thread::CurrentYield();
      sched->WaitForSlice(idx);
      if (!SpawnClock(clock))
        break;
      clock++;
    }

    int ii = clock - Slices + 1;
    if (ii < 0) ii = 0;
    for (; ii < clock; ii++) {
      const int idx = ii % Slices;
      while (sink_counter_[idx] > 0) embb::base::Thread::CurrentYield();
      sched->WaitForSlice(idx);
    }
  }

  /**
   * Internal.
   * \internal
   * Gets called when a token has reached all sinks and frees up the
   * corresponding slot, thus allowing a new token to be emitted.
   */
  virtual void OnClock(int clock) {
    const int idx = clock % Slices;
    const int cnt = --sink_counter_[idx];
    if (cnt < 0)
      EMBB_THROW(embb::base::ErrorException,
        "More sinks than expected signaled reception of given clock.")
  }

  /**
   * Internal.
   * \internal
   * Gets called when an init token has reached all sinks.
   */
  virtual void OnInit(internal::InitData * /*sched*/) {
    sink_count_++;
  }

 private:
  std::vector<internal::Node*> processes_;
  std::vector<internal::Node*> sources_;
  std::vector<internal::Node*> sinks_;
  embb::base::Atomic<int> sink_counter_[Slices];
  int sink_count_;

#if EMBB_DATAFLOW_TRACE_SIGNAL_HISTORY
  std::vector<int> spawn_history_[Slices];
#endif

  bool SpawnClock(int clock) {
    const int idx = clock % Slices;
    bool result = true;
#if EMBB_DATAFLOW_TRACE_SIGNAL_HISTORY
    spawn_history_[idx].push_back(clock);
#endif
    sink_counter_[idx] = sink_count_;
    for (size_t kk = 0; kk < sources_.size(); kk++) {
      result &= sources_[kk]->Start(clock);
    }
    return result;
  }
};

#endif // DOXYGEN

} // namespace dataflow
} // namespace embb

#endif // EMBB_DATAFLOW_NETWORK_H_
