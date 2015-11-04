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

#ifndef EMBB_CONTAINERS_LOCK_FREE_CHROMATIC_TREE_H_
#define EMBB_CONTAINERS_LOCK_FREE_CHROMATIC_TREE_H_

#include <stddef.h>
#include <functional>

#include <embb/base/c/errors.h>
#include <embb/base/mutex.h>
#include <embb/containers/internal/hazard_pointer.h>
#include <embb/containers/lock_free_tree_value_pool.h>
#include <embb/containers/object_pool.h>

namespace embb {
namespace containers {
namespace internal {

template<typename Key, typename Value>
class ChromaticTreeOperation;

/**
 * Chromatic tree node.
 * 
 * Stores the key-value pair, as well as the weight value (used for rebalancing)
 * and two pointers to child nodes (left and right).
 * 
 * \tparam Key   Key type
 * \tparam Value Value type
 */
template<typename Key, typename Value>
class ChromaticTreeNode {
 public:
  /** Node of the tree (self type). */
  typedef ChromaticTreeNode         Node;
  /** Atomic pointer to a node. */
  typedef embb::base::Atomic<Node*> AtomicNodePtr;
  /** Chromatic tree operation. */
  typedef ChromaticTreeOperation<Key, Value> Operation;
  /** Atomic pointer to a tree operation. */
  typedef embb::base::Atomic<Operation*>     AtomicOperationPtr;

  /**
   * Creates a node with given parameters.
   * 
   * \param[IN] key       Key of the new node
   * \param[IN] value     Value of the new node
   * \param[IN] weight    Weight of the new node
   * \param[IN] left      Pointer to the left child node
   * \param[IN] right     Pointer to the right child node
   * \param[IN] operation Pointer to an operation object
   */
  ChromaticTreeNode(const Key& key, const Value& value, int weight,
                    Node* left, Node* right, Operation* operation);
  
  /**
   * Creates a node with given parameters and no child nodes.
   * 
   * \param[IN] key       Key of the new node
   * \param[IN] value     Value of the new node
   * \param[IN] weight    Weight of the new node
   * \param[IN] operation Pointer to an operation object
   */
  ChromaticTreeNode(const Key& key, const Value& value, int weight,
                    Operation* operation);
  
  /**
   * Accessor for the stored key.
   * 
   * \return Stored key
   */
  const Key&   GetKey() const;
  
  /**
   * Accessor for the stored value.
   * 
   * \return Stored value
   */
  const Value& GetValue() const;
  
  /**
   * Accessor for the weight of the node.
   * 
   * \return Weight of the node
   */
  int GetWeight() const;
  
  /**
   * Accessor for the left child pointer.
   * 
   * \return Reference to the left child pointer
   */
  AtomicNodePtr& GetLeft();
  Node* GetLeft() const;
  
  /**
   * Accessor for the right child pointer.
   * 
   * \return Reference to the right child pointer
   */
  AtomicNodePtr& GetRight();
  Node* GetRight() const;

  /**
   * Checks if the node is a leaf.
   * 
   * @return \c true if node is a leaf, \c false otherwise
   */
  bool IsLeaf() const;

  /**
   * Checks if the node is a sentinel.
   *
   * @return \c true if node is a sentinel, \c false otherwise
   */
  bool IsSentinel() const;

  /**
   * Tries to replace one of the child pointers that compares equal to
   * \c old_child with the \c new_child using an atomic compare-and-swap
   * operation. If neither left nor right child pointer is pointing to
   * \c old_child, returns \c false.
   *
   * \param old_child[IN] Pointer to an old child node to compare against
   * \param new_child[IN] Pointer to the new child node
   *
   * \return \c true if one of the child pointers is now pointing to
   *         \c new_child, \c false otherwise
   */
  bool ReplaceChild(Node* old_child, Node* new_child);

  /**
   * Marks node for deletion from the tree
   */
  void Retire();

  /**
   * Checks whether the node is marked for deletion from the tree.
   *
   * \return \c true if node is retired, \c false otherwise
   */
  bool IsRetired() const;

  /**
   * Accessor for the operation pointer of the node.
   *
   * \return Reference to this node's operation pointer
   */
  AtomicOperationPtr& GetOperation();

 private:
  /** Atomic boolean flag. */
  typedef embb::base::Atomic<bool> AtomicFlag;

  /**
   * Disable copy construction and assignment.
   */
  ChromaticTreeNode(const ChromaticTreeNode&);
  ChromaticTreeNode& operator=(const ChromaticTreeNode&);
  
  const Key          key_;         /**< Stored key. */
  const Value        value_;       /**< Stored value. */
  const int          weight_;      /**< Weight of the node. */
  const bool         is_leaf_;     /**< True if node is a leaf. */
  const bool         is_sentinel_; /**< True if node is a sentinel. */
  AtomicNodePtr      left_;        /**< Pointer to left child node. */
  AtomicNodePtr      right_;       /**< Pointer to right child node. */
  AtomicFlag         retired_;     /**< Retired (marked for deletion) flag. */
  AtomicOperationPtr operation_;   /**< Pointer to a tree operation object. */
};

/**
 * Tree operation
 *
 * Describes a chromatic tree operation (insertion, deletion or rotation).
 * Contains pointers to the root node of the operation, nodes that are part of
 * the operation window, and the newly created node to become a new child of the
 * root node. For all existing nodes, the original operation pointers acquired
 * through a previous WeakLLXs are stored. Methods for initialization, helping
 * and status enquiries are provided.
 *
 * \tparam Key   Key type
 * \tparam Value Value type
 */
template<typename Key, typename Value>
class ChromaticTreeOperation {
 public:
  /** Node of the tree. */
  typedef ChromaticTreeNode<Key, Value>  Node;
  /** Atomic pointer to a node. */
  typedef embb::base::Atomic<Node*>      AtomicNodePtr;
  /** Hazard-protected pointer to a node. */
  typedef UniqueHazardPointer<Node>      HazardNodePtr;
  /** Chromatic tree operation (self type). */
  typedef ChromaticTreeOperation         Operation;
  /** Atomic pointer to a tree operation. */
  typedef embb::base::Atomic<Operation*> AtomicOperationPtr;
  /** Hazard-protected pointer to a tree operation. */
  typedef UniqueHazardPointer<Operation> HazardOperationPtr;

  /**
   * Creates an empty operation object with an "in progress" state.
   *
   * \param is_dummy Boolean flag for creation of dummy operation objects.
   *
   */
  ChromaticTreeOperation(bool is_dummy = false);

  /**
   * Set the root node of this operation together with its original operation
   * pointer.
   *
   * \param root           The root node of the operation
   * \param root_operation The original operation pointer of the root node
   */
  void SetRoot(Node* root, Operation* root_operation);

  /**
   * Sets the nodes of this operation window together with their original
   * operation pointers.
   *
   * \param node1      The node from the operation window
   * \param operation1 The original operation pointer of \c node1
   */
  void SetOldNodes(Node* node1, Operation* operation1);

  void SetOldNodes(Node* node1, Operation* operation1,
                   Node* node2, Operation* operation2);

  void SetOldNodes(Node* node1, Operation* operation1,
                   Node* node2, Operation* operation2,
                   Node* node3, Operation* operation3);

  void SetOldNodes(Node* node1, Operation* operation1,
                   Node* node2, Operation* operation2,
                   Node* node3, Operation* operation3,
                   Node* node4, Operation* operation4);

  void SetOldNodes(Node* node1, Operation* operation1,
                   Node* node2, Operation* operation2,
                   Node* node3, Operation* operation3,
                   Node* node4, Operation* operation4,
                   Node* node5, Operation* operation5);

  /**
   * Set the node that is to become the new child of the operation root.
   *
   * \param new_child Node to become the new child of the operation root
   */
  void SetNewChild(Node* new_child);

  /**
   * Help execute the operation. First tries to freeze all the nodes in the
   * operation window, and if succeeds - retires those nodes and injects the new
   * window into the tree. If the freezing step fails, rolls back the operation
   * by unfreezing all the nodes of the original window.
   *
   * \param node_guard Node hazard guard used to protect the helped nodes
   * \param oper_guard Operation hazard guard to protect the original operation
   *                   pointers of the helped nodes
   *
   * \return \c true is the operation successfully commits, \c false if it
   *         aborts
   */
  bool Help(AtomicNodePtr& node_guard, AtomicOperationPtr& oper_guard);

  /**
   * Help an operation that has successfully frozen all the nodes in its window
   * to complete. This operation may no longer fail or abort.
   *
   * \param node_guard Node hazard guard used to protect the helped nodes
   */
  void HelpCommit(AtomicNodePtr& node_guard);

  /**
   * Help an aborted operation by unfreezing the given \c node.
   *
   * \param node The node to be unfrozen
   */
  void HelpAbort(Node* node);

  /**
   * Check whether the operation is aborted.
   *
   * \return \c true if operation is aborted, \c false otherwise
   */
  bool IsAborted();

  /**
   * Check whether the operation is in progress.
   *
   * \return \c true if operation is in progress, \c false otherwise
   */
  bool IsInProgress();

  /**
   * Check whether the operation is committed.
   *
   * \return \c true if operation is committed, \c false otherwise
   */
  bool IsCommitted();

  /**
   * Performs the necessary post-processing steps for the operation, i.e. if the
   * operation commits, resets all the operation pointers of the nodes of the
   * removed operation window to point to the retired dummy-operation. Must be
   * called only once after the operation either commits or aborts.
   *
   * \param retired_dummy Dummy operation object used to reset operation
   *                      pointers in the retired nodes.
   */
  void CleanUp(Operation* retired_dummy);

#ifdef EMBB_DEBUG
  /**
   * Set the deleted flag for this operation. No other method of this operation
   * is allowed to be called after this method returns.
   */
  void SetDeleted();
#endif

 private:
  /** Enumeration of possible operation states. */
  typedef enum {
    STATE_ABORTED,
    STATE_ROLLBACK,
    STATE_FREEZING,
    STATE_ALL_FROZEN,
    STATE_COMMITTED
  } State;
  /** Atomic wrapper for the operation state. */
  typedef embb::base::Atomic<State> AtomicState;

  /** Maximal possible number of nodes in an operation window. */
  static const size_t MAX_NODES = 5;

  /**
   * Check whether the operation is rolling back.
   *
   * \return \c true if operation is rolling back, \c false otherwise
   */
  bool IsRollingBack();

  /**
   * Check whether the operation is freezing.
   *
   * \return \c true if operation is freezing, \c false otherwise
   */
  bool IsFreezing();

  /**
   * Check whether all the nodes from the operation window were successfully
   * frozen for this operation.
   *
   * \return \c true if all nodes of the window were frozen, \c false otherwise
   */
  bool IsAllFrozen();

  /**
   * Tries to freeze all the nodes from the operation window for this operation.
   * If the current operation encounters a conflict and fails, this method helps
   * to roll back the operation by unfreezing all the nodes.
   *
   * \param node_guard Node hazard guard used to protect the freezing nodes
   * \param oper_guard Operation hazard guard to protect the original operation
   *                   pointers of the freezing nodes
   *
   * \return \c true if all nodes of this operation window were successfully
   *         frozen, \c false otherwise
   */
  bool FreezeAll(AtomicNodePtr& node_guard, AtomicOperationPtr& oper_guard);

  /**
   * Tries to freeze the given \c node for the current operation by using a CAS
   * to set the node's operation pointer to point to this operation. If the CAS
   * fails and the node is not frozen for this operation, tries to switch to the
   * "rolling back" state.
   *
   * \param node      The node to be frozen
   * \param operation The original operation pointer of the \c node
   *
   * \return \c true if \c node was successfully frozen, \c false otherwise
   */
  bool Freeze(Node* node, Operation* operation);

  /**
   * Rolls back the current operation by unfreezing all the nodes of the
   * operation window, i.e. changing the operation pointers of those nodes to
   * their original values.
   *
   * \param node_guard Node hazard guard used to protect the nodes being
   *                   unfrozen
   */
  void UnfreezeAll(AtomicNodePtr& node_guard);

  /**
   * Unfreezes the given \c node by changing its operation pointer back to the
   * original value.
   *
   * \param node      The node to be unfrozen
   * \param operation The original operation pointer of the \c node
   */
  void Unfreeze(Node* node, Operation* operation);

  /**
   * Tries to switch from one operation state to another. If current state is
   * already equal to \c new_state, returns \c true. Otherwise, tries to switch
   * from \c old_state to \c new_state atomically using a CAS operation, and
   * returns \c true if this CAS succeeds.
   * 
   * \param old_state Currently expected state of the operation
   * \param new_state New desired state of the operation
   *
   * \return \c true is state was successfully changed to \c new_state, \c false
   *         otherwise
   */
  bool SwitchState(State old_state, State new_state);

  /**
   * Disable copy construction and assignment.
   */
  ChromaticTreeOperation(const ChromaticTreeOperation&);
  ChromaticTreeOperation& operator=(const ChromaticTreeOperation&);

  /** Current state of the operation. */
  AtomicState state_;
  /** Root node of the operation. */
  Node*       root_;
  /** Original operation pointer of the root node. */
  Operation*  root_operation_;
  /** Number of nodes in the operation window. */
  size_t      num_old_nodes_;
  /** Nodes of the operation window. */
  Node*       old_nodes_[MAX_NODES];
  /** Original operation pointers for the nodes of the window. */
  Operation*  old_operations_[MAX_NODES];
  /** Pointer to the new node to become the new child of the root. */
  Node*       new_child_;
#ifdef EMBB_DEBUG
  /** Debug flag for memory management control (is set when node is deleted). */
  embb::base::Atomic<bool> deleted_;
#endif
};

} // namespace internal

namespace test {
/**
 * Forward declaration of the test class.
 */
template<typename Tree>
class TreeTest;

} // namespace test

/**
 * Chromatic balanced binary search tree.
 * 
 * Implements a balanced BST with support for \c Get, \c Insert and \c Delete
 * operations.
 * 
 * \tparam Key       Key type
 * \tparam Value     Value type
 * \tparam Compare   Custom comparator type for the keys. An object of the
 *                   type \c Compare must must be a functor taking two
 *                   arguments \c rhs and \c lhs of type \c Key and
 *                   returning \c true if and only if <tt>(rhs < lhs)</tt> holds
 * \tparam ValuePool Type of the value pool to be used inside object pools for
 *                   tree nodes and operation objects
 */
template<typename Key,
         typename Value,
         typename Compare = ::std::less<Key>,
         typename ValuePool = LockFreeTreeValuePool<bool, false>
         >
class ChromaticTree {
 public:
  /**
   * Exposing the \c Key template parameter back to the user.
   */
  typedef Key   KeyType;
  /**
   * Exposing the \c Value template parameter back to the user.
   */
  typedef Value ValueType;

  /**
   * Creates a new tree with given capacity.
   * 
   * \memory Allocates <tt>(2 * capacity + 7)</tt> tree nodes each of size
   *         <tt>sizeof(internal::ChromaticTreeNode<Key, Value>)</tt>.
   * 
   * \notthreadsafe
   * 
   * \param[IN] capacity        Required capacity of the tree
   * \param[IN] undefined_key   Object of type \c Key to be used as a dummy key.
   *                            Defaults to <tt>Key()</tt>
   * \param[IN] undefined_value Object of type \c Value to be used as a dummy 
   *                            value. Defaults to <tt>Value()</tt>
   * \param[IN] compare         Custom comparator object for managed keys.
   *                            Defaults to <tt>Compare()</tt>
   * 
   * \note If any of \c undefined_key, \c undefined_value or \c compare is not
   *       provided, that will require the corresponding type (\c Key, \c Value
   *       or \c Compare) to support value-initialization.
   */
  explicit ChromaticTree(size_t capacity, Key undefined_key = Key(),
                Value undefined_value = Value(), Compare compare = Compare());
  
  /**
   * Destroys the tree.
   * 
   * \notthreadsafe
   */
  ~ChromaticTree();
  
  
  /**
   * Tries to find a value for the given key.
   * 
   * \param[IN]     key    Key to search for
   * \param[IN,OUT] value  Reference to the found value. Unchanged if the given
   *                       key is not stored in the tree
   * 
   * \return \c true if the given key was found in the tree, \c false otherwise
   */
  bool Get(const Key& key, Value& value);
  
  /**
   * Tries to insert a new key-value pair into the tree. If a value for the
   * given key is already stored in the tree, tries to replace the stored value
   * with the new one.
   * 
   * \param[IN] key    New key to be inserted
   * \param[IN] value  New value to be inserted
   * 
   * \return \c true if the given key-value pair was successfully inserted into
   *         the tree, \c false if the tree has reached its capacity
   */
  bool TryInsert(const Key& key, const Value& value);
  
  /**
   * Tries to insert a new key-value pair into the tree. If a value for the
   * given key is already stored in the tree, tries to replace the stored value
   * with the new one. Also returns the original value stored in the tree for
   * the given \c key, or the \c undefined_value if the key was not present in
   * the tree.
   * 
   * \param[IN]     key       New key to be inserted
   * \param[IN]     value     New value to be inserted
   * \param[IN,OUT] old_value Reference to the value previously stored in the
   *                          tree for the given key
   * 
   * \return \c true if the given key-value pair was successfully inserted into
   *         the tree, \c false if the tree has reached its capacity
   */
  bool TryInsert(const Key& key, const Value& value, Value& old_value);
  
  /**
   * Tries to remove a given key-value pair from the tree.
   * 
   * \param[IN] key    Key to be removed
   * 
   * \return \c true if the given key-value pair was successfully deleted from
   *         the tree, \c false if there is not enough memory
   */
  bool TryDelete(const Key& key);
  
  /**
   * Tries to remove a given key-value pair from the tree, and returns the value
   * that was stored in the tree for the given key (or \c undefined_value if 
   * the key was not present in the tree).
   * 
   * \param[IN]     key       Key to be removed
   * \param[IN,OUT] old_value Reference to the value previously stored in the
   *                          tree for the given key
   * 
   * \return \c true if the given key-value pair was successfully deleted from
   *         the tree, \c false if there is not enough memory
   */
  bool TryDelete(const Key& key, Value& old_value);
  
  
  /**
   * Accessor for the capacity of the tree.
   * 
   * \return Number of key-value pairs the tree can store
   */
  size_t GetCapacity() const;
  
  /**
   * Accessor for the dummy value used by the tree
   * 
   * \return Object of type \c Value that is used by the tree as a dummy value
   */
  const Value& GetUndefinedValue() const;
  
  /**
   * Checks whether the tree is currently empty.
   * 
   * \return \c true if the tree stores no key-value pairs, \c false otherwise
   */
  bool IsEmpty() const;

 private:
  /** Node of the tree. */
  typedef internal::ChromaticTreeNode<Key, Value>   Node;
  /** Atomic pointer to a node. */
  typedef embb::base::Atomic<Node*>                 AtomicNodePtr;
  /** Hazard-protected pointer to a node. */
  typedef internal::UniqueHazardPointer<Node>       HazardNodePtr;
  /** Chromatic tree operation. */
  typedef internal::ChromaticTreeOperation<Key, Value> Operation;
  /** Atomic pointer to a tree operation. */
  typedef embb::base::Atomic<Operation*>               AtomicOperationPtr;
  /** Hazard-protected pointer to a tree operation. */
  typedef internal::UniqueHazardPointer<Operation>     HazardOperationPtr;
  /** Object pool for tree nodes. */
  typedef ObjectPool<Node, ValuePool>               NodePool;
  /** Object pool for tree operations. */
  typedef ObjectPool<Operation, ValuePool>          OperationPool;

  /** Enumeration of used hazard pointer indexes. */
  typedef enum {
    // Node/operation used for helping
    HIDX_HELPING = 0,
    // Common shared nodes/operations
    HIDX_GRANDGRANDPARENT,
    HIDX_GRANDPARENT,
    HIDX_PARENT,
    HIDX_LEAF,
    HIDX_SIBLING = HIDX_GRANDGRANDPARENT, // Never occur in the same scope
    // Rebalancing nodes/operations
    HIDX_U    = HIDX_GRANDGRANDPARENT, // Renamed when passed to "Rebalance"
    HIDX_UX   = HIDX_GRANDPARENT,      // Renamed when passed to "Rebalance"
    HIDX_UXX  = HIDX_PARENT,           // Renamed when passed to "Rebalance"
    HIDX_UXXX = HIDX_LEAF,             // Renamed when passed to "Rebalance"
    HIDX_UXL = HIDX_LEAF + 1, // Restoring sequence
    HIDX_UXR,
    HIDX_UXXL,
    HIDX_UXXR,
    // Left overweight
    HIDX_UXXRL  = HIDX_U,   // Reusing hazard guard that is no longer used
    HIDX_UXXRR  = HIDX_UXR, // Reusing hazard guard that is no longer used
    HIDX_UXXRLR = HIDX_UXR, // Reusing hazard guard that is no longer used
    HIDX_UXXRLL = HIDX_UXL, // Reusing hazard guard that is no longer used
    // Right overweight
    HIDX_UXXLR  = HIDX_UXXRL,  // Symmetric rotation
    HIDX_UXXLL  = HIDX_UXXRR,  // Symmetric rotation
    HIDX_UXXLRL = HIDX_UXXRLR, // Symmetric rotation
    HIDX_UXXLRR = HIDX_UXXRLL, // Symmetric rotation
    // Current operation object
    HIDX_CURRENT_OP = HIDX_UXXR + 1, // Restoring sequence
    HIDX_MAX
  } HazardIndex;

  /**
   * Follows a path from the root of the tree to some leaf searching for the 
   * given key (the leaf found by this method may or may not contain the given
   * key). Returns the reached leaf together with its ancestors.
   * 
   * \param[IN]     key         Key to be searched for
   * \param[IN,OUT] leaf        Reference to the reached leaf
   * \param[IN,OUT] parent      Reference to the parent of the reached leaf
   * \param[IN,OUT] grandparent Reference to the grandparent of the reached leaf
   */
  void Search(const Key& key, HazardNodePtr& leaf, HazardNodePtr& parent,
              HazardNodePtr& grandparent);
  
  /**
   * Checks whether the given node has a specified child node.
   * 
   * \param[IN] parent Parent node
   * \param[IN] child  Node that is supposed to be a child of \c parent
   * 
   * \return \c true if \c child is a child node of \c parent, \c false
   *         otherwise
   */
  bool HasChild(const Node* parent, const Node* child) const;

  /**
   * Destroys all the nodes of a subtree rooted at the given node, including the
   * node itself.
   * 
   * \notthreadsafe
   * 
   * \param node Root of the subtree to be destroyed
   */
  void Destruct(Node* node);

  /**
   * Computes the hight of the subtree rooted at the given node.
   *
   * \notthreadsafe
   *
   * \param[IN] node Root of the subtree for which the height is requested
   *
   * \return The height of a subtree rooted at node \c node. (The height of a
   *         leaf node is defined to be zero).
   */
  int GetHeight(const Node* node) const;

  /**
   * Check whether the tree is currently in a balanced state (if it is a valid
   * red-black tree).
   *
   * \return \c true if the tree is balanced, \c false otherwise
   */
  bool IsBalanced() const;

  /**
   * Check whether a subtree rooted at the given node is balanced.
   *
   * \param[IN] node Root of the subtree for which the balance is checked
   *
   * \return \c true if the tree is balanced, \c false otherwise
   */
  bool IsBalanced(const Node* node) const;

  /**
   * Checks whether a given operation is a dummy operation.
   *
   * \param[IN] operation Operation to be checked
   *
   * \return \c true if the given operation is a dummy, \c false otherwise
   */
  bool IsDummyOperation(const Operation* operation) const;

  /**
   * Retire a hazardous node using the node hazard manager.
   *
   * \param node A hazardous node to be retired
   */
  void RetireNode(HazardNodePtr& node);

  /**
   * Retire a hazardous operation object using the operation hazard manager.
   *
   * \param operation A hazardous operation to be retired
   */
  void RetireOperation(HazardOperationPtr& operation);

  /**
   * Get a node hazard guard with the specified \c index from the node hazard
   * manager.
   *
   * \param index Index of requested guard
   *
   * \return Hazard guard with the specified index
   */
  AtomicNodePtr& GetNodeGuard(HazardIndex index);

  /**
   * Get an operation hazard guard with the specified \c index from the
   * operation hazard manager.
   *
   * \param index Index of requested guard
   *
   * \return Hazard guard with the specified index
   */
  AtomicOperationPtr& GetOperationGuard(HazardIndex index);

  /**
   * Performs a WeakLLX operation according to the Tree Update template, i.e.
   * reads the operation pointer of the given \c node and check whether this
   * operation is completed or in progress. If it is completed and the \c node
   * is not retired, returns \c true and sets the \c operation variable to point
   * to the read operation. If the read operation is in progress, helps it and 
   * returns \c false.
   *
   * \param node      The node
   * \param operation Reference to the current operation pointer value of the
   *                  \c node to be used in the following SCX
   *
   * \return \c true if the \c node is not reserved by another operation and the
   *         \c operation was successfully read, \c false if the \c node is busy
   */
  bool WeakLLX(HazardNodePtr& node, HazardOperationPtr& operation);

  /**
   * Free a tree node by returning it to the node pool.
   *
   * \param[IN] node A node to be freed.
   */
  void FreeNode(Node* node);

  /**
   * Free a tree operation by returning it to the operation pool.
   *
   * \param[IN] operation An operation to be freed.
   */
  void FreeOperation(Operation* operation);

  /**
   * Follows the path from the root to some leaf (directed by the given key) and
   * checks for any tree balancing violations. If a violation is found, tries
   * to fix it by using a set of rebalancing rotations.
   * 
   * \param[IN] key Key to be searched for
   * 
   * \return \c true if the tree was successfully rebalanced, \c false otherwise
   */
  bool CleanUp(const Key& key);
  
  /**
   * Next block of methods is used internally to keep the balance of the tree.
   */
  embb_errors_t Rebalance(HazardNodePtr& u, HazardNodePtr& ux,
                          HazardNodePtr& uxx, HazardNodePtr& uxxx);

  embb_errors_t OverweightLeft(HazardNodePtr& u, HazardOperationPtr& u_op,
                               HazardNodePtr& ux, HazardOperationPtr& ux_op,
                               HazardNodePtr& uxx, HazardOperationPtr& uxx_op,
                               HazardNodePtr& uxl, HazardNodePtr& uxr,
                               HazardNodePtr& uxxl, HazardOperationPtr& uxxl_op,
                               HazardNodePtr& uxxr, bool uxx_is_left);

  embb_errors_t OverweightRight(HazardNodePtr& u, HazardOperationPtr& u_op,
                                HazardNodePtr& ux, HazardOperationPtr& ux_op,
                                HazardNodePtr& uxx, HazardOperationPtr& uxx_op,
                                HazardNodePtr& uxl, HazardNodePtr& uxr,
                                HazardNodePtr& uxxl, HazardNodePtr& uxxr,
                                HazardOperationPtr& uxxr_op, bool uxx_is_right);

  // The following included header contains the class methods implementing
  // tree rotations. It is generated automatically and must be included
  // directly inside the class definition.
# include <embb/containers/internal/lock_free_chromatic_tree-rebalance.h>

  /** Hazard pointer manager for protecting node pointers. */
  internal::HazardPointer<Node*> node_hazard_manager_;
  /** Hazard pointer manager for protecting operation pointers. */
  internal::HazardPointer<Operation*> operation_hazard_manager_;

  /** A dummy key used by the tree. */
  const Key     undefined_key_;   
  /** A dummy value used by the tree. */
  const Value   undefined_value_; 
  /** Comparator object for the keys. */
  const Compare compare_;         
  /** User-requested capacity of the tree. */
  size_t        capacity_;        
  /** Pool of tree nodes. */
  NodePool      node_pool_;       
  /** Pool of operation objects. */
  OperationPool operation_pool_;  
  /** Dummy operation used in newly created nodes. */
  Operation     initial_operation_dummy_;
  /** Dummy operation used in retired nodes. */
  Operation     retired_operation_dummy_;
  /** Pointer to the sentinel node used as the entry point into the tree. */
  Node* const   entry_;

  /**
   * Friending the test class for white-box testing
   */
  friend class test::TreeTest<ChromaticTree>;
};

} // namespace containers
} // namespace embb

#include <embb/containers/internal/lock_free_chromatic_tree-inl.h>

#endif // EMBB_CONTAINERS_LOCK_FREE_CHROMATIC_TREE_H_
