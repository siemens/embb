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
ChromaticTreeNode(const Key& key, const Value& value, const int weight,
                  const ChildPointer& left, const ChildPointer& right)
    : key_(key),
      value_(value),
      weight_(weight),
      left_(left),
      right_(right),
      retired_(false) {}

template<typename Key, typename Value>
ChromaticTreeNode<Key, Value>::
ChromaticTreeNode(const Key& key, const Value& value, const int weight)
    : key_(key),
      value_(value),
      weight_(weight),
      left_(NULL),
      right_(NULL),
      retired_(false)  {}

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
typename ChromaticTreeNode<Key, Value>::AtomicChildPointer&
ChromaticTreeNode<Key, Value>::GetLeft() {
  return left_;
}

template<typename Key, typename Value>
typename ChromaticTreeNode<Key, Value>::AtomicChildPointer&
ChromaticTreeNode<Key, Value>::GetRight() {
  return right_;
}

} // namespace internal


template<typename Key, typename Value, typename Compare, typename NodePool>
ChromaticTree<Key, Value, Compare, NodePool>::
ChromaticTree(size_t capacity, Key undefined_key, Value undefined_value,
              Compare compare)
#ifdef EMBB_PLATFORM_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable:4355)
#endif
    :  free_node_callback_(*this, &ChromaticTree::FreeNode),
#ifdef EMBB_PLATFORM_COMPILER_MSVC
#pragma warning(pop)
#endif
      node_hazard_manager_(free_node_callback_, NULL, 8),
      undefined_key_(undefined_key),
      undefined_value_(undefined_value),
      compare_(compare),
      capacity_(capacity),
      node_pool_(2 + 5 + 2 * capacity_ +
                 node_hazard_manager_.GetRetiredListMaxSize() *
                 embb::base::Thread::GetThreadsMaxCount()) {
  entry_ = node_pool_.Allocate(undefined_key_, undefined_value_);
  NodePtr sentinel = node_pool_.Allocate(undefined_key_, undefined_value_);
  entry_->GetLeft() = sentinel;
}

template<typename Key, typename Value, typename Compare, typename NodePool>
ChromaticTree<Key, Value, Compare, NodePool>::
~ChromaticTree() {
  Destruct(entry_->GetLeft());
  FreeNode(entry_);
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
Get(const Key& key, Value& value) {
  NodePtr leaf;
  Search(key, leaf);

  bool keys_are_equal = !IsSentinel(leaf) &&
                        !(compare_(key, leaf->GetKey()) ||
                          compare_(leaf->GetKey(), key));

  if (keys_are_equal) {
    value = leaf->GetValue();
  }

  node_hazard_manager_.GuardPointer(0, NULL);

  return keys_are_equal;
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
TryInsert(const Key& key, const Value& value) {
  Value old_value;
  return TryInsert(key, value, old_value);
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
TryInsert(const Key& key, const Value& value, Value& old_value) {
  NodePtr new_leaf = NULL;
  NodePtr new_sibling = NULL;
  NodePtr new_parent = NULL;
  bool insertion_succeeded = false;

  while (!insertion_succeeded) {
    NodePtr leaf, parent;
    Search(key, leaf, parent);

    // Try to lock the parent
    UniqueLock parent_lock(parent->GetMutex(), embb::base::try_lock);
    if (!parent_lock.OwnsLock() || parent->IsRetired()) continue;
    // Verify that the leaf is still the parent's child
    if (!HasChild(parent, leaf)) continue;

    // Try to lock the leaf
    UniqueLock leaf_lock(leaf->GetMutex(), embb::base::try_lock);
    if (!leaf_lock.OwnsLock() || leaf->IsRetired()) continue;

    // Reached leaf has a matching key: replace it with a new copy
    if (!IsSentinel(leaf) && !(compare_(key, leaf->GetKey()) ||
                               compare_(leaf->GetKey(), key))) {
      old_value = leaf->GetValue();
      new_parent = node_pool_.Allocate(key, value, leaf->GetWeight());
      if (new_parent == NULL) break;

    // Reached leaf has a different key: add a new leaf
    } else {
      old_value = undefined_value_;

      new_leaf = node_pool_.Allocate(key, value);
      if (new_leaf == NULL) break;
      new_sibling = node_pool_.Allocate(leaf->GetKey(), leaf->GetValue());
      if (new_sibling == NULL) break;

      int new_weight = (HasFixedWeight(leaf)) ? 1 : (leaf->GetWeight() - 1);
      if (IsSentinel(leaf) || compare_(key, leaf->GetKey())) {
        new_parent = node_pool_.Allocate(leaf->GetKey(), undefined_value_,
                                         new_weight, new_leaf, new_sibling);
      } else {
        new_parent = node_pool_.Allocate(key, undefined_value_,
                                         new_weight, new_sibling, new_leaf);
      }
      if (new_parent == NULL) break;
    }

    GetPointerToChild(parent, leaf).CompareAndSwap(leaf, new_parent);

    insertion_succeeded = true;

    leaf->Retire();
    leaf_lock.Unlock();
    node_hazard_manager_.GuardPointer(1, NULL);
    RemoveNode(leaf);

//    if (parent->GetWeight() == 0 && new_parent->GetWeight() == 0) {
//      CleanUp(key);
//    }
  }
  node_hazard_manager_.GuardPointer(0, NULL);
  node_hazard_manager_.GuardPointer(1, NULL);

  if (!insertion_succeeded) {
    if (new_leaf != NULL)    FreeNode(new_leaf);
    if (new_sibling != NULL) FreeNode(new_sibling);
    if (new_parent != NULL)  FreeNode(new_parent);
  }

  return insertion_succeeded;
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
TryDelete(const Key& key) {
  Value old_value;
  return TryDelete(key, old_value);
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
TryDelete(const Key& key, Value& old_value) {
  NodePtr new_leaf = NULL;
  bool deletion_succeeded = false;

  while (!deletion_succeeded) {
    NodePtr leaf, parent, grandparent;
    Search(key, leaf, parent, grandparent);

    // Reached leaf has a different key - nothing to delete
    if (IsSentinel(leaf) || (compare_(key, leaf->GetKey()) ||
                             compare_(leaf->GetKey(), key))) {
      old_value = undefined_value_;
      deletion_succeeded = true;
      break;
    }

    // Try to lock the grandparent
    UniqueLock grandparent_lock(grandparent->GetMutex(), embb::base::try_lock);
    if (!grandparent_lock.OwnsLock() || grandparent->IsRetired()) continue;
    // Verify that the parent is still the grandparent's child
    if (!HasChild(grandparent, parent)) continue;

    // Try to lock the parent
    UniqueLock parent_lock(parent->GetMutex(), embb::base::try_lock);
    if (!parent_lock.OwnsLock() || parent->IsRetired()) continue;
    // Verify that the leaf is still the parent's child
    if (!HasChild(parent, leaf)) continue;

    AtomicNodePtr& other_child = ((parent->GetLeft() == leaf) ?
                                  parent->GetRight() : parent->GetLeft());
    NodePtr sibling = other_child;
    node_hazard_manager_.GuardPointer(3, sibling);
    if (sibling != other_child) continue;
    VERIFY_ADDRESS(sibling);

    // Try to lock the sibling
    UniqueLock sibling_lock(sibling->GetMutex(), embb::base::try_lock);
    if (!sibling_lock.OwnsLock() || sibling->IsRetired()) continue;

    // Try to lock the leaf
    UniqueLock leaf_lock(leaf->GetMutex(), embb::base::try_lock);
    if (!leaf_lock.OwnsLock() || leaf->IsRetired()) continue;

    int new_weight = (HasFixedWeight(parent)) ?
                  1 : (parent->GetWeight() + sibling->GetWeight());

    new_leaf = node_pool_.Allocate(
        sibling->GetKey(), sibling->GetValue(), new_weight,
        sibling->GetLeft(), sibling->GetRight());
    if (new_leaf == NULL) break;

    old_value = leaf->GetValue();

    GetPointerToChild(grandparent, parent).CompareAndSwap(parent, new_leaf);

    deletion_succeeded = true;

    parent->Retire();
    parent_lock.Unlock();
    node_hazard_manager_.GuardPointer(1, NULL);
    RemoveNode(parent);
    leaf->Retire();
    leaf_lock.Unlock();
    node_hazard_manager_.GuardPointer(2, NULL);
    RemoveNode(leaf);
    sibling->Retire();
    sibling_lock.Unlock();
    node_hazard_manager_.GuardPointer(3, NULL);
    RemoveNode(sibling);

//    if (new_weight > 1) {
//      CleanUp(key);
//    }
  }
  node_hazard_manager_.GuardPointer(0, NULL);
  node_hazard_manager_.GuardPointer(1, NULL);
  node_hazard_manager_.GuardPointer(2, NULL);
  node_hazard_manager_.GuardPointer(3, NULL);

  if (!deletion_succeeded) {
    if (new_leaf != NULL)    FreeNode(new_leaf);
  }

  return deletion_succeeded;
}

template<typename Key, typename Value, typename Compare, typename NodePool>
size_t ChromaticTree<Key, Value, Compare, NodePool>::
GetCapacity() {
  return capacity_;
}

template<typename Key, typename Value, typename Compare, typename NodePool>
const Value& ChromaticTree<Key, Value, Compare, NodePool>::
GetUndefinedValue() {
  return undefined_value_;
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
IsEmpty() {
  return IsLeaf(entry_->GetLeft());
}

template<typename Key, typename Value, typename Compare, typename NodePool>
void ChromaticTree<Key, Value, Compare, NodePool>::
Search(const Key& key, NodePtr& leaf) {
  NodePtr parent;
  Search(key, leaf, parent);
  node_hazard_manager_.GuardPointer(0, leaf);
  node_hazard_manager_.GuardPointer(1, NULL);
}

template<typename Key, typename Value, typename Compare, typename NodePool>
void ChromaticTree<Key, Value, Compare, NodePool>::
Search(const Key& key, NodePtr& leaf, NodePtr& parent) {
  bool reached_leaf = false;

  while (!reached_leaf) {
    parent = entry_;
    node_hazard_manager_.GuardPointer(0, parent);

    leaf = entry_->GetLeft();
    node_hazard_manager_.GuardPointer(1, leaf);
    if (leaf != entry_->GetLeft()) continue;

    reached_leaf = IsLeaf(leaf);
    while (!reached_leaf) {
      parent = leaf;
      node_hazard_manager_.GuardPointer(0, parent);

      AtomicNodePtr& next_leaf =
          (IsSentinel(leaf) || compare_(key, leaf->GetKey())) ?
          leaf->GetLeft() : leaf->GetRight();
      leaf = next_leaf;
      node_hazard_manager_.GuardPointer(1, leaf);
      if (leaf != next_leaf || parent->IsRetired()) break;

      VERIFY_ADDRESS(leaf);
      reached_leaf = IsLeaf(leaf);
    }
  }
}

template<typename Key, typename Value, typename Compare, typename NodePool>
void ChromaticTree<Key, Value, Compare, NodePool>::
Search(const Key& key, NodePtr& leaf, NodePtr& parent, NodePtr& grandparent) {
  bool reached_leaf = false;

  while (!reached_leaf) {
    grandparent = NULL;

    parent = entry_;
    node_hazard_manager_.GuardPointer(1, parent);

    leaf = entry_->GetLeft();
    node_hazard_manager_.GuardPointer(2, leaf);
    if (leaf != entry_->GetLeft()) continue;

    reached_leaf = IsLeaf(leaf);
    while (!reached_leaf) {
      grandparent = parent;
      node_hazard_manager_.GuardPointer(0, grandparent);

      parent = leaf;
      node_hazard_manager_.GuardPointer(1, parent);

      AtomicNodePtr& next_leaf =
          (IsSentinel(leaf) || compare_(key, leaf->GetKey())) ?
          leaf->GetLeft() : leaf->GetRight();
      leaf = next_leaf;
      node_hazard_manager_.GuardPointer(2, leaf);
      if (leaf != next_leaf || parent->IsRetired()) break;

      VERIFY_ADDRESS(leaf);
      reached_leaf = IsLeaf(leaf);
    }
  }
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
IsLeaf(const NodePtr& node) const {
  return node->GetLeft() == NULL;
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
IsSentinel(const NodePtr& node) const {
  return (node == entry_) || (node == entry_->GetLeft());
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
HasFixedWeight(const NodePtr& node) const {
  return (IsSentinel(node)) || (node == entry_->GetLeft()->GetLeft());
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
HasChild(const NodePtr& parent, const NodePtr& child) const {
  return (parent->GetLeft() == child || parent->GetRight() == child);
}

template<typename Key, typename Value, typename Compare, typename NodePool>
typename ChromaticTree<Key, Value, Compare, NodePool>::AtomicNodePtr&
ChromaticTree<Key, Value, Compare, NodePool>::
GetPointerToChild(const NodePtr& parent, const NodePtr& child) const {
  assert(HasChild(parent, child));
  return (parent->GetLeft() == child) ? parent->GetLeft() : parent->GetRight();
}

template<typename Key, typename Value, typename Compare, typename NodePool>
void ChromaticTree<Key, Value, Compare, NodePool>::
Destruct(const NodePtr& node) {
  if (!IsLeaf(node)) {
    Destruct(node->GetLeft());
    Destruct(node->GetRight());
  }
  FreeNode(node);
}

template<typename Key, typename Value, typename Compare, typename NodePool>
int ChromaticTree<Key, Value, Compare, NodePool>::
GetHeight(const NodePtr& node) const {
  int height = 0;
  if (node != NULL) {
    height = 1 + ::std::max(GetHeight(node->GetLeft()),
                            GetHeight(node->GetRight()));
  }
  return height;
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
IsBalanced() const {
  return IsBalanced(entry_->GetLeft());
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
IsBalanced(const NodePtr& node) const {
  // Overweight violation
  bool has_violation = node->GetWeight() > 1;

  if (!has_violation && !IsLeaf(node)) {
    NodePtr left  = node->GetLeft();
    NodePtr right = node->GetRight();

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

template<typename Key, typename Value, typename Compare, typename NodePool>
void ChromaticTree<Key, Value, Compare, NodePool>::
FreeNode(NodePtr node) {
#ifdef EMBB_DEBUG
  node->GetLeft()  = reinterpret_cast<NodePtr>(INVALID_POINTER);
  node->GetRight() = reinterpret_cast<NodePtr>(INVALID_POINTER);
#endif
  node_pool_.Free(node);
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
CleanUp(const Key& key) {
  for (;;) {
    NodePtr grandgrandparent = NULL;
    NodePtr grandparent = NULL;
    NodePtr parent = entry_;
    NodePtr leaf = entry_->GetLeft();

    while (!IsLeaf(leaf) && (leaf->GetWeight() <= 1) &&
           (leaf->GetWeight() != 0 || parent->GetWeight() != 0)) {
      grandgrandparent = grandparent;
      grandparent = parent;
      parent = leaf;
      leaf = (IsSentinel(leaf) || compare_(key, leaf->GetKey())) ?
             leaf->GetLeft() : leaf->GetRight();
      VERIFY_ADDRESS(leaf);
    }

    if (leaf->GetWeight() == 1) {
      break;
    }

    if (!Rebalance(grandgrandparent, grandparent, parent, leaf)) {
      return false;
    }
  }

  return true;
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
Rebalance(const NodePtr& u, const NodePtr& ux, const NodePtr& uxx,
          const NodePtr& uxxx) {
  //TODO: weakLLX(u);
  if (!HasChild(u, ux)) return false;

  //TODO: weakLLX(ux);
  NodePtr uxl = ux->GetLeft(); VERIFY_ADDRESS(uxl);
  NodePtr uxr = ux->GetRight(); VERIFY_ADDRESS(uxr);
  bool uxx_is_left = (uxx == uxl);
  if (!HasChild(ux, uxx)) return false;

  //TODO: weakLLX(uxx);
  NodePtr uxxl = uxx->GetLeft(); VERIFY_ADDRESS(uxxl);
  NodePtr uxxr = uxx->GetRight(); VERIFY_ADDRESS(uxxr);
  bool uxxx_is_left = (uxxx == uxxl);
  if (!HasChild(uxx, uxxx)) return false;

  if (uxxx->GetWeight() > 1) {
    if (uxxx_is_left) {
      //TODO: weakLLX(uxxl);
      return OverweightLeft(u, ux, uxx, uxl, uxr, uxxl, uxxr, uxx_is_left);
    } else {
      //TODO: weakLLX(uxxr);
      return OverweightRight(u, ux, uxx, uxl, uxr, uxxl, uxxr, !uxx_is_left);
    }
  } else {
    if (uxx_is_left) {
      if (uxr->GetWeight() == 0) {
        //TODO: weakLLX(uxr);
        return BLK(u, ux, uxx, uxr);
      } else if (uxxx_is_left) {
        return RB1_L(u, ux, uxx);
      } else {
        //TODO: weakLLX(uxxr);
        return RB2_L(u, ux, uxx, uxxr);
      }
    } else {
      if (uxl->GetWeight() == 0) {
        //TODO: weakLLX(uxl);
        return BLK(u, ux, uxl, uxx);
      } else if (!uxxx_is_left) {
        return RB1_R(u, ux, uxx);
      } else {
        //TODO: weakLLX(uxxl);
        return RB2_R(u, ux, uxx, uxxl);
      }
    }
  }
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
OverweightLeft(const NodePtr& u, const NodePtr& ux, const NodePtr& uxx,
               const NodePtr& uxl, const NodePtr& uxr,
               const NodePtr& uxxl, const NodePtr& uxxr,
               const bool& uxx_is_left) {
  // Let "Root" be the top of the overweight violation decision tree (see p.30)
  // Root -> Middle
  if (uxxr->GetWeight() == 0) {
    // Root -> Middle -> Left
    if (uxx->GetWeight() == 0) {
      // Root -> Middle -> Left -> Left
      if (uxx_is_left) {
        // Root -> Middle -> Left -> Left -> Left
        if (uxr->GetWeight() == 0) {
          //TODO: weakLLX(uxr);
          return BLK(u, ux, uxx, uxr);

        // Root -> Middle -> Left -> Left -> Right
        } else {
          assert(uxr->GetWeight() > 0);
          //TODO: weakLLX(uxxr);
          return RB2_L(u, ux, uxx, uxxr);
        }

      // Root -> Middle -> Left -> Right
      } else {
        assert(!uxx_is_left);
        // Root -> Middle -> Left -> Right -> Left
        if (uxl->GetWeight() == 0) {
          //TODO: weakLLX(uxl);
          return BLK(u, ux, uxl, uxx);

        // Root -> Middle -> Left -> Right -> Right
        } else {
          assert(uxl->GetWeight() > 0);
          return RB1_R(u, ux, uxx);
        }
      }

    // Root -> Middle -> Right
    } else {
      assert(uxx->GetWeight() > 0);
      //TODO: weakLLX(uxxr);
      // Note: we know that 'uxxr' is not a leaf because it has weight 0.
      NodePtr uxxrl = uxxr->GetLeft(); VERIFY_ADDRESS(uxxrl);
      //TODO: weakLLX(uxxrl);

      // Root -> Middle -> Right -> Left
      if (uxxrl->GetWeight() == 0) {
        return RB2_R(ux, uxx, uxxr, uxxrl);

      // Root -> Middle -> Right -> Middle
      } else if (uxxrl->GetWeight() == 1) {
        NodePtr uxxrll = uxxrl->GetLeft(); VERIFY_ADDRESS(uxxrll);
        NodePtr uxxrlr = uxxrl->GetRight(); VERIFY_ADDRESS(uxxrlr);
        if (uxxrlr == NULL) return false;

        // Root -> Middle -> Right -> Middle -> Left
        if (uxxrlr->GetWeight() == 0) {
          //TODO: weakLLX(uxxrlr);
          return W4_L(ux, uxx, uxxl, uxxr, uxxrl, uxxrlr);

        // Root -> Middle -> Right -> Middle -> Right
        } else {
          assert(uxxrlr->GetWeight() > 0);
          // Root -> Middle -> Right -> Middle -> Right -> Left
          if (uxxrll->GetWeight() == 0) {
            //TODO: weakLLX(uxxrll);
            return W3_L(ux, uxx, uxxl, uxxr, uxxrl, uxxrll);

          // Root -> Middle -> Right -> Middle -> Right -> Right
          } else {
            assert(uxxrll->GetWeight() > 0);
            return W2_L(ux, uxx, uxxl, uxxr, uxxrl);
          }
        }

      // Root -> Middle -> Right -> Right
      } else {
        assert(uxxrl->GetWeight() > 1);
        return W1_L(ux, uxx, uxxl, uxxr, uxxrl);
      }
    }

  // Root -> Right
  } else if (uxxr->GetWeight() == 1) {
    //TODO: weakLLX(uxxr);
    NodePtr uxxrl = uxxr->GetLeft(); VERIFY_ADDRESS(uxxrl);
    NodePtr uxxrr = uxxr->GetRight(); VERIFY_ADDRESS(uxxrr);
    if (uxxrl == NULL) return false;

    // Root -> Right -> Left
    if (uxxrr->GetWeight() == 0) {
      //TODO: weakLLX(uxxrr);
      return W5_L(ux, uxx, uxxl, uxxr, uxxrr);

    // Root -> Right -> Right
    } else {
      assert(uxxrr->GetWeight() > 0);
      // Root -> Right -> Right -> Left
      if (uxxrl->GetWeight() == 0) {
        //TODO: weakLLX(uxxrl);
        return W6_L(ux, uxx, uxxl, uxxr, uxxrl);

      // Root -> Right -> Right -> Right
      } else {
        assert(uxxrl->GetWeight() > 0);
        return PUSH_L(ux, uxx, uxxl, uxxr);
      }
    }

  // Root -> Left
  } else {
    assert(uxxr->GetWeight() > 1);
    //TODO: weakLLX(uxxr);
    return W7(ux, uxx, uxxl, uxxr);
  }
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
OverweightRight(const NodePtr& u, const NodePtr& ux, const NodePtr& uxx,
                const NodePtr& uxl, const NodePtr& uxr,
                const NodePtr& uxxl, const NodePtr& uxxr,
                const bool& uxx_is_right) {
  // Let "Root" be the top of the overweight violation decision tree (see p.30)
  // Root -> Middle
  if (uxxl->GetWeight() == 0) {
    // Root -> Middle -> Left
    if (uxx->GetWeight() == 0) {
      // Root -> Middle -> Left -> Left
      if (uxx_is_right) {
        // Root -> Middle -> Left -> Left -> Left
        if (uxl->GetWeight() == 0) {
          //TODO: weakLLX(uxl);
          return BLK(u, ux, uxl, uxx);

        // Root -> Middle -> Left -> Left -> Right
        } else {
          assert(uxl->GetWeight() > 0);
          //TODO: weakLLX(uxxl);
          return RB2_R(u, ux, uxx, uxxl);
        }

      // Root -> Middle -> Left -> Right
      } else {
        assert(!uxx_is_right);
        // Root -> Middle -> Left -> Right -> Left
        if (uxr->GetWeight() == 0) {
          //TODO: weakLLX(uxr);
          return BLK(u, ux, uxx, uxr);

        // Root -> Middle -> Left -> Right -> Right
        } else {
          assert(uxr->GetWeight() > 0);
          return RB1_L(u, ux, uxx);
        }
      }

    // Root -> Middle -> Right
    } else {
      assert(uxx->GetWeight() > 0);
      //TODO: weakLLX(uxxl);
      // Note: we know that 'uxxl' is not a leaf because it has weight 0.
      NodePtr uxxlr = uxxl->GetRight(); VERIFY_ADDRESS(uxxlr);
      //TODO: weakLLX(uxxlr);

      // Root -> Middle -> Right -> Left
      if (uxxlr->GetWeight() == 0) {
        return RB2_L(ux, uxx, uxxl, uxxlr);

      // Root -> Middle -> Right -> Middle
      } else if (uxxlr->GetWeight() == 1) {
        NodePtr uxxlrl = uxxlr->GetLeft(); VERIFY_ADDRESS(uxxlrl);
        NodePtr uxxlrr = uxxlr->GetRight(); VERIFY_ADDRESS(uxxlrr);
        if (uxxlrl == NULL) return false;

        // Root -> Middle -> Right -> Middle -> Left
        if (uxxlrl->GetWeight() == 0) {
          //TODO: weakLLX(uxxlrl);
          return W4_R(ux, uxx, uxxl, uxxr, uxxlr, uxxlrl);

        // Root -> Middle -> Right -> Middle -> Right
        } else {
          assert(uxxlrl->GetWeight() > 0);
          // Root -> Middle -> Right -> Middle -> Right -> Left
          if (uxxlrr->GetWeight() == 0) {
            //TODO: weakLLX(uxxlrr);
            return W3_R(ux, uxx, uxxl, uxxr, uxxlr, uxxlrr);

          // Root -> Middle -> Right -> Middle -> Right -> Right
          } else {
            assert(uxxlrr->GetWeight() > 0);
            return W2_R(ux, uxx, uxxl, uxxr, uxxlr);
          }
        }

      // Root -> Middle -> Right -> Right
      } else {
        assert(uxxlr->GetWeight() > 1);
        return W1_R(ux, uxx, uxxl, uxxr, uxxlr);
      }
    }

  // Root -> Right
  } else if (uxxl->GetWeight() == 1) {
    //TODO: weakLLX(uxxl);
    NodePtr uxxll = uxxl->GetLeft(); VERIFY_ADDRESS(uxxll);
    NodePtr uxxlr = uxxl->GetRight(); VERIFY_ADDRESS(uxxlr);
    if (uxxll == NULL) return false;

    // Root -> Right -> Left
    if (uxxll->GetWeight() == 0) {
      //TODO: weakLLX(uxxll);
      return W5_R(ux, uxx, uxxl, uxxr, uxxll);

    // Root -> Right -> Right
    } else {
      assert(uxxll->GetWeight() > 0);
      // Root -> Right -> Right -> Left
      if (uxxlr->GetWeight() == 0) {
        //TODO: weakLLX(uxxlr);
        return W6_R(ux, uxx, uxxl, uxxr, uxxlr);

      // Root -> Right -> Right -> Right
      } else {
        assert(uxxlr->GetWeight() > 0);
        return PUSH_R(ux, uxx, uxxl, uxxr);
      }
    }

  // Root -> Left
  } else {
    assert(uxxl->GetWeight() > 1);
    //TODO: weakLLX(uxxl);
    return W7(ux, uxx, uxxl, uxxr);
  }
}

} // namespace containers
} // namespace embb

#endif // EMBB_CONTAINERS_INTERNAL_LOCK_FREE_CHROMATIC_TREE_INL_H_
