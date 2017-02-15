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

#ifndef EMBB_CONTAINERS_LOCK_FREE_STACK_H_
#define EMBB_CONTAINERS_LOCK_FREE_STACK_H_

#include <embb/containers/object_pool.h>
#include <embb/base/atomic.h>
#include <embb/base/function.h>
#include <embb/containers/internal/hazard_pointer.h>
#include <embb/containers/lock_free_tree_value_pool.h>

/**
 * \defgroup CPP_CONCEPTS_STACK Stack Concept
 * Concept for thread-safe stacks
 *
 * \ingroup CPP_CONCEPT
 * \{
 * \par Description
 * A stack is an abstract data type holding a collection of elements of some
 * predetermined type. A stack provides two operations: \c TryPush and
 * \c TryPop. \c TryPush tries to add an element to the collection, and
 * \c TryPop tries to remove an element from the collection. A stack has LIFO
 * (Last-In, First-out) semantics, i.e., the last element added to the
 * collection (\c TryPush) is removed first (\c TryPop). The capacity \c cap of
 * a stack defines the number of elements it can store (depending on the
 * implementation, a stack might store more than \c cap elements, since for
 * thread-safe memory management, more memory than necessary for holding \c cap
 * elements has to be provided).
 *
 * \par Requirements
 * - Let \c Stack be the stack class
 * - Let \c Type be the element type of the stack
 * - Let \c capacity be a value of type \c size_t
 * - Let \c element be a reference to an element of type \c Type
 *
 * \par Valid Expressions
 * <table>
 *   <tr>
 *     <th>Expression</th>
 *     <th>Return type</th>
 *     <th>Description</th>
 *   </tr>
 *   <tr>
 *     <td>\code{.cpp} Stack<Type>(capacity) \endcode</td>
 *     <td>Nothing</td>
 *     <td>
 *      Constructs a stack with capacity \c capacity that holds elements of
 *      type \c Type.
 *     </td>
 *   </tr>
 *   <tr>
 *     <td>\code{.cpp} TryPush(element) \endcode</td>
 *     <td>\code{.cpp} bool \endcode</td>
 *     <td>
 *      Tries to push \c element onto the stack. Returns \c false if the stack
 *      is full, otherwise \c true.
 *     </td>
 *   </tr>
 *   <tr>
 *     <td>\code{.cpp} TryPop(element) \endcode</td>
 *     <td>\code{.cpp} bool \endcode</td>
 *     <td>
 *      Tries to pop an element from the stack. Returns \c false if the stack is
 *      empty, otherwise \c true. In the latter case, the popped element is 
 *      stored in \c element which must be passed by reference.
 *     </td>
 *   </tr>
 * </table>
 *
 * \}
 */

/**
 * \defgroup CPP_CONTAINERS_STACKS Stacks
 * Concurrent stacks
 *
 * \ingroup CPP_CONTAINERS
 *
 * \see CPP_CONCEPTS_STACK
 */

namespace embb {
namespace containers {
namespace internal {
/**
 * Stack node
 *
 * Single linked list, contains the element (\c element) and a pointer to the
 * next node (\c next).
 *
 * \tparam T Element type
 */
template< typename T >
class LockFreeStackNode {
 private:
  /**
   * Pointer to the next node
   */
  LockFreeStackNode< T >* next;

  /**
   * The stored element
   */
  T element;

 public:
  /**
   * Creates a stack node
   */
  LockFreeStackNode(
    T const& element
    /**< [IN] The element of this stack node */
  );

  /**
   * Sets the next node
   */
  void SetNext(
    LockFreeStackNode< T >* next
    /**< [IN] Pointer to the next node */
  );

  /**
   * Returns the next pointer
   *
   * \return The next pointer
   */
  LockFreeStackNode< T >* GetNext();

  /**
   * Returns the element held by this node
   */
  T GetElement();
};
} // namespace internal

/**
 * Lock-free stack
 *
 * \concept{CPP_CONCEPTS_STACK}
 *
 * \ingroup CPP_CONTAINERS_STACKS
 *
 * \tparam Type Type of the stack elements
 * \tparam ValuePool Type of the value pool used as basis for the ObjectPool
 *         which stores the elements.
 */
template< typename Type,
typename ValuePool = embb::containers::LockFreeTreeValuePool < bool, false > >
class LockFreeStack {
 private:
  /**
   * The capacity of the stack. It is guaranteed that the stack can hold at
   * least as many elements, maybe more.
   */
  size_t capacity;

  /**
   * Callback to the method that is called by hazard pointers if a pointer is
   * not hazardous anymore, i.e., can safely be reused.
   */
  embb::base::Function<void, internal::LockFreeStackNode<Type>*>
    delete_pointer_callback;

  /**
   * The callback function, used to cleanup non-hazardous pointers.
   * \see delete_pointer_callback
   */
  void DeletePointerCallback(internal::LockFreeStackNode<Type>* to_delete);

  /**
   * The object pool, used for lock-free memory allocation.
   *
   * Warning: the objectPool has to be initialized before the hazardPointer
   * object, to be sure that the hazardPointer object is destructed before the
   * Pool as the hazardPointer object might return elements to the pool in its
   * destructor. So the ordering of the members objectPool and hazardPointer is
   * important here!
   */
  ObjectPool< internal::LockFreeStackNode<Type>, ValuePool > objectPool;

  /**
   * Definition of the used hazard pointer type
   */
  typedef internal::HazardPointer < internal::LockFreeStackNode<Type>* >
    StackNodeHazardPointer_t;

  /**
   * The hazard pointer object, used for memory management.
   */
  StackNodeHazardPointer_t hazardPointer;

  /**
   * Atomic pointer to the top node of the stack (element that is popped next)
   */
  embb::base::Atomic<internal::LockFreeStackNode<Type>*> top;

 public:
  /**
   * Creates a stack with the specified capacity.
   *
   * \memory
   * Let \c t be the maximum number of threads and \c x be <tt>1.25*t+1</tt>.
   * Then, <tt>x*(3*t+1)</tt> elements of size <tt>sizeof(void*)</tt>, \c x
   * elements of size <tt>sizeof(Type)</tt>, and \c capacity elements of size
   * <tt>sizeof(Type)</tt> are allocated.
   *
   * \notthreadsafe
   *
   * \see CPP_CONCEPTS_STACK
   */
  LockFreeStack(
    size_t capacity
    /**< [IN] Capacity of the stack */
  );

  /**
   * Returns the capacity of the stack.
   *
   * \return Number of elements the stack can hold.
   *
   * \waitfree
   */
  size_t GetCapacity();

  /**
   * Destroys the stack.
   *
   * \notthreadsafe
   */
  ~LockFreeStack();

  /**
   * Tries to push an element onto the stack.
   *
   * \return \c true if the element could be pushed, \c false if the stack is
   * full.
   *
   * \lockfree
   *
   * \note It might be possible to push more elements onto the stack than its
   * capacity permits.
   *
   * \see CPP_CONCEPTS_STACK
   */
  bool TryPush(
    Type const& element
    /**< [IN] Const reference to the element that shall be pushed */
  );

  /**
   * Tries to pop an element from the stack.
   *
   * \return \c true if an element could be popped, \c false if the stack is
   * empty.
   *
   * \lockfree
   *
   * \see CPP_CONCEPTS_STACK
   */
  bool TryPop(
    Type & element
    /**< [IN,OUT] Reference to the popped element. Unchanged, if the operation
                  was not successful. */
  );
};

} // namespace containers
} // namespace embb

#include <embb/containers/internal/lock_free_stack-inl.h>

#endif  // EMBB_CONTAINERS_LOCK_FREE_STACK_H_
