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

#ifndef EMBB_CONTAINERS_INTERNAL_WAIT_FREE_MPMC_QUEUE_INL_H_
#define EMBB_CONTAINERS_INTERNAL_WAIT_FREE_MPMC_QUEUE_INL_H_

#include <embb/base/atomic.h>
#include <embb/base/function.h>
#include <embb/base/c/internal/thread_index.h>

namespace embb {
namespace containers {

namespace internal {

template<typename Type>
inline WaitFreeMPMCQueueNode<Type>::WaitFreeMPMCQueueNode()
: enq_aid(UndefinedIndex) {
  next_idx.Store(UndefinedIndex);
  deq_aid.Store(UndefinedIndex);
}

template<typename Type>
inline WaitFreeMPMCQueueNode<Type>::WaitFreeMPMCQueueNode(
  const self_t & other)
: value(other.value),
  enq_aid(other.enq_aid) {
  next_idx.Store(other.next_idx.Load());
  deq_aid.Store(other.deq_aid.Load());
}

template<typename Type>
inline WaitFreeMPMCQueueNode<Type>::WaitFreeMPMCQueueNode(
  Type const val, uint32_t enqAid)
: value(val), enq_aid(enqAid) {
  next_idx.Store(UndefinedIndex);
  deq_aid.Store(UndefinedIndex);
}

template<typename Type>
inline Type WaitFreeMPMCQueueNode<Type>::Value() const {
  return value;
}

template<typename Type>
inline uint32_t WaitFreeMPMCQueueNode<Type>::NextPoolIdx() const {
  return next_idx.Load();
}

template<typename Type>
inline WaitFreeMPMCQueueNode<Type> &
WaitFreeMPMCQueueNode<Type>::operator=(
  const self_t & other) {
  if (this != &other) {
    next_idx.Store(other.next_idx.Load());
    deq_aid.Store(other.deq_aid.Load());
    value = other.value;
    enq_aid = other.enq_aid;
  }
  return *this;
}

template<typename Type>
bool WaitFreeMPMCQueueNode<Type>::CASNext(
  uint32_t expNextIdx, uint32_t newNextIdx) {
  return next_idx.CompareAndSwap(expNextIdx, newNextIdx);
}

template<typename Type>
bool WaitFreeMPMCQueueNode<Type>::Next_IsNull() const {
  return next_idx.Load() == UndefinedIndex;
}

template<typename Type>
inline uint32_t WaitFreeMPMCQueueNode<Type>::EnqueueAID() const {
  return enq_aid;
}

template<typename Type>
embb::base::Atomic<uint32_t> & WaitFreeMPMCQueueNode<Type>::DequeueAID() {
  return deq_aid;
}

/// Using maximum value of OperationDesc::NodeIndex (15 bit) to
/// represent 'undefined'.
template<typename Type>
const uint32_t WaitFreeMPMCQueueNode<Type>::UndefinedIndex = 0x3fffffff;

}  // namespace internal

template<
  typename Type, class NodeAllocator, class OpAllocator, class ValuePool >
WaitFreeMPMCQueue<Type, NodeAllocator, OpAllocator, ValuePool>::OperationDesc::
OperationDesc(
  bool pending,
  bool enqueue,
  index_t nodeIndex) :
  Pending(pending),
  Enqueue(enqueue),
  NodeIndex(nodeIndex),
  Raw(0) {
  index_t nodeIndexMask = NodeIndex & NODE_INDEX_MASK;
  if (Pending) {
    Raw |= PENDING_FLAG_MASK;
  }
  if (Enqueue) {
    Raw |= ENQUEUE_FLAG_MASK;
  }
  Raw |= nodeIndexMask;
}

template<
  typename Type, class NodeAllocator, class OpAllocator, class ValuePool >
WaitFreeMPMCQueue<Type, NodeAllocator, OpAllocator, ValuePool>::OperationDesc::
OperationDesc(index_t raw) : Raw(raw) {
  Pending   = (raw & PENDING_FLAG_MASK) ? true : false;
  Enqueue   = (raw & ENQUEUE_FLAG_MASK) ? true : false;
  NodeIndex = (raw & NODE_INDEX_MASK);
}

template<
  typename Type, class NodeAllocator, class OpAllocator, class ValuePool >
WaitFreeMPMCQueue<Type, NodeAllocator, OpAllocator, ValuePool>::
WaitFreeMPMCQueue(size_t capacity)
: max_size_(capacity),
//  Disable "this is used in base member initializer" warning.
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4355)
#endif
  delete_pointer_callback(*this, &self_t::DeleteNodeCallback),
#ifdef _MSC_VER
#pragma warning(pop)
#endif
  hp(delete_pointer_callback, UndefinedGuard, 2),
  // Using int for numThreads so compiler warning is
  // raised when size and numThreads are switched by
  // mistake.
  num_states(embb::base::Thread::GetThreadsMaxCount()),
  // Node pool size, with respect to the maximum number of
  // retired nodes not eligible for reuse due to hazard pointers:
  node_pool_size(
    // numThreads caused trouble here
    (hp.GetRetiredListMaxSize() *
     embb::base::Thread::GetThreadsMaxCount()) +
    max_size_ + 1),
  nodePool(node_pool_size, nullNode) {
  // Assert width of binary representation of operation description
  assert(sizeof(index_t) == 4);
  if (max_size_ > QUEUE_SIZE_MAX) {
    EMBB_THROW(embb::base::NoMemoryException,
      "Maximum size of queue exceeded");
  }
  // Allocate sentinel node:
  Node_t sentinelNode;
  int sentinelNodePoolIndex = nodePool.Allocate(sentinelNode);
  if (sentinelNodePoolIndex < 0) {
    EMBB_THROW(embb::base::NoMemoryException,
      "Allocation of sentinel node failed");
  }
  // No need to guard sentinel node, it is prevented from reuse
  // in the hazard pointers' delete callback (see DeleteNodeCallback).
  sentinelNodeIndex = static_cast<index_t>(sentinelNodePoolIndex);
  headIdx.Store(sentinelNodeIndex);
  tailIdx.Store(sentinelNodeIndex);
  // State of the queue is one operation description per queue accessor.
  // Initialize clear state: Null-operarion for every accessor.
  operationDescriptions = operationDescriptionAllocator.allocate(num_states);
  for (size_t accessorId = 0; accessorId < num_states; ++accessorId) {
    OperationDesc op(
      false,                 // nonpending
      true,                  // enqueue, should not matter
      Node_t::UndefinedIndex // node index
    );
    // As all operation descriptions are the same,
    // we do not have to map accessor ids to operation
    // pool indices. Every accessor will randomly grab
    // an operation pool element and stick to it, as
    // a threads accessor id will not change.
    operationDescriptions[accessorId].Store(op.Raw);
  }
}

template<
  typename Type, class NodeAllocator, class OpAllocator, class ValuePool >
WaitFreeMPMCQueue<Type, NodeAllocator, OpAllocator, ValuePool>::
~WaitFreeMPMCQueue() {
  // Dequeue until queue is empty:
  Type val;
  // Delete internally managed memory regions:
  operationDescriptionAllocator.deallocate(
    operationDescriptions,
    num_states);
}

template<
  typename Type, class NodeAllocator, class OpAllocator, class ValuePool >
inline bool WaitFreeMPMCQueue<Type, NodeAllocator, OpAllocator, ValuePool>::
LoadAccessorThreadIndex(index_t & retIndexValue) {
  unsigned int tmpIndexValue; // For conversion size32_t <-> unsigned int
  if (embb_internal_thread_index(&tmpIndexValue) == EMBB_SUCCESS) {
    // Fail if thread index is not in range of number of accessors:
    if (tmpIndexValue < num_states) {
      retIndexValue = tmpIndexValue;
      return true;
    }
    return false;
  }
  retIndexValue = Node_t::UndefinedIndex;
  return false;
}

template<
  typename Type, class NodeAllocator, class OpAllocator, class ValuePool >
inline size_t WaitFreeMPMCQueue<Type, NodeAllocator, OpAllocator, ValuePool>::
RetiredListMaxSize(size_t nThreads) {
  return static_cast<size_t>(
    1.25 *
    static_cast<double>(nThreads) * static_cast<double>(num_guards)) + 1;
}

template<
  typename Type, class NodeAllocator, class OpAllocator, class ValuePool >
bool WaitFreeMPMCQueue<Type, NodeAllocator, OpAllocator, ValuePool>::
TryEnqueue(Type const & element) {
  index_t accessorId = Node_t::UndefinedIndex;
  if (!LoadAccessorThreadIndex(accessorId)) {
    EMBB_THROW(embb::base::ErrorException,
      "Invalid thread ID.");
  }
  // Register new node in pool:
  Node_t poolNode;
  int nodeIndex = nodePool.Allocate(poolNode);
  if (nodeIndex < 0) {
    return false; // Queue is at capacity
  }
  // Initialize node in pool:
  Node_t newNode(element, accessorId);
  nodePool[static_cast<index_t>(nodeIndex)] = newNode;
  OperationDesc enqOp(
    true,    // pending
    true,    // enqueue
    static_cast<index_t>(nodeIndex)
    );
  operationDescriptions[accessorId].Store(enqOp.Raw);
  Help();
  HelpFinishEnqueue();
  return true;
}

template<
  typename Type, class NodeAllocator, class OpAllocator, class ValuePool >
bool WaitFreeMPMCQueue<Type, NodeAllocator, OpAllocator, ValuePool>::
TryDequeue(Type & retElement) {
  index_t accessorId = static_cast<index_t>(-1);
  if (!LoadAccessorThreadIndex(accessorId)) {
    EMBB_THROW(embb::base::ErrorException,
      "Invalid thread ID. Verify embb::base::Thread::thread_max_count.");
  }
  OperationDesc curOp(operationDescriptions[accessorId].Load());
  // Assert that current operation of this accessor is completed:
  assert(!curOp.Pending);
  // Register new operation description for CAS:
  OperationDesc newOp(
    true,                  // pending
    false,                 // dequeue
    Node_t::UndefinedIndex // node index
    );
  index_t curOpRaw = curOp.Raw;
  if (!operationDescriptions[accessorId].CompareAndSwap(curOpRaw, newOp.Raw)) {
    // The threads own operation has changed,
    // should not happen.
    assert(false);
  }
  Help();
  HelpFinishDequeue();
  // Accessor's operation Idx might have changed in between.
  curOp = OperationDesc(operationDescriptions[accessorId].Load());
  // Operation announced by this thread must not be pending any more:
  assert(!curOp.Pending);
  // Check element
  index_t nodeIdx = curOp.NodeIndex;
  Node_t & node = nodePool[nodeIdx];
  if (nodeIdx == Node_t::UndefinedIndex) {
    // Allow dequeueing from empty queue, but
    // return false:
    retElement = Type();
    return false;
  }
  index_t deqNodeIdx = node.NextPoolIdx();
  retElement = nodePool[deqNodeIdx].Value();
  // Mark node as non-pending by setting node index to UndefinedIndex:
  OperationDesc noOp(
    false,                 // non-pending
    false,                 // any
    Node_t::UndefinedIndex // no node index
    );
  curOp = OperationDesc(operationDescriptions[accessorId].Load());
  curOpRaw = curOp.Raw;
  if (!operationDescriptions[accessorId].CompareAndSwap(curOpRaw, noOp.Raw)) {
    // The threads own operation has changed,
    // should not happen.
    assert(false);
  }
  hp.EnqueuePointerForDeletion(nodeIdx);
  return true;
}

template<
  typename Type, class NodeAllocator, class OpAllocator, class ValuePool >
void WaitFreeMPMCQueue<Type, NodeAllocator, OpAllocator, ValuePool>::
HelpEnqueue(unsigned int accessorId) {
  while (IsPending(accessorId)) {
    index_t lastIdx = tailIdx.Load();
    // Guard tail:
    hp.GuardPointer(0, lastIdx);
    // Last node still is tail:
    if (lastIdx == tailIdx.Load()) {
      Node_t & lastNode = nodePool[lastIdx];
      index_t nextIdx = lastNode.NextPoolIdx();
      // tail.next is null (no pending enqueue on tail):
      if (lastNode.NextPoolIdx() == Node_t::UndefinedIndex) {
        // Apply enqueue.
        // No other accessor helped this enqueue operation yet:
        if (IsPending(accessorId)) {
          // Set next-pointer of last element in list
          OperationDesc opDesc(operationDescriptions[accessorId].Load());
          if (lastNode.CASNext(nextIdx, opDesc.NodeIndex)) {
            HelpFinishEnqueue();
            return;
          }
        }
      } else {
        // Some enqueue operation in progress
        HelpFinishEnqueue();
      }
    }
  }
}

template<
  typename Type, class NodeAllocator, class OpAllocator, class ValuePool >
void WaitFreeMPMCQueue<Type, NodeAllocator, OpAllocator, ValuePool>::
HelpFinishEnqueue() {
  // Load node pointed at by tail:
  index_t lastIdx = tailIdx.Load();
  // Guard tail:
  hp.GuardPointer(0, lastIdx);

  Node_t & lastNode = nodePool[lastIdx];
  index_t nextIdx = lastNode.NextPoolIdx();
  // This node is NEXT of tail, but not tail => unfinished ENQ
  hp.GuardPointer(1, nextIdx);
  if (nextIdx != Node_t::UndefinedIndex) {
    Node_t & nextNode = nodePool[nextIdx];
    // Load accessor id from last (non-tail) element in list:
    index_t helpAID = nextNode.EnqueueAID();
    // Load operation for accessor that started the unfinished enqueue:
    OperationDesc helpOp(operationDescriptions[helpAID].Load());

    // tail index still points at last node:
    // (last == tail && state[aid].node == next)
    if (lastIdx == tailIdx.Load() &&
      helpOp.NodeIndex == nextIdx) {
      OperationDesc newOp(
        false,   // set to nonpending
        true,    // enqueue
        nextIdx // node index == helpOp.NodeIndex
        );
      index_t helpOpRaw = helpOp.Raw;
      operationDescriptions[helpAID].CompareAndSwap(
        helpOpRaw,
        newOp.Raw);
      // Update tail pointer:
      tailIdx.CompareAndSwap(lastIdx, nextIdx);
    }
  }
}

template<
  typename Type, class NodeAllocator, class OpAllocator, class ValuePool >
void WaitFreeMPMCQueue<Type, NodeAllocator, OpAllocator, ValuePool>::
HelpDequeue(index_t accessorId) {
  while (IsPending(accessorId)) {
    index_t firstIdx = headIdx.Load();
    // Guard head:
    hp.GuardPointer(0, firstIdx);
    if (firstIdx != headIdx.Load()) {
      // Head pointer changed by concurrent enqueue
      continue;
    }
    index_t lastIdx = tailIdx.Load();
    Node_t & first = nodePool[firstIdx];
    index_t nextIdx = first.NextPoolIdx();
    // Guard head->next:
    hp.GuardPointer(1, nextIdx);
    if (nextIdx != first.NextPoolIdx()) {
      // Head->next pointer changed by concurrent enqueue
      continue;
    }
    if (firstIdx == lastIdx) {
      // Queue might be empty
      if (nextIdx == Node_t::UndefinedIndex) {
        // Queue is empty
        OperationDesc curOp(operationDescriptions[accessorId].Load());
        if (lastIdx == tailIdx.Load() && IsPending(accessorId)) {
          OperationDesc newOp(
            false,                 // Set nonpending state
            false,
            Node_t::UndefinedIndex // Leave undefined, to signal failed dequeue
            );
          // CAS without check as possibly another accessor
          // already helped this dequeue operation.
          index_t curOpRaw = curOp.Raw;
          operationDescriptions[accessorId].CompareAndSwap(curOpRaw, newOp.Raw);
        }
      } else {
        // Head is advanced because of unfinished enqueue, so
        // help other enqueue and retry:
        HelpFinishEnqueue();
      }
    } else {
      // Queue is not empty
      OperationDesc curOp(operationDescriptions[accessorId].Load());
      index_t nodeIdx = curOp.NodeIndex;
      if (!IsPending(accessorId)) {
        // Accessor not pending because another thread completed this
        // operation already, done.
        break;
      }
      if (firstIdx == headIdx.Load() && nodeIdx != firstIdx) {
        OperationDesc newOp(
          true,
          false,
          firstIdx // Set node index
          );
        index_t curOpRaw = curOp.Raw;
        if (!operationDescriptions[accessorId].CompareAndSwap(
              curOpRaw, newOp.Raw)) {
          // This loop is wait-free as every accessor's operation
          // will be pending for a limited time only, so continue
          // and retry is okay.
          // This CAS can only have failed because another
          // thread completed this dequeue operation in the
          // meantime.
          continue;  // Retry
        }
      }
      // The following CAS also happens if
      //
      //   firstIdx != headIdx.Load() || nodeIdx == firstIdx
      //
      // In this case, HelpFinishDequeue will complete the dequeue.
      index_t curDeqAID = Node_t::UndefinedIndex;
      // Register this accessor as dequeuer of this node.
      // If this CAS fails, another accessor is already (perhaps
      // helping) dequeueing this node winning this CAS. In this
      // case this dequeue operation is just ignored.
      if (!first.DequeueAID().CompareAndSwap(curDeqAID, accessorId)) {
        continue;
        // Lost CAS to helping accessor. In this case, this operation
        // is not pending anymore at this point.
      }
      HelpFinishDequeue();
    }
  }  // while pending
}

template<
  typename Type, class NodeAllocator, class OpAllocator, class ValuePool >
void WaitFreeMPMCQueue<Type, NodeAllocator, OpAllocator, ValuePool>::
HelpFinishDequeue() {
  index_t firstIdx = headIdx.Load();
  // Guard head:
  hp.GuardPointer(0, firstIdx);
  Node_t & first = nodePool[firstIdx];
  index_t nextIdx = first.NextPoolIdx();
  // Guard and head->next:
  hp.GuardPointer(1, nextIdx);
  index_t accessorId = first.DequeueAID().Load();
  if (accessorId != Node_t::UndefinedIndex) {
    OperationDesc curOp(operationDescriptions[accessorId].Load());
    if (firstIdx == headIdx.Load() &&
        nextIdx != Node_t::UndefinedIndex) {
      // Set state of helped operation to NONPENDING:
      OperationDesc newOp(
        false, // nonpending
        false, // dequeue
        curOp.NodeIndex
        );
      // CAS without check as possibly another accessor
      // already helped this dequeue operation.
      index_t curOpRaw = curOp.Raw;
      operationDescriptions[accessorId].CompareAndSwap(curOpRaw, newOp.Raw);
      headIdx.CompareAndSwap(firstIdx, nextIdx);
    }
  }
}

template<
  typename Type, class NodeAllocator, class OpAllocator, class ValuePool >
void WaitFreeMPMCQueue<Type, NodeAllocator, OpAllocator, ValuePool>::
Help() {
  // Fairness guarantee in every thread: 
  // "Every other thread will help my operation before helping its
  // own operation"
  // Number of total operations in operation description buffer to be
  // helped before engaging in own announced operation
  index_t numHelpOps    = static_cast<index_t>(num_states);
  index_t ownAccessorId = Node_t::UndefinedIndex;
  LoadAccessorThreadIndex(ownAccessorId);
  // Start helping accessor with next id, ensuring that own accessor id
  // will be used in last iteration of help loop: 
  index_t startAccessorId = (ownAccessorId + 1) % num_states;
  for (unsigned int accessorId = startAccessorId;
       numHelpOps > 0;
       ++accessorId, --numHelpOps) {
    OperationDesc desc(
      operationDescriptions[accessorId % num_states].Load());
    if (desc.Pending) {
      if (desc.Enqueue) {
        HelpEnqueue(accessorId % num_states);
      } else {
        HelpDequeue(accessorId % num_states);
      }
    }
  }
}

template<
  typename Type, class NodeAllocator, class OpAllocator, class ValuePool >
void WaitFreeMPMCQueue<Type, NodeAllocator, OpAllocator, ValuePool>::
DeleteNodeCallback(index_t releasedNodeIndex) {
  if (sentinelNodeIndex == releasedNodeIndex) {
    // Prevent sentinel node from reuse, it will be returned to pool
    // in destructor only.
    return;
  }
  if (NodeIsPending(releasedNodeIndex)) {
    return;
  }
  nodePool.Free(static_cast<int>(releasedNodeIndex));
}

template<
  typename Type, class NodeAllocator, class OpAllocator, class ValuePool >
inline size_t WaitFreeMPMCQueue<Type, NodeAllocator, OpAllocator, ValuePool>::
GetCapacity() {
  return max_size_;
}

template<
  typename Type, class NodeAllocator, class OpAllocator, class ValuePool >
inline bool WaitFreeMPMCQueue<Type, NodeAllocator, OpAllocator, ValuePool>::
IsPending(unsigned int accessorId) {
  OperationDesc opDesc(operationDescriptions[accessorId].Load());
  return opDesc.Pending;
}

template<
  typename Type, class NodeAllocator, class OpAllocator, class ValuePool >
bool WaitFreeMPMCQueue<Type, NodeAllocator, OpAllocator, ValuePool>::
NodeIsPending(index_t nodeId) {
  // Returns true if given node index is involved in any announced operation,
  // including non-pending operations, as dequeue-operations are set to
  // non-pending before returning the node's value. Dequeue operations are
  // finalized by setting their node index to UndefinedIndex.
  for (unsigned int accessorId = 0; accessorId < num_states; ++accessorId) {
    OperationDesc desc(operationDescriptions[accessorId].Load());
    if (desc.NodeIndex == nodeId) {
      return true;
    }
  }
  return false;
}

} // namespace containers
} // namespace embb

#endif  // EMBB_CONTAINERS_INTERNAL_WAIT_FREE_MPMC_QUEUE_INL_H_
