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

#ifndef EMBB_CONTAINERS_INTERNAL_LOCK_FREE_CHROMATIC_TREE_INL_H_
#define EMBB_CONTAINERS_INTERNAL_LOCK_FREE_CHROMATIC_TREE_INL_H_

#include <assert.h>
#include <algorithm>

#ifdef EMBB_DEBUG
static const size_t INVALID_POINTER = static_cast<size_t>(-1);
# define VERIFY_ADDRESS(addr) assert(reinterpret_cast<size_t>((addr)) != \
                                     INVALID_POINTER)
#else
# define VERIFY_ADDRESS(address) ((void)0)
#endif

namespace embb {
namespace containers {
namespace internal {

template<typename Key, typename Value>
ChromaticTreeNode<Key, Value>::
ChromaticTreeNode(const Key& key, const Value& value, int weight,
                  Node* left, Node* right, Operation* operation)
    : key_(key),
      value_(value),
      weight_(weight),
      left_(left),
      right_(right),
      retired_(false),
      operation_(operation) {}

template<typename Key, typename Value>
ChromaticTreeNode<Key, Value>::
ChromaticTreeNode(const Key& key, const Value& value, int weight,
                  Operation* operation)
    : key_(key),
      value_(value),
      weight_(weight),
      left_(NULL),
      right_(NULL),
      retired_(false),
      operation_(operation) {}

template<typename Key, typename Value>
const Key& ChromaticTreeNode<Key, Value>::GetKey() const {
  return key_;
}

template<typename Key, typename Value>
const Value& ChromaticTreeNode<Key, Value>::GetValue() const {
  return value_;
}

template<typename Key, typename Value>
int ChromaticTreeNode<Key, Value>::GetWeight() const {
  return weight_;
}

template<typename Key, typename Value>
typename ChromaticTreeNode<Key, Value>::AtomicNodePtr&
ChromaticTreeNode<Key, Value>::GetLeft() {
  return left_;
}

template<typename Key, typename Value>
typename ChromaticTreeNode<Key, Value>::Node*
ChromaticTreeNode<Key, Value>::GetLeft() const {
  return left_.Load();
}

template<typename Key, typename Value>
typename ChromaticTreeNode<Key, Value>::AtomicNodePtr&
ChromaticTreeNode<Key, Value>::GetRight() {
  return right_;
}

template<typename Key, typename Value>
typename ChromaticTreeNode<Key, Value>::Node*
ChromaticTreeNode<Key, Value>::GetRight() const {
  return right_.Load();
}

template<typename Key, typename Value>
bool ChromaticTreeNode<Key, Value>::
ReplaceChild(Node* old_child, Node* new_child) {
  bool replaced = false;

  if (left_ == old_child) {
    replaced = left_.CompareAndSwap(old_child, new_child);
  } else if (right_ == old_child) {
    replaced = right_.CompareAndSwap(old_child, new_child);
  }

  return replaced;
}

template<typename Key, typename Value>
void ChromaticTreeNode<Key, Value>::Retire() {
  retired_ = true;
}

template<typename Key, typename Value>
bool ChromaticTreeNode<Key, Value>::IsRetired() const {
  return retired_;
}

} // namespace internal


template<typename Key, typename Value, typename Compare, typename ValuePool>
ChromaticTree<Key, Value, Compare, ValuePool>::
ChromaticTree(size_t capacity, Key undefined_key, Value undefined_value,
              Compare compare)
#ifdef EMBB_PLATFORM_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable:4355)
#endif
    : node_hazard_manager_(
          embb::base::Function<void, Node*>(*this, &ChromaticTree::FreeNode),
          NULL, HIDX_MAX),
      operation_hazard_manager_(
          embb::base::Function<void, Operation*>(*this,
                                                 &ChromaticTree::FreeOperation),
          NULL, HIDX_MAX),
#ifdef EMBB_PLATFORM_COMPILER_MSVC
#pragma warning(pop)
#endif
      undefined_key_(undefined_key),
      undefined_value_(undefined_value),
      compare_(compare),
      capacity_(capacity),
      node_pool_(2 + 5 + 2 * capacity_ +
                 node_hazard_manager_.GetRetiredListMaxSize() *
                 embb::base::Thread::GetThreadsMaxCount()),
      operation_pool_(2 + 5 + 2 * capacity_ +
                      operation_hazard_manager_.GetRetiredListMaxSize() *
                      embb::base::Thread::GetThreadsMaxCount()),
      entry_(node_pool_.Allocate(undefined_key_, undefined_value_, 1,
                                 node_pool_.Allocate(undefined_key_,
                                                     undefined_value_,
                                                     1,
                                                     Operation::INITIAL_DUMMY),
                                 static_cast<Node*>(NULL),
                                 Operation::INITIAL_DUMMY)) {
  assert(entry_ != NULL);
  assert(entry_->GetLeft() != NULL);
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
ChromaticTree<Key, Value, Compare, ValuePool>::
~ChromaticTree() {
  Destruct(entry_->GetLeft());
  FreeNode(entry_);
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
bool ChromaticTree<Key, Value, Compare, ValuePool>::
Get(const Key& key, Value& value) {
  HazardNodePtr grandparent(GetNodeGuard(HIDX_GRANDPARENT));
  HazardNodePtr parent(GetNodeGuard(HIDX_PARENT));
  HazardNodePtr leaf(GetNodeGuard(HIDX_LEAF));
  Search(key, leaf, parent, grandparent);

  bool keys_are_equal = !IsSentinel(leaf) &&
                        !(compare_(key, leaf->GetKey()) ||
                          compare_(leaf->GetKey(), key));

  if (keys_are_equal) {
    value = leaf->GetValue();
  }

  return keys_are_equal;
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
bool ChromaticTree<Key, Value, Compare, ValuePool>::
TryInsert(const Key& key, const Value& value) {
  Value old_value;
  return TryInsert(key, value, old_value);
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
bool ChromaticTree<Key, Value, Compare, ValuePool>::
TryInsert(const Key& key, const Value& value, Value& old_value) {
  Node* new_leaf = NULL;
  Node* new_sibling = NULL;
  Node* new_parent = NULL;
  bool insertion_succeeded = false;
  bool added_violation = false;

  while (!insertion_succeeded) {
    HazardNodePtr grandparent(GetNodeGuard(HIDX_GRANDPARENT));
    HazardNodePtr parent(GetNodeGuard(HIDX_PARENT));
    HazardNodePtr leaf(GetNodeGuard(HIDX_LEAF));
    Search(key, leaf, parent, grandparent);

    // Protect the parent
    HazardOperationPtr parent_op(GetOperationGuard(HIDX_PARENT));
    if (!WeakLLX(parent, parent_op)) continue;
    // Verify that the leaf is still the parent's child
    if (!HasChild(parent, leaf)) continue;

    // Protect the leaf
    HazardOperationPtr leaf_op(GetOperationGuard(HIDX_LEAF));
    if (!WeakLLX(leaf, leaf_op)) continue;

    bool keys_are_equal = !IsSentinel(leaf) &&
                          !(compare_(key, leaf->GetKey()) ||
                            compare_(leaf->GetKey(), key));
    if (keys_are_equal) {
      // Reached leaf has a matching key: replace it with a new copy
      old_value = leaf->GetValue();
      new_parent = node_pool_.Allocate(key, value, leaf->GetWeight(),
                                       Operation::INITIAL_DUMMY);
      if (new_parent == NULL) break;

    // Reached leaf has a different key: add a new leaf
    } else {
      old_value = undefined_value_;

      new_leaf = node_pool_.Allocate(key, value, 1,
                                     Operation::INITIAL_DUMMY);
      if (new_leaf == NULL) break;
      new_sibling = node_pool_.Allocate(leaf->GetKey(), leaf->GetValue(), 1,
                                        Operation::INITIAL_DUMMY);
      if (new_sibling == NULL) break;

      int new_weight = (IsSentinel(parent)) ? 1 : (leaf->GetWeight() - 1);
      if (IsSentinel(leaf) || compare_(key, leaf->GetKey())) {
        new_parent = node_pool_.Allocate(leaf->GetKey(), undefined_value_,
                                         new_weight, new_leaf, new_sibling,
                                         Operation::INITIAL_DUMMY);
      } else {
        new_parent = node_pool_.Allocate(key, undefined_value_,
                                         new_weight, new_sibling, new_leaf,
                                         Operation::INITIAL_DUMMY);
      }
      if (new_parent == NULL) break;
    }

    // Create and fill the operation object
    HazardOperationPtr insert_op(GetOperationGuard(HIDX_CURRENT_OP));
    insert_op.ProtectSafe(operation_pool_.Allocate());
    if (insert_op == NULL) break;
    insert_op->SetRoot(parent, parent_op);
    insert_op->SetOldNodes(leaf, leaf_op);
    insert_op->SetNewChild(new_parent);

    // Execute operation
    insertion_succeeded = insert_op->Help(GetNodeGuard(HIDX_HELPING),
                                          GetOperationGuard(HIDX_HELPING));
    insert_op->CleanUp();

    // If operation failed
    if (!insertion_succeeded) {
      // Retire failed operation
      RetireOperation(insert_op);
      // Delete new nodes
      FreeNode(new_parent); new_parent = NULL;
      if (!keys_are_equal) {
        FreeNode(new_leaf); new_leaf = NULL;
        FreeNode(new_sibling); new_sibling = NULL;
      }
      // Restart from scratch
      continue;
    }

    // Retire old operation objects
    RetireOperation(parent_op);
    RetireOperation(leaf_op);
    // Retire old nodes
    RetireNode(leaf);

    added_violation = (parent->GetWeight() == 0 &&
                       new_parent->GetWeight() == 0);
  }

  if (insertion_succeeded) {
    if (added_violation) CleanUp(key);
  } else {
    if (new_leaf != NULL)    FreeNode(new_leaf);
    if (new_sibling != NULL) FreeNode(new_sibling);
    if (new_parent != NULL)  FreeNode(new_parent);
  }

  return insertion_succeeded;
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
bool ChromaticTree<Key, Value, Compare, ValuePool>::
TryDelete(const Key& key) {
  Value old_value;
  return TryDelete(key, old_value);
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
bool ChromaticTree<Key, Value, Compare, ValuePool>::
TryDelete(const Key& key, Value& old_value) {
  Node* new_leaf = NULL;
  bool deletion_succeeded = false;
  bool added_violation = false;

  while (!deletion_succeeded) {
    HazardNodePtr grandparent(GetNodeGuard(HIDX_GRANDPARENT));
    HazardNodePtr parent(GetNodeGuard(HIDX_PARENT));
    HazardNodePtr leaf(GetNodeGuard(HIDX_LEAF));
    Search(key, leaf, parent, grandparent);

    // Reached leaf has a different key - nothing to delete
    if (IsSentinel(leaf) || (compare_(key, leaf->GetKey()) ||
                             compare_(leaf->GetKey(), key))) {
      old_value = undefined_value_;
      deletion_succeeded = true;
      break;
    }

    // Protect the grandparent
    HazardOperationPtr grandparent_op(GetOperationGuard(HIDX_GRANDPARENT));
    if (!WeakLLX(grandparent, grandparent_op)) continue;
    // Verify that the parent is still the child of grandparent
    if (!HasChild(grandparent, parent)) continue;

    // Protect the parent
    HazardOperationPtr parent_op(GetOperationGuard(HIDX_PARENT));
    if (!WeakLLX(parent, parent_op)) continue;

    // Get the sibling (and protect it with hazard pointer)
    HazardNodePtr sibling(GetNodeGuard(HIDX_SIBLING));
    sibling.ProtectHazard((parent->GetLeft() == leaf) ?
                          parent->GetRight() : parent->GetLeft());
    if (parent->IsRetired() || !sibling.IsActive()) continue;
    VERIFY_ADDRESS(static_cast<Node*>(sibling));

    // Verify that the leaf is still the parent's child
    if (!HasChild(parent, leaf)) continue;

    // Protect the sibling
    HazardOperationPtr sibling_op(GetOperationGuard(HIDX_SIBLING));
    if (!WeakLLX(sibling, sibling_op)) continue;

    // Protect the leaf
    HazardOperationPtr leaf_op(GetOperationGuard(HIDX_LEAF));
    if (!WeakLLX(leaf, leaf_op)) continue;

    int new_weight = (IsSentinel(grandparent)) ?
                  1 : (parent->GetWeight() + sibling->GetWeight());

    new_leaf = node_pool_.Allocate(
        sibling->GetKey(), sibling->GetValue(), new_weight,
        sibling->GetLeft(), sibling->GetRight(), Operation::INITIAL_DUMMY);
    if (new_leaf == NULL) break;

    old_value = leaf->GetValue();

    // Create and fill the operation object
    HazardOperationPtr delete_op(GetOperationGuard(HIDX_CURRENT_OP));
    delete_op.ProtectSafe(operation_pool_.Allocate());
    if (delete_op == NULL) break;
    delete_op->SetRoot(grandparent, grandparent_op);
    delete_op->SetOldNodes(parent, parent_op, leaf, leaf_op, sibling, sibling_op);
    delete_op->SetNewChild(new_leaf);

    // Execute operation
    deletion_succeeded = delete_op->Help(GetNodeGuard(HIDX_HELPING),
                                         GetOperationGuard(HIDX_HELPING));
    delete_op->CleanUp();

    // If operation failed
    if (!deletion_succeeded) {
      // Retire failed operation
      RetireOperation(delete_op);
      // Delete new nodes
      FreeNode(new_leaf);
      // Restart from scratch
      continue;
    }

    // Retire old operation objects
    RetireOperation(grandparent_op);
    RetireOperation(parent_op);
    RetireOperation(leaf_op);
    RetireOperation(sibling_op);
    // Retire old nodes
    RetireNode(parent);
    RetireNode(leaf);
    RetireNode(sibling);

    added_violation =  (new_weight > 1);
  }

  if (deletion_succeeded) {
    if (added_violation) CleanUp(key);
  } else {
    if (new_leaf != NULL)    FreeNode(new_leaf);
  }

  return deletion_succeeded;
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
size_t ChromaticTree<Key, Value, Compare, ValuePool>::
GetCapacity() const {
  return capacity_;
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
const Value& ChromaticTree<Key, Value, Compare, ValuePool>::
GetUndefinedValue() const {
  return undefined_value_;
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
bool ChromaticTree<Key, Value, Compare, ValuePool>::
IsEmpty() const {
  return IsLeaf(entry_->GetLeft());
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
void ChromaticTree<Key, Value, Compare, ValuePool>::
Search(const Key& key, HazardNodePtr& leaf, HazardNodePtr& parent,
       HazardNodePtr& grandparent) {
  bool reached_leaf = false;

  while (!reached_leaf) {
    grandparent.ProtectSafe(entry_);
    parent.ProtectSafe(entry_);
    leaf.ProtectSafe(entry_);

    reached_leaf = IsLeaf(leaf);
    while (!reached_leaf) {
      grandparent.AdoptHazard(parent);
      parent.AdoptHazard(leaf);

      AtomicNodePtr& next_leaf =
          (IsSentinel(leaf) || compare_(key, leaf->GetKey())) ?
            leaf->GetLeft() : leaf->GetRight();

      // Parent is protected, so we can tolerate a changing child pointer
      while(!leaf.ProtectHazard(next_leaf));

      // Parent is retired - make sure it is actually removed from the tree
      if (parent->IsRetired()) {
        HazardOperationPtr op(GetOperationGuard(HIDX_HELPING));
        if (op.ProtectHazard(parent->GetOperation())) {
          op->HelpCommit(GetNodeGuard(HIDX_HELPING));
        }
        // Can't follow a child pointer in a retired node - restart from root
        break;
      }

      VERIFY_ADDRESS(static_cast<Node*>(leaf));

      reached_leaf = IsLeaf(leaf);
    }
  }
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
bool ChromaticTree<Key, Value, Compare, ValuePool>::
IsLeaf(const Node* node) const {
  return node->GetLeft() == NULL;
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
bool ChromaticTree<Key, Value, Compare, ValuePool>::
IsSentinel(const Node* node) const {
  return (node == entry_) || (node == entry_->GetLeft());
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
bool ChromaticTree<Key, Value, Compare, ValuePool>::
HasChild(const Node* parent, const Node* child) const {
  return (parent->GetLeft() == child || parent->GetRight() == child);
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
void ChromaticTree<Key, Value, Compare, ValuePool>::
Destruct(Node* node) {
  if (!IsLeaf(node)) {
    Destruct(node->GetLeft());
    Destruct(node->GetRight());
  }
  FreeNode(node);
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
int ChromaticTree<Key, Value, Compare, ValuePool>::
GetHeight(const Node* node) const {
  int height = 0;
  if (node != NULL) {
    height = 1 + ::std::max(GetHeight(node->GetLeft()),
                            GetHeight(node->GetRight()));
  }
  return height;
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
bool ChromaticTree<Key, Value, Compare, ValuePool>::
IsBalanced() const {
  return IsBalanced(entry_->GetLeft());
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
bool ChromaticTree<Key, Value, Compare, ValuePool>::
IsBalanced(const Node* node) const {
  // Overweight violation
  bool has_violation = node->GetWeight() > 1;

  if (!has_violation && !IsLeaf(node)) {
    const Node* left  = node->GetLeft();
    const Node* right = node->GetRight();

    // Red-red violation
    has_violation = node->GetWeight() == 0 &&
                    (left->GetWeight() == 0 || right->GetWeight() == 0);

    // Check children
    if (!has_violation) {
      has_violation = !IsBalanced(left) || !IsBalanced(right);
    }
  }

  return !has_violation;
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
void ChromaticTree<Key, Value, Compare, ValuePool>::
RetireNode(HazardNodePtr& node) {
  node_hazard_manager_.EnqueuePointerForDeletion(node.ReleaseHazard());
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
void ChromaticTree<Key, Value, Compare, ValuePool>::
RetireOperation(HazardOperationPtr& operation) {
  Operation* op = operation.ReleaseHazard();
  if (op != Operation::INITIAL_DUMMY && op != Operation::RETIRED_DUMMY) {
    operation_hazard_manager_.EnqueuePointerForDeletion(op);
  }
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
typename ChromaticTree<Key, Value, Compare, ValuePool>::AtomicNodePtr&
ChromaticTree<Key, Value, Compare, ValuePool>::
GetNodeGuard(HazardIndex index) {
  return node_hazard_manager_.GetGuardedPointer(static_cast<int>(index));
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
typename ChromaticTree<Key, Value, Compare, ValuePool>::AtomicOperationPtr&
ChromaticTree<Key, Value, Compare, ValuePool>::
GetOperationGuard(HazardIndex index) {
  return operation_hazard_manager_.GetGuardedPointer(static_cast<int>(index));
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
bool ChromaticTree<Key, Value, Compare, ValuePool>::
WeakLLX(HazardNodePtr& node, HazardOperationPtr& operation) {
  // Make sure we have a protected operation pointer
  while (!operation.ProtectHazard(node->GetOperation()));

  // Node is not retired and operation is committed - node is available
  if (!node->IsRetired() && operation->IsCommitted()) {
    return true;
  }

  if (operation->IsAborted()) {
    // Operation is aborted, but the node is still frozen - unfreeze it
    operation->HelpAbort(node);

  } else if (operation->IsInProgress()) {
    // Operation is still in progress - help it
    operation->Help(GetNodeGuard(HIDX_HELPING),
                    GetOperationGuard(HIDX_HELPING));
  }

  // LLX failed - operation pointer should not be exposed
  operation.ReleaseHazard();

  return false;
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
void ChromaticTree<Key, Value, Compare, ValuePool>::
FreeNode(Node* node) {
#ifdef EMBB_DEBUG
  node->GetLeft()  = reinterpret_cast<Node*>(INVALID_POINTER);
  node->GetRight() = reinterpret_cast<Node*>(INVALID_POINTER);
#endif
  node_pool_.Free(node);
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
void ChromaticTree<Key, Value, Compare, ValuePool>::
FreeOperation(Operation* operation) {
#ifdef EMBB_DEBUG
  operation->SetDeleted();
#endif
  operation_pool_.Free(operation);
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
bool ChromaticTree<Key, Value, Compare, ValuePool>::
CleanUp(const Key& key) {
  (void)(key);
  return true;
//  HazardNodePtr grandgrandparent(node_hazard_manager_.GetGuardedPointer(0));
//  HazardNodePtr grandparent     (node_hazard_manager_.GetGuardedPointer(1));
//  HazardNodePtr parent          (node_hazard_manager_.GetGuardedPointer(2));
//  HazardNodePtr leaf            (node_hazard_manager_.GetGuardedPointer(3));
//  bool reached_leaf = false;
//
//  while (!reached_leaf) {
//    bool found_violation = false;
//
//    grandgrandparent.ProtectSafe(entry_);
//    grandparent.ProtectSafe(entry_);
//    parent.ProtectSafe(entry_);
//    leaf.ProtectHazard(entry_->GetLeft());
//    if (parent->IsRetired() || !leaf.IsActive()) continue;
//
//    reached_leaf = IsLeaf(leaf);
//    while (!reached_leaf && !found_violation) {
//      grandgrandparent.AdoptGuard(grandparent);
//      grandparent.AdoptGuard(parent);
//      parent.AdoptGuard(leaf);
//      leaf.ProtectHazard((IsSentinel(leaf) || compare_(key, leaf->GetKey())) ?
//                         leaf->GetLeft() : leaf->GetRight());
//      if (parent->IsRetired() || !leaf.IsActive()) break;
//      VERIFY_ADDRESS(static_cast<Node*>(leaf));
//
//      found_violation = (leaf->GetWeight() > 1) ||
//                        (leaf->GetWeight() == 0 && parent->GetWeight() == 0);
//
//      reached_leaf = IsLeaf(leaf);
//    }
//
//    if (found_violation) {
//      reached_leaf = false;
//
//      if (Rebalance(grandgrandparent, grandparent, parent, leaf) ==
//          EMBB_NOMEM) {
//        assert(false && "No memory for rebalancing!");
//        return false;
//      }
//    }
//  }
//
//  return true;
}

#define PROTECT_NODE_WITH_LOCK(node, lock_name) \
    UniqueLock lock_name(node->GetMutex(), embb::base::try_lock); \
    if (!lock_name.OwnsLock() || node->IsRetired()) return EMBB_BUSY;

#define DEFINE_NODE_WITH_HAZARD(h_num, node, parent, method) \
    HazardNodePtr node(node_hazard_manager_.GetGuardedPointer(h_num)); \
    node.ProtectHazard(parent->method()); \
    if (parent->IsRetired() || !node.IsActive()) return EMBB_BUSY; \
    VERIFY_ADDRESS(static_cast<Node*>(node))

template<typename Key, typename Value, typename Compare, typename ValuePool>
embb_errors_t ChromaticTree<Key, Value, Compare, ValuePool>::
Rebalance(HazardNodePtr& u, HazardNodePtr& ux, HazardNodePtr& uxx,
          HazardNodePtr& uxxx) {
  // Protect node 'u'
  PROTECT_NODE_WITH_LOCK(u, u_lock);
  // Verify that ux is still a child of u
  if (!HasChild(u, ux)) return EMBB_BUSY;

  // Protect node 'ux'
  PROTECT_NODE_WITH_LOCK(ux, ux_lock);
  // Get children of 'ux'
  DEFINE_NODE_WITH_HAZARD(4, uxl, ux, GetLeft);
  DEFINE_NODE_WITH_HAZARD(5, uxr, ux, GetRight);
  // Verify that 'uxx' is still a child of 'ux'
  bool uxx_is_left = (uxx == uxl); (void)uxx_is_left;
  if (!HasChild(ux, uxx)) return EMBB_BUSY;

  // Protect node 'uxx'
  PROTECT_NODE_WITH_LOCK(uxx, uxx_lock);
  // Get children of 'uxx'
  DEFINE_NODE_WITH_HAZARD(6, uxxl, uxx, GetLeft);
  DEFINE_NODE_WITH_HAZARD(7, uxxr, uxx, GetRight);
  // Verify that 'uxxx' is still a child of 'uxx'
  bool uxxx_is_left = (uxxx == uxxl);
  if (!HasChild(uxx, uxxx)) return EMBB_BUSY;

  if (uxxx->GetWeight() > 1) {
    if (uxxx_is_left) {
      // Protect node 'uxxl'
      PROTECT_NODE_WITH_LOCK(uxxl, uxxl_lock);
      return OverweightLeft(u, u_lock, ux, ux_lock, uxx, uxx_lock, uxl, uxr,
                            uxxl, uxxl_lock, uxxr, uxx_is_left);
    } else {
      // Protect node 'uxxr'
      PROTECT_NODE_WITH_LOCK(uxxr, uxxr_lock);
      return OverweightRight(u, u_lock, ux, ux_lock, uxx, uxx_lock, uxl, uxr,
                             uxxl, uxxr, uxxr_lock, !uxx_is_left);
    }
  } else {
    assert(uxxx->GetWeight() == 0 && uxx->GetWeight() == 0); //Red-red violation
    if (uxx_is_left) {
      if (uxr->GetWeight() == 0) {
        // Protect node 'uxr'
        PROTECT_NODE_WITH_LOCK(uxr, uxr_lock);
        return BLK(u, u_lock, ux, ux_lock, uxx, uxx_lock, uxr, uxr_lock);
      } else if (uxxx_is_left) {
        return RB1_L(u, u_lock, ux, ux_lock, uxx, uxx_lock);
      } else {
        // Protect node 'uxxr'
        PROTECT_NODE_WITH_LOCK(uxxr, uxxr_lock);
        return RB2_L(u, u_lock, ux, ux_lock, uxx, uxx_lock, uxxr, uxxr_lock);
      }
    } else {
      if (uxl->GetWeight() == 0) {
        // Protect node 'uxl'
        PROTECT_NODE_WITH_LOCK(uxl, uxl_lock);
        return BLK(u, u_lock, ux, ux_lock, uxl, uxl_lock, uxx, uxx_lock);
      } else if (!uxxx_is_left) {
        return RB1_R(u, u_lock, ux, ux_lock, uxx, uxx_lock);
      } else {
        // Protect node 'uxxl'
        PROTECT_NODE_WITH_LOCK(uxxl, uxxl_lock);
        return RB2_R(u, u_lock, ux, ux_lock, uxx, uxx_lock, uxxl, uxxl_lock);
      }
    }
  }
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
embb_errors_t ChromaticTree<Key, Value, Compare, ValuePool>::
OverweightLeft(HazardNodePtr& u, UniqueLock& u_lock,
               HazardNodePtr& ux, UniqueLock& ux_lock,
               HazardNodePtr& uxx, UniqueLock& uxx_lock,
               HazardNodePtr& uxl, HazardNodePtr& uxr,
               HazardNodePtr& uxxl, UniqueLock& uxxl_lock,
               HazardNodePtr& uxxr, bool uxx_is_left) {
  // Let "Root" be the top of the overweight violation decision tree (see p.30)
  // Root -> Middle
  if (uxxr->GetWeight() == 0) {
    // Root -> Middle -> Left
    if (uxx->GetWeight() == 0) {
      // Root -> Middle -> Left -> Left
      if (uxx_is_left) {
        // Root -> Middle -> Left -> Left -> Left
        if (uxr->GetWeight() == 0) {
          // Protect node 'uxr'
          PROTECT_NODE_WITH_LOCK(uxr, uxr_lock);
          return BLK(u, u_lock, ux, ux_lock, uxx, uxx_lock, uxr, uxr_lock);

        // Root -> Middle -> Left -> Left -> Right
        } else {
          assert(uxr->GetWeight() > 0);
          // Protect node 'uxxr'
          PROTECT_NODE_WITH_LOCK(uxxr, uxxr_lock);
          return RB2_L(u, u_lock, ux, ux_lock, uxx, uxx_lock, uxxr, uxxr_lock);
        }

      // Root -> Middle -> Left -> Right
      } else {
        assert(!uxx_is_left);
        // Root -> Middle -> Left -> Right -> Left
        if (uxl->GetWeight() == 0) {
          // Protect node 'uxl'
          PROTECT_NODE_WITH_LOCK(uxl, uxl_lock);
          return BLK(u, u_lock, ux, ux_lock, uxl, uxl_lock, uxx, uxx_lock);

        // Root -> Middle -> Left -> Right -> Right
        } else {
          assert(uxl->GetWeight() > 0);
          return RB1_R(u, u_lock, ux, ux_lock, uxx, uxx_lock);
        }
      }

    // Root -> Middle -> Right
    } else {
      assert(uxx->GetWeight() > 0);
      // Protect node 'uxxr'
      PROTECT_NODE_WITH_LOCK(uxxr, uxxr_lock);

      // Get left child of 'uxxr'
      // Note: we know that 'uxxr' is not a leaf because it has weight 0.
      DEFINE_NODE_WITH_HAZARD(8, uxxrl, uxxr, GetLeft);

      // Protect node 'uxxrl'
      PROTECT_NODE_WITH_LOCK(uxxrl, uxxrl_lock);

      // Root -> Middle -> Right -> Left
      if (uxxrl->GetWeight() == 0) {
        return RB2_R(ux, ux_lock, uxx, uxx_lock,
                     uxxr, uxxr_lock, uxxrl, uxxrl_lock);

      // Root -> Middle -> Right -> Middle
      } else if (uxxrl->GetWeight() == 1) {
        DEFINE_NODE_WITH_HAZARD(9, uxxrlr, uxxrl, GetRight);
        if (uxxrlr == NULL) return EMBB_BUSY;

        // Root -> Middle -> Right -> Middle -> Left
        if (uxxrlr->GetWeight() == 0) {
          // Protect node 'uxxrlr'
          PROTECT_NODE_WITH_LOCK(uxxrlr, uxxrlr_lock);
          return W4_L(ux, ux_lock, uxx, uxx_lock, uxxl, uxxl_lock,
                      uxxr, uxxr_lock, uxxrl, uxxrl_lock, uxxrlr, uxxrlr_lock);

        // Root -> Middle -> Right -> Middle -> Right
        } else {
          assert(uxxrlr->GetWeight() > 0);
          // Root -> Middle -> Right -> Middle -> Right -> Left
          // Node: reusing hazard of node 'uxxrlr' as it is no longer used
          DEFINE_NODE_WITH_HAZARD(9, uxxrll, uxxrl, GetLeft);
          if (uxxrll->GetWeight() == 0) {
            // Protect node 'uxxrll'
            PROTECT_NODE_WITH_LOCK(uxxrll, uxxrll_lock);
            return W3_L(ux, ux_lock, uxx, uxx_lock, uxxl, uxxl_lock, uxxr,
                        uxxr_lock, uxxrl, uxxrl_lock, uxxrll, uxxrll_lock);

          // Root -> Middle -> Right -> Middle -> Right -> Right
          } else {
            assert(uxxrll->GetWeight() > 0);
            return W2_L(ux, ux_lock, uxx, uxx_lock, uxxl, uxxl_lock,
                        uxxr, uxxr_lock, uxxrl, uxxrl_lock);
          }
        }

      // Root -> Middle -> Right -> Right
      } else {
        assert(uxxrl->GetWeight() > 1);
        return W1_L(ux, ux_lock, uxx, uxx_lock, uxxl, uxxl_lock,
                    uxxr, uxxr_lock, uxxrl, uxxrl_lock);
      }
    }

  // Root -> Right
  } else if (uxxr->GetWeight() == 1) {
    // Protect node 'uxxr'
    PROTECT_NODE_WITH_LOCK(uxxr, uxxr_lock);
    // Get children of 'uxxr'
    DEFINE_NODE_WITH_HAZARD(8, uxxrl, uxxr, GetLeft);
    DEFINE_NODE_WITH_HAZARD(9, uxxrr, uxxr, GetRight);
    if (uxxrl == NULL) return EMBB_BUSY;

    // Root -> Right -> Left
    if (uxxrr->GetWeight() == 0) {
      // Protect node 'uxxrr'
      PROTECT_NODE_WITH_LOCK(uxxrr, uxxrr_lock);
      return W5_L(ux, ux_lock, uxx, uxx_lock, uxxl, uxxl_lock,
                  uxxr, uxxr_lock, uxxrr, uxxrr_lock);

    // Root -> Right -> Right
    } else {
      assert(uxxrr->GetWeight() > 0);
      // Root -> Right -> Right -> Left
      if (uxxrl->GetWeight() == 0) {
        // Protect node 'uxxrl'
        PROTECT_NODE_WITH_LOCK(uxxrl, uxxrl_lock);
        return W6_L(ux, ux_lock, uxx, uxx_lock, uxxl, uxxl_lock,
                    uxxr, uxxr_lock, uxxrl, uxxrl_lock);

      // Root -> Right -> Right -> Right
      } else {
        assert(uxxrl->GetWeight() > 0);
        return PUSH_L(ux, ux_lock, uxx, uxx_lock,
                      uxxl, uxxl_lock, uxxr, uxxr_lock);
      }
    }

  // Root -> Left
  } else {
    assert(uxxr->GetWeight() > 1);
    // Protect node 'uxxr'
    PROTECT_NODE_WITH_LOCK(uxxr, uxxr_lock);
    return W7(ux, ux_lock, uxx, uxx_lock, uxxl, uxxl_lock, uxxr, uxxr_lock);
  }
}

template<typename Key, typename Value, typename Compare, typename ValuePool>
embb_errors_t ChromaticTree<Key, Value, Compare, ValuePool>::
OverweightRight(HazardNodePtr& u, UniqueLock& u_lock,
                HazardNodePtr& ux, UniqueLock& ux_lock,
                HazardNodePtr& uxx, UniqueLock& uxx_lock,
                HazardNodePtr& uxl, HazardNodePtr& uxr,
                HazardNodePtr& uxxl, HazardNodePtr& uxxr,
                UniqueLock& uxxr_lock, bool uxx_is_right) {
  // Let "Root" be the top of the overweight violation decision tree (see p.30)
  // Root -> Middle
  if (uxxl->GetWeight() == 0) {
    // Root -> Middle -> Left
    if (uxx->GetWeight() == 0) {
      // Root -> Middle -> Left -> Left
      if (uxx_is_right) {
        // Root -> Middle -> Left -> Left -> Left
        if (uxl->GetWeight() == 0) {
          // Protect node 'uxl'
          PROTECT_NODE_WITH_LOCK(uxl, uxl_lock);
          return BLK(u, u_lock, ux, ux_lock, uxl, uxl_lock, uxx, uxx_lock);

        // Root -> Middle -> Left -> Left -> Right
        } else {
          assert(uxl->GetWeight() > 0);
          // Protect node 'uxxl'
          PROTECT_NODE_WITH_LOCK(uxxl, uxxl_lock);
          return RB2_R(u, u_lock, ux, ux_lock, uxx, uxx_lock, uxxl, uxxl_lock);
        }

      // Root -> Middle -> Left -> Right
      } else {
        assert(!uxx_is_right);
        // Root -> Middle -> Left -> Right -> Left
        if (uxr->GetWeight() == 0) {
          // Protect node 'uxr'
          PROTECT_NODE_WITH_LOCK(uxr, uxr_lock);
          return BLK(u, u_lock, ux, ux_lock, uxx, uxx_lock, uxr, uxr_lock);

        // Root -> Middle -> Left -> Right -> Right
        } else {
          assert(uxr->GetWeight() > 0);
          return RB1_L(u, u_lock, ux, ux_lock, uxx, uxx_lock);
        }
      }

    // Root -> Middle -> Right
    } else {
      assert(uxx->GetWeight() > 0);
      // Protect node 'uxxl'
      PROTECT_NODE_WITH_LOCK(uxxl, uxxl_lock);

      // Get left child of 'uxxl'
      // Note: we know that 'uxxl' is not a leaf because it has weight 0.
      DEFINE_NODE_WITH_HAZARD(8, uxxlr, uxxl, GetRight);

      // Protect node 'uxxlr'
      PROTECT_NODE_WITH_LOCK(uxxlr, uxxlr_lock);

      // Root -> Middle -> Right -> Left
      if (uxxlr->GetWeight() == 0) {
        return RB2_L(ux, ux_lock, uxx, uxx_lock,
                     uxxl, uxxl_lock, uxxlr, uxxlr_lock);

      // Root -> Middle -> Right -> Middle
      } else if (uxxlr->GetWeight() == 1) {
        DEFINE_NODE_WITH_HAZARD(9, uxxlrl, uxxlr, GetLeft);
        if (uxxlrl == NULL) return EMBB_BUSY;

        // Root -> Middle -> Right -> Middle -> Left
        if (uxxlrl->GetWeight() == 0) {
          // Protect node 'uxxlrl'
          PROTECT_NODE_WITH_LOCK(uxxlrl, uxxlrl_lock);
          return W4_R(ux, ux_lock, uxx, uxx_lock, uxxl, uxxl_lock,
                      uxxr, uxxr_lock, uxxlr, uxxlr_lock, uxxlrl, uxxlrl_lock);

        // Root -> Middle -> Right -> Middle -> Right
        } else {
          assert(uxxlrl->GetWeight() > 0);
          // Root -> Middle -> Right -> Middle -> Right -> Left
          // Node: reusing hazard of node 'uxxlrl' as it is no longer used
          DEFINE_NODE_WITH_HAZARD(9, uxxlrr, uxxlr, GetRight);
          if (uxxlrr->GetWeight() == 0) {
            // Protect node 'uxxlrr'
            PROTECT_NODE_WITH_LOCK(uxxlrr, uxxlrr_lock);
            return W3_R(ux, ux_lock, uxx, uxx_lock, uxxl, uxxl_lock, uxxr,
                        uxxr_lock, uxxlr, uxxlr_lock, uxxlrr, uxxlrr_lock);

          // Root -> Middle -> Right -> Middle -> Right -> Right
          } else {
            assert(uxxlrr->GetWeight() > 0);
            return W2_R(ux, ux_lock, uxx, uxx_lock, uxxl, uxxl_lock,
                        uxxr, uxxr_lock, uxxlr, uxxlr_lock);
          }
        }

      // Root -> Middle -> Right -> Right
      } else {
        assert(uxxlr->GetWeight() > 1);
        return W1_R(ux, ux_lock, uxx, uxx_lock, uxxl, uxxl_lock,
                    uxxr, uxxr_lock, uxxlr, uxxlr_lock);
      }
    }

  // Root -> Right
  } else if (uxxl->GetWeight() == 1) {
    // Protect node 'uxxl'
    PROTECT_NODE_WITH_LOCK(uxxl, uxxl_lock);
    // Get children of 'uxxl'
    DEFINE_NODE_WITH_HAZARD(8, uxxll, uxxl, GetLeft);
    DEFINE_NODE_WITH_HAZARD(9, uxxlr, uxxl, GetRight);
    if (uxxll == NULL) return EMBB_BUSY;

    // Root -> Right -> Left
    if (uxxll->GetWeight() == 0) {
      // Protect node 'uxxll'
      PROTECT_NODE_WITH_LOCK(uxxll, uxxll_lock);
      return W5_R(ux, ux_lock, uxx, uxx_lock, uxxl, uxxl_lock,
                  uxxr, uxxr_lock, uxxll, uxxll_lock);

    // Root -> Right -> Right
    } else {
      assert(uxxll->GetWeight() > 0);
      // Root -> Right -> Right -> Left
      if (uxxlr->GetWeight() == 0) {
        // Protect node 'uxxlr'
        PROTECT_NODE_WITH_LOCK(uxxlr, uxxlr_lock);
        return W6_R(ux, ux_lock, uxx, uxx_lock, uxxl, uxxl_lock,
                    uxxr, uxxr_lock, uxxlr, uxxlr_lock);

      // Root -> Right -> Right -> Right
      } else {
        assert(uxxlr->GetWeight() > 0);
        return PUSH_R(ux, ux_lock, uxx, uxx_lock,
                      uxxl, uxxl_lock, uxxr, uxxr_lock);
      }
    }

  // Root -> Left
  } else {
    assert(uxxl->GetWeight() > 1);
    // Protect node 'uxxl'
    PROTECT_NODE_WITH_LOCK(uxxl, uxxl_lock);
    return W7(ux, ux_lock, uxx, uxx_lock, uxxl, uxxl_lock, uxxr, uxxr_lock);
  }
}

} // namespace containers
} // namespace embb

#endif // EMBB_CONTAINERS_INTERNAL_LOCK_FREE_CHROMATIC_TREE_INL_H_
