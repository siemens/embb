/*
 * Copyright (c) 2014, Siemens AG. All rights reserved.
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

#ifndef EMBB_CONTAINERS_WAIT_FREE_MPMC_QUEUE_H_
#define EMBB_CONTAINERS_WAIT_FREE_MPMC_QUEUE_H_

#include <embb/containers/wait_free_array_value_pool.h>
#include <embb/containers/internal/indexed_object_pool.h>
#include <embb/containers/internal/hazard_pointer.h>
#include <embb/containers/internal/returning_true_iterator.h>
#include <embb/base/atomic.h>
#include <embb/base/function.h>
#include <embb/base/c/internal/thread_index.h>
#include <stdlib.h>

namespace embb {
namespace containers {

namespace internal {

/**
 * Queue node
 *
 * Single linked list, contains the element (\c value) and a pool index
 * referencing the next node (\c next_idx).
 *
 * \tparam ElementT Element type
 */
template<typename Type>
class WaitFreeMPMCQueueNode {
 private:
  typedef WaitFreeMPMCQueueNode<Type> self_t;

 public:
  typedef uint32_t index_t;
  static const index_t UndefinedIndex;

 private:
  /**
   * Node value
   */
  Type value;

  /**
   * Enqeue accessor id
   */
  uint32_t enq_aid;

  /**
   * Pool-index of next Node in list (atomic), -1 for none
   */
  embb::base::Atomic<uint32_t> next_idx;

  /**
   * Dequeue accessor id (atomic), -1 for none
   */
  embb::base::Atomic<uint32_t> deq_aid;

 public:
  /**
   * Default constructor
   */
  inline WaitFreeMPMCQueueNode();

  /**
   * Copy constructor
   */
  inline WaitFreeMPMCQueueNode(
    const self_t & other
    /**< [IN] Instance to copy */
  );

  /**
   * Assignment operator
   */
  inline self_t & operator=(
    const self_t & other
    /**< [IN] Instance to assign */
  );

  /**
   * Constructs a new instance of Node.
   */
  inline WaitFreeMPMCQueueNode(
    Type const val,
    /**< [IN] Value to be contained in the Node instance */
    uint32_t enqAid
    /**< [IN] Enqueuer accessor id */
  );

  inline Type Value() const;

  inline uint32_t NextPoolIdx() const;

  /**
   * Set pointer to next Node element via CAS.
   *
   * \return true if new pointer value could be set.
   */
  inline bool CASNext(
    uint32_t expNextIdx,
    /**< [IN] Expected current pointer value */
    uint32_t newNextIdx
    /**< [IN] New pointer value to set */
  );

  inline bool NextIsNull() const;

  inline uint32_t EnqueueAID() const;

  inline embb::base::Atomic<index_t> & DequeueAID();
};

}  // namespace internal

/**
 * Wait-free queue for multiple enqueuers and dequeuers
 *
 * \concept{CPP_CONCEPTS_QUEUE}
 *
 * \ingroup CPP_CONTAINERS_QUEUES
 *
 * \see LockFreeMPMCQueue
 * \see WaitFreeSPSCQueue
 *
 * \tparam Type Type of the queue elements
 * \tparam NodeAllocator Allocator type for allocating queue nodes
 * \tparam OpAllocator Allocator type for allocating operation description
 *                     objects
 * \tparam ValuePool Type of the value pool used as basis for the ObjectPool
 *                   which stores queue nodes
 */
template<
  typename Type,
  class ValuePool =
    WaitFreeArrayValuePool<bool, false>,
  class NodeAllocator =
    embb::base::AllocatorCacheAligned<internal::WaitFreeMPMCQueueNode<Type> >,
  class OpAllocator =
    embb::base::AllocatorCacheAligned<embb::base::Atomic<uint32_t> >
>
class WaitFreeMPMCQueue {
 private:
  typedef internal::WaitFreeMPMCQueueNode<Type> Node_t;
  typedef typename internal::WaitFreeMPMCQueueNode<Type>::index_t index_t;
  typedef WaitFreeMPMCQueue<Type, ValuePool, NodeAllocator, OpAllocator> self_t;
  typedef internal::IndexedObjectPool<
    internal::WaitFreeMPMCQueueNode<Type>, ValuePool, NodeAllocator> NodePool_t;

 private:
  /**
   * Maximum size of queue. Using maximum value of
   * OperationDesc::NodeIndex (15 bit) minus one element
   * required for sentinel node.
   */
  static const index_t QUEUE_SIZE_MAX = static_cast<index_t>(0x3FFFFFFF - 1);
  
  /**
   *  Number of guards per thread
   */
  static const size_t num_guards = 2;

  /**
   *  Null-pointer for hazard pointers
   */
  static const index_t UndefinedGuard = 0x3fffffff;

  /**
   * Helper class for operation descriptions.
   * Used instead of bit-field struct for portability.
   */
  class OperationDesc {
   private:
    static const index_t PENDING_FLAG_MASK = 0x80000000; ///< Bit 32
    static const index_t ENQUEUE_FLAG_MASK = 0x40000000; ///< Bit 31
    static const index_t NODE_INDEX_MASK   = 0x3FFFFFFF; ///< Bit 30..1

   private:
    OperationDesc();

   public:
    bool Pending;
    bool Enqueue;
    index_t NodeIndex;
    index_t Raw;

   public:
    /**
     * Converts state to binary value { pending:1, enqueue:1, nodeIndex:30 }
     */
    OperationDesc(
      bool pending,
      bool enqueue,
      index_t nodeIndex);

    /**
     * Expects binary value { pending:1, enqueue:1, nodeIndex:30 }
     */
    explicit OperationDesc(index_t raw);
  };

  /** Index of head node in node pool, should be aligned */
  embb::base::Atomic<index_t> headIdx;
  /** Index of tail node in node pool, should be aligned */
  embb::base::Atomic<index_t> tailIdx;
  /** Maximum element capacity of the queue */
  size_t max_size_;
  /** Callback instance for release of guarded node indices */
  embb::base::Function< void, index_t > delete_pointer_callback;
  /** Hazard pointer for node index (guards stack top pointer) */
  embb::containers::internal::HazardPointer< index_t > hp;
  /** Instance of empty node used as sentinel */
  Node_t nullNode;
  /** Maximum number of threads accessing this queue instance */
  size_t num_states;
  /** Capacity of the node pool, includes overhead due to hazard pointers */
  size_t node_pool_size;
  /** Array containing two states for every concurrent accessor on the
      queue (needed for swapping) */
  embb::base::Atomic<index_t> * operationDescriptions;
  /** Pool for element nodes in the queue */
  NodePool_t nodePool;
  /** Allocator for memory used for operation descriptions */
  OpAllocator operationDescriptionAllocator;
  /** Index of sentinel node, stored for release in destructor */
  index_t sentinelNodeIndex;

 private:
  /**
   * Resolves thread index usable as accessor id.
   *
   * \returns True if thread index could be resolved, false otherwise.
   *          A call could fail, e.g. if there have been created more
   *          threads than initially configured in the embb runtime.
   */
  inline bool LoadAccessorThreadIndex(
    index_t & retIndexValue
    /**< [OUT] Value of thread index */
  );

  /**
   * Resolves maximum required capacity for retired lists in hazard
   * pointer implementation
   */
  inline size_t RetiredListMaxSize(
    size_t nThreads
    /**< [IN] maximum number of threads operating on the queue */
  );

 public:
  /**
   * Creates a queue with the specified capacity.
   *
   * \memory
   * Let \c t be the maximum number of threads and \c x be <tt>2.5*t+1</tt>.
   * Then, <tt>x*(3*t+1)</tt> elements of size <tt>sizeof(void*)</tt>, \c x
   * elements of size <tt>sizeof(Type)</tt>, and \c capacity+1 elements of size
   * <tt>sizeof(Type)</tt> are allocated.
   *
   * \notthreadsafe
   *
   * \see CPP_CONCEPTS_QUEUE
   */
  WaitFreeMPMCQueue(
    size_t capacity
    /**< [IN] Element capacity of the queue */
  );

  /**
   * Destroys the queue.
   *
   * \notthreadsafe
   */
  ~WaitFreeMPMCQueue();

  /**
   * Tries to enqueue an element into the queue.
   *
   * \return \c true if the element could be enqueued, \c false if the queue is
   * full.
   *
   * \waitfree
   *
   * \note It might be possible to enqueue more elements into the queue than its
   * capacity permits.
   *
   * \see CPP_CONCEPTS_QUEUE
   */
  bool TryEnqueue(
    Type const & element
    /**< [IN] Const reference to the element that shall be enqueued */
  );

  /**
   * Tries to dequeue an element from the queue.
   *
   * \return \c true if an element could be dequeued, \c false if the queue is
   * empty.
   *
   * \waitfree
   *
   * \see CPP_CONCEPTS_QUEUE
   */
  bool TryDequeue(
    Type & retElement
    /**< [IN,OUT] Reference to the dequeued element. Unchanged, if the
                  operation was not successful. */
  );

  /**
   * Returns the maximum size for instances of this queue.
   *
   * \waitfree
   */
  inline size_t GetCapacity();
  
 private:
  /**
   * Help progressing pending enqueue operations of given accessors
   */
  void HelpEnqueue(
    unsigned int accessorId
    /**< [IN] Accessor id of operations to help */
  );

  /**
   * Help finishing pending enqueue operations of arbitrary accessors,
   * including own pending enqueue operations.
   *
   * \waitfree
   */
  void HelpFinishEnqueue();

  /**
   * Help progressing pending dequeue operations of given accessor,
   * including own pending dequeue operations.
   *
   * \waitfree
   */
  void HelpDequeue(
    index_t accessorId
    /**< [IN] Accessor id of operations to help */
  );

  /**
   * Help finishing pending dequeue operations of arbitrary accessors,
   * including own pending dequeue operations.
   *
   * \waitfree
   */
  void HelpFinishDequeue();

  /**
   * Help finishing pending operations of arbitrary accessors, including
   * own pending operations.
   * One operation of every other thread is completed before engaging in the
   * calling thread's own announced operation.
   *
   * \waitfree
   */
  void Help();

  /**
   * Whether the accessor with given id is pending.
   *
   * \returns True if the given accessor has a pending operation.
   *
   * \waitfree
   */
  inline bool IsPending(
    unsigned int accessorId
    /**< [IN] Accessor id of operations to help */
  );

  /**
   * Whether the node with given index is in the process of being
   * dequeued. Prevents reclamation of node id when it cannot be
   * guarded by hazard pointers, between HelpDequeue and HelpFinishDequeue.
   */
  bool NodeIsPending(
    index_t nodeId
    /**< [IN] Pool index of the node instance to test */
  );

  /**
   * Callback used for deallocating a node index from hazard
   * pointers. Frees the associated node in the pool.
   *
   * \waitfree
   */
  void DeleteNodeCallback(
    index_t releasedNodeIndex
    /**< [IN] Pool index of the node instance to release */
  );
};

} // namespace containers
} // namespace embb

#include <embb/containers/internal/wait_free_mpmc_queue-inl.h>

#endif  // EMBB_CONTAINERS_WAIT_FREE_MPMC_QUEUE_H_
