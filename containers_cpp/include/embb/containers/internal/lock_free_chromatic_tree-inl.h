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

namespace embb {
namespace containers {
namespace internal {

template<typename Key, typename Value>
ChromaticTreeNode<Key, Value>::
ChromaticTreeNode(const Key& key, const Value& value, const int& weight,
                  ChromaticTreeNode<Key, Value>* const & left,
                  ChromaticTreeNode<Key, Value>* const & right)
    : key_(key),
      value_(value),
      weight_(weight),
      left_(left),
      right_(right) {}

template<typename Key, typename Value>
ChromaticTreeNode<Key, Value>::
ChromaticTreeNode(const Key& key, const Value& value)
    : key_(key),
      value_(value),
      weight_(1),
      left_(NULL),
      right_(NULL) {}

template<typename Key, typename Value>
ChromaticTreeNode<Key, Value>::
ChromaticTreeNode(const ChromaticTreeNode& other)
    : key_(other.key_),
      value_(other.value_),
      weight_(other.weight_),
      left_(other.left_),
      right_(other.right_) {}

template<typename Key, typename Value>
const Key& ChromaticTreeNode<Key, Value>::GetKey() const {
  return key_;
}

template<typename Key, typename Value>
const Value& ChromaticTreeNode<Key, Value>::GetValue() const {
  return value_;
}

template<typename Key, typename Value>
const int& ChromaticTreeNode<Key, Value>::GetWeight() const {
  return weight_;
}

template<typename Key, typename Value>
ChromaticTreeNode<Key, Value>*& ChromaticTreeNode<Key, Value>::GetLeft() {
  return left_;
}

template<typename Key, typename Value>
ChromaticTreeNode<Key, Value>*& ChromaticTreeNode<Key, Value>::GetRight() {
  return right_;
}

} // namespace internal


template<typename Key, typename Value, typename Compare, typename NodePool>
ChromaticTree<Key, Value, Compare, NodePool>::
ChromaticTree(size_t capacity, Key undefined_key, Value undefined_value,
              Compare compare)
    : undefined_key_(undefined_key),
      undefined_value_(undefined_value),
      compare_(compare),
      capacity_(capacity),
      node_pool_(2 + 5 + 2 * capacity_) {
  entry_ = node_pool_.Allocate(undefined_key_, undefined_value_);
  NodePtr sentinel = node_pool_.Allocate(undefined_key_, undefined_value_);
  entry_->GetLeft() = sentinel;
}

template<typename Key, typename Value, typename Compare, typename NodePool>
ChromaticTree<Key, Value, Compare, NodePool>::
~ChromaticTree() {
  Destruct(entry_->GetLeft());
  node_pool_.Free(entry_);
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
Get(const Key& key, Value& value) {
  ReaderLockGuard guard(readwrite_lock_.GetReader());

  NodePtr leaf;
  Search(key, leaf);

  bool keys_are_equal = !(compare_(key, leaf->GetKey()) ||
                          compare_(leaf->GetKey(), key));
  if (!IsSentinel(leaf) && keys_are_equal) {
    value = leaf->GetValue();
    return true;
  } else {
    return false;
  }
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
  WriterLockGuard guard(readwrite_lock_.GetWriter());

  NodePtr leaf, parent;
  Search(key, leaf, parent);

  assert(HasChild(parent, leaf));

  NodePtr new_parent;
  bool added_violation = false;

  NodePtr new_leaf = node_pool_.Allocate(key, value);
  if (new_leaf == NULL) {
    return false;
  }

  bool keys_are_equal = !(compare_(key, leaf->GetKey()) ||
                          compare_(leaf->GetKey(), key));
  if (!IsSentinel(leaf) && keys_are_equal) {
    old_value = leaf->GetValue();
    new_parent = new_leaf;
  } else {
    old_value = undefined_value_;

    NodePtr new_sibling = node_pool_.Allocate(*leaf);
    if (new_sibling == NULL) {
      node_pool_.Free(new_leaf);
      return false;
    }

    int new_weight = (HasFixedWeight(leaf)) ? 1 : (leaf->GetWeight() - 1);
    if (IsSentinel(leaf) || compare_(key, leaf->GetKey())) {
      new_parent = node_pool_.Allocate(
          leaf->GetKey(), undefined_value_, new_weight, new_leaf, new_sibling);
    } else {
      new_parent = node_pool_.Allocate(
          key, undefined_value_, new_weight, new_sibling, new_leaf);
    }

    if (new_parent == NULL) {
      node_pool_.Free(new_leaf);
      node_pool_.Free(new_sibling);
      return false;
    }

    added_violation = (parent->GetWeight() == 0 && new_weight == 0);
  }

  NodePtr& new_subtree_ptr = GetPointerToChild(parent, leaf);
  new_subtree_ptr = new_parent;

  node_pool_.Free(leaf);

  if (added_violation) {
    CleanUp(key);
  }

  return true;
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
  WriterLockGuard guard(readwrite_lock_.GetWriter());

  NodePtr leaf, parent, grandparent;
  Search(key, leaf, parent, grandparent);

  bool keys_are_equal = !(compare_(key, leaf->GetKey()) ||
                          compare_(leaf->GetKey(), key));
  if (IsSentinel(leaf) || !keys_are_equal) {
    old_value = undefined_value_;
    return false;
  }

  assert(HasChild(grandparent, parent));
  assert(HasChild(parent, leaf));

  NodePtr sibling = ((parent->GetLeft() == leaf) ?
                     parent->GetRight() : parent->GetLeft());

  int new_weight = (HasFixedWeight(parent)) ?
                1 : (parent->GetWeight() + sibling->GetWeight());
  bool added_violation = (new_weight > 1);

  NodePtr new_leaf = node_pool_.Allocate(
      sibling->GetKey(), sibling->GetValue(), new_weight,
      sibling->GetLeft(), sibling->GetRight());
  assert((new_leaf != NULL) && "No nodes available for replacement!");

  NodePtr& new_subtree_ptr = GetPointerToChild(grandparent, parent);
  new_subtree_ptr = new_leaf;

  old_value = leaf->GetValue();

  node_pool_.Free(leaf);
  node_pool_.Free(sibling);
  node_pool_.Free(parent);

  if (added_violation) {
    CleanUp(key);
  }

  return true;
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
Search(const Key& key, NodePtr& leaf) const {
  NodePtr parent;
  Search(key, leaf, parent);
}

template<typename Key, typename Value, typename Compare, typename NodePool>
void ChromaticTree<Key, Value, Compare, NodePool>::
Search(const Key& key, NodePtr& leaf, NodePtr& parent) const {
  NodePtr grandparent;
  Search(key, leaf, parent, grandparent);
}

template<typename Key, typename Value, typename Compare, typename NodePool>
void ChromaticTree<Key, Value, Compare, NodePool>::
Search(const Key& key, NodePtr& leaf, NodePtr& parent,
       NodePtr& grandparent) const {
  grandparent = NULL;
  parent      = entry_;
  leaf        = entry_->GetLeft();

  while (!IsLeaf(leaf)) {
    grandparent = parent;
    parent      = leaf;
    leaf        = (IsSentinel(leaf) || compare_(key, leaf->GetKey())) ?
                  leaf->GetLeft() : leaf->GetRight();
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
typename ChromaticTree<Key, Value, Compare, NodePool>::NodePtr&
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
  node_pool_.Free(node);
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
CleanUp(const Key& key) {
  while (true) {
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
  NodePtr uxl = ux->GetLeft();
  NodePtr uxr = ux->GetRight();
  bool uxx_is_left = (uxx == uxl);
  if (!HasChild(ux, uxx)) return false;

  //TODO: weakLLX(uxx);
  NodePtr uxxl = uxx->GetLeft();
  NodePtr uxxr = uxx->GetRight();
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
      NodePtr uxxrl = uxxr->GetLeft();
      //TODO: weakLLX(uxxrl);

      // Root -> Middle -> Right -> Left
      if (uxxrl->GetWeight() == 0) {
        return RB2_R(ux, uxx, uxxr, uxxrl);

      // Root -> Middle -> Right -> Middle
      } else if (uxxrl->GetWeight() == 1) {
        NodePtr uxxrll = uxxrl->GetLeft();
        NodePtr uxxrlr = uxxrl->GetRight();
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
    NodePtr uxxrl = uxxr->GetLeft();
    NodePtr uxxrr = uxxr->GetRight();
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

  return true;
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
      NodePtr uxxlr = uxxl->GetRight();
      //TODO: weakLLX(uxxlr);

      // Root -> Middle -> Right -> Left
      if (uxxlr->GetWeight() == 0) {
        return RB2_L(ux, uxx, uxxl, uxxlr);

      // Root -> Middle -> Right -> Middle
      } else if (uxxlr->GetWeight() == 1) {
        NodePtr uxxlrl = uxxlr->GetLeft();
        NodePtr uxxlrr = uxxlr->GetRight();
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
    NodePtr uxxll = uxxl->GetLeft();
    NodePtr uxxlr = uxxl->GetRight();
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

  return true;
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
BLK(const NodePtr& u,
    const NodePtr& ux,
    const NodePtr& uxl,
    const NodePtr& uxr) {
  NodePtr nxl = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      1,
      uxl->GetLeft(), uxl->GetRight());
  NodePtr nxr = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      1,
      uxr->GetLeft(), uxr->GetRight());
  NodePtr nx = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      HasFixedWeight(ux) ? 1 : ux->GetWeight() - 1,
      nxl, nxr);

  if (nxl == NULL ||
      nxr == NULL ||
      nx == NULL) {
    if (nxl) node_pool_.Free(nxl);
    if (nxr) node_pool_.Free(nxr);
    if (nx) node_pool_.Free(nx);
    return false;
  }

  NodePtr& new_subtree_ptr = GetPointerToChild(u, ux);
  new_subtree_ptr = nx;

  node_pool_.Free(ux);
  node_pool_.Free(uxl);
  node_pool_.Free(uxr);

  return true;
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
PUSH_L(const NodePtr& u,
       const NodePtr& ux,
       const NodePtr& uxl,
       const NodePtr& uxr) {
  NodePtr nxl = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      uxl->GetWeight() - 1,
      uxl->GetLeft(), uxl->GetRight());
  NodePtr nxr = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      0,
      uxr->GetLeft(), uxr->GetRight());
  NodePtr nx = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      HasFixedWeight(ux) ? 1 : ux->GetWeight() + 1,
      nxl, nxr);

  if (nxl == NULL ||
      nxr == NULL ||
      nx == NULL) {
    if (nxl) node_pool_.Free(nxl);
    if (nxr) node_pool_.Free(nxr);
    if (nx) node_pool_.Free(nx);
    return false;
  }

  NodePtr& new_subtree_ptr = GetPointerToChild(u, ux);
  new_subtree_ptr = nx;

  node_pool_.Free(ux);
  node_pool_.Free(uxl);
  node_pool_.Free(uxr);

  return true;
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
PUSH_R(const NodePtr& u,
       const NodePtr& ux,
       const NodePtr& uxl,
       const NodePtr& uxr) {
  NodePtr nxr = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      uxr->GetWeight() - 1,
      uxr->GetLeft(), uxr->GetRight());
  NodePtr nxl = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      0,
      uxl->GetLeft(), uxl->GetRight());
  NodePtr nx = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      HasFixedWeight(ux) ? 1 : ux->GetWeight() + 1,
      nxl, nxr);

  if (nxr == NULL ||
      nxl == NULL ||
      nx == NULL) {
    if (nxr) node_pool_.Free(nxr);
    if (nxl) node_pool_.Free(nxl);
    if (nx) node_pool_.Free(nx);
    return false;
  }

  NodePtr& new_subtree_ptr = GetPointerToChild(u, ux);
  new_subtree_ptr = nx;

  node_pool_.Free(ux);
  node_pool_.Free(uxl);
  node_pool_.Free(uxr);

  return true;
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
RB1_L(const NodePtr& u,
      const NodePtr& ux,
      const NodePtr& uxl) {
  NodePtr nxr = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      0,
      uxl->GetRight(), ux->GetRight());
  NodePtr nx = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      ux->GetWeight(),
      uxl->GetLeft(), nxr);

  if (nxr == NULL ||
      nx == NULL) {
    if (nxr) node_pool_.Free(nxr);
    if (nx) node_pool_.Free(nx);
    return false;
  }

  NodePtr& new_subtree_ptr = GetPointerToChild(u, ux);
  new_subtree_ptr = nx;

  node_pool_.Free(ux);
  node_pool_.Free(uxl);

  return true;
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
RB1_R(const NodePtr& u,
      const NodePtr& ux,
      const NodePtr& uxr) {
  NodePtr nxl = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      0,
      ux->GetLeft(), uxr->GetLeft());
  NodePtr nx = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      ux->GetWeight(),
      nxl, uxr->GetRight());

  if (nxl == NULL ||
      nx == NULL) {
    if (nxl) node_pool_.Free(nxl);
    if (nx) node_pool_.Free(nx);
    return false;
  }

  NodePtr& new_subtree_ptr = GetPointerToChild(u, ux);
  new_subtree_ptr = nx;

  node_pool_.Free(ux);
  node_pool_.Free(uxr);

  return true;
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
RB2_L(const NodePtr& u,
      const NodePtr& ux,
      const NodePtr& uxl,
      const NodePtr& uxlr) {
  NodePtr nxl = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      0,
      uxl->GetLeft(), uxlr->GetLeft());
  NodePtr nxr = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      0,
      uxlr->GetRight(), ux->GetRight());
  NodePtr nx = node_pool_.Allocate(
      uxlr->GetKey(), uxlr->GetValue(),
      ux->GetWeight(),
      nxl, nxr);

  if (nxl == NULL ||
      nxr == NULL ||
      nx == NULL) {
    if (nxl) node_pool_.Free(nxl);
    if (nxr) node_pool_.Free(nxr);
    if (nx) node_pool_.Free(nx);
    return false;
  }

  NodePtr& new_subtree_ptr = GetPointerToChild(u, ux);
  new_subtree_ptr = nx;

  node_pool_.Free(ux);
  node_pool_.Free(uxl);
  node_pool_.Free(uxlr);

  return true;
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
RB2_R(const NodePtr& u,
      const NodePtr& ux,
      const NodePtr& uxr,
      const NodePtr& uxrl) {
  NodePtr nxr = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      0,
      uxrl->GetRight(), uxr->GetRight());
  NodePtr nxl = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      0,
      ux->GetLeft(), uxrl->GetLeft());
  NodePtr nx = node_pool_.Allocate(
      uxrl->GetKey(), uxrl->GetValue(),
      ux->GetWeight(),
      nxl, nxr);

  if (nxr == NULL ||
      nxl == NULL ||
      nx == NULL) {
    if (nxr) node_pool_.Free(nxr);
    if (nxl) node_pool_.Free(nxl);
    if (nx) node_pool_.Free(nx);
    return false;
  }

  NodePtr& new_subtree_ptr = GetPointerToChild(u, ux);
  new_subtree_ptr = nx;

  node_pool_.Free(ux);
  node_pool_.Free(uxr);
  node_pool_.Free(uxrl);

  return true;
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
W1_L(const NodePtr& u,
     const NodePtr& ux,
     const NodePtr& uxl,
     const NodePtr& uxr,
     const NodePtr& uxrl) {
  NodePtr nxll = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      uxl->GetWeight() - 1,
      uxl->GetLeft(), uxl->GetRight());
  NodePtr nxlr = node_pool_.Allocate(
      uxrl->GetKey(), uxrl->GetValue(),
      uxrl->GetWeight() - 1,
      uxrl->GetLeft(), uxrl->GetRight());
  NodePtr nxl = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      1,
      nxll, nxlr);
  NodePtr nx = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      ux->GetWeight(),
      nxl, uxr->GetRight());

  if (nxll == NULL ||
      nxlr == NULL ||
      nxl == NULL ||
      nx == NULL) {
    if (nxll) node_pool_.Free(nxll);
    if (nxlr) node_pool_.Free(nxlr);
    if (nxl) node_pool_.Free(nxl);
    if (nx) node_pool_.Free(nx);
    return false;
  }

  NodePtr& new_subtree_ptr = GetPointerToChild(u, ux);
  new_subtree_ptr = nx;

  node_pool_.Free(ux);
  node_pool_.Free(uxl);
  node_pool_.Free(uxr);
  node_pool_.Free(uxrl);

  return true;
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
W1_R(const NodePtr& u,
     const NodePtr& ux,
     const NodePtr& uxl,
     const NodePtr& uxr,
     const NodePtr& uxlr) {
  NodePtr nxrr = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      uxr->GetWeight() - 1,
      uxr->GetLeft(), uxr->GetRight());
  NodePtr nxrl = node_pool_.Allocate(
      uxlr->GetKey(), uxlr->GetValue(),
      uxlr->GetWeight() - 1,
      uxlr->GetLeft(), uxlr->GetRight());
  NodePtr nxr = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      1,
      nxrl, nxrr);
  NodePtr nx = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      ux->GetWeight(),
      uxl->GetLeft(), nxr);

  if (nxrr == NULL ||
      nxrl == NULL ||
      nxr == NULL ||
      nx == NULL) {
    if (nxrr) node_pool_.Free(nxrr);
    if (nxrl) node_pool_.Free(nxrl);
    if (nxr) node_pool_.Free(nxr);
    if (nx) node_pool_.Free(nx);
    return false;
  }

  NodePtr& new_subtree_ptr = GetPointerToChild(u, ux);
  new_subtree_ptr = nx;

  node_pool_.Free(ux);
  node_pool_.Free(uxl);
  node_pool_.Free(uxr);
  node_pool_.Free(uxlr);

  return true;
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
W2_L(const NodePtr& u,
     const NodePtr& ux,
     const NodePtr& uxl,
     const NodePtr& uxr,
     const NodePtr& uxrl) {
  NodePtr nxll = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      uxl->GetWeight() - 1,
      uxl->GetLeft(), uxl->GetRight());
  NodePtr nxlr = node_pool_.Allocate(
      uxrl->GetKey(), uxrl->GetValue(),
      0,
      uxrl->GetLeft(), uxrl->GetRight());
  NodePtr nxl = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      1,
      nxll, nxlr);
  NodePtr nx = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      ux->GetWeight(),
      nxl, uxr->GetRight());

  if (nxll == NULL ||
      nxlr == NULL ||
      nxl == NULL ||
      nx == NULL) {
    if (nxll) node_pool_.Free(nxll);
    if (nxlr) node_pool_.Free(nxlr);
    if (nxl) node_pool_.Free(nxl);
    if (nx) node_pool_.Free(nx);
    return false;
  }

  NodePtr& new_subtree_ptr = GetPointerToChild(u, ux);
  new_subtree_ptr = nx;

  node_pool_.Free(ux);
  node_pool_.Free(uxl);
  node_pool_.Free(uxr);
  node_pool_.Free(uxrl);

  return true;
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
W2_R(const NodePtr& u,
     const NodePtr& ux,
     const NodePtr& uxl,
     const NodePtr& uxr,
     const NodePtr& uxlr) {
  NodePtr nxrr = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      uxr->GetWeight() - 1,
      uxr->GetLeft(), uxr->GetRight());
  NodePtr nxrl = node_pool_.Allocate(
      uxlr->GetKey(), uxlr->GetValue(),
      0,
      uxlr->GetLeft(), uxlr->GetRight());
  NodePtr nxr = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      1,
      nxrl, nxrr);
  NodePtr nx = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      ux->GetWeight(),
      uxl->GetLeft(), nxr);

  if (nxrr == NULL ||
      nxrl == NULL ||
      nxr == NULL ||
      nx == NULL) {
    if (nxrr) node_pool_.Free(nxrr);
    if (nxrl) node_pool_.Free(nxrl);
    if (nxr) node_pool_.Free(nxr);
    if (nx) node_pool_.Free(nx);
    return false;
  }

  NodePtr& new_subtree_ptr = GetPointerToChild(u, ux);
  new_subtree_ptr = nx;

  node_pool_.Free(ux);
  node_pool_.Free(uxl);
  node_pool_.Free(uxr);
  node_pool_.Free(uxlr);

  return true;
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
W3_L(const NodePtr& u,
     const NodePtr& ux,
     const NodePtr& uxl,
     const NodePtr& uxr,
     const NodePtr& uxrl,
     const NodePtr& uxrll) {
  NodePtr nxlll = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      uxl->GetWeight() - 1,
      uxl->GetLeft(), uxl->GetRight());
  NodePtr nxll = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      1,
      nxlll, uxrll->GetLeft());
  NodePtr nxlr = node_pool_.Allocate(
      uxrl->GetKey(), uxrl->GetValue(),
      1,
      uxrll->GetRight(), uxrl->GetRight());
  NodePtr nxl = node_pool_.Allocate(
      uxrll->GetKey(), uxrll->GetValue(),
      0,
      nxll, nxlr);
  NodePtr nx = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      ux->GetWeight(),
      nxl, uxr->GetRight());

  if (nxlll == NULL ||
      nxll == NULL ||
      nxlr == NULL ||
      nxl == NULL ||
      nx == NULL) {
    if (nxlll) node_pool_.Free(nxlll);
    if (nxll) node_pool_.Free(nxll);
    if (nxlr) node_pool_.Free(nxlr);
    if (nxl) node_pool_.Free(nxl);
    if (nx) node_pool_.Free(nx);
    return false;
  }

  NodePtr& new_subtree_ptr = GetPointerToChild(u, ux);
  new_subtree_ptr = nx;

  node_pool_.Free(ux);
  node_pool_.Free(uxl);
  node_pool_.Free(uxr);
  node_pool_.Free(uxrl);
  node_pool_.Free(uxrll);

  return true;
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
W3_R(const NodePtr& u,
     const NodePtr& ux,
     const NodePtr& uxl,
     const NodePtr& uxr,
     const NodePtr& uxlr,
     const NodePtr& uxlrr) {
  NodePtr nxrrr = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      uxr->GetWeight() - 1,
      uxr->GetLeft(), uxr->GetRight());
  NodePtr nxrr = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      1,
      uxlrr->GetRight(), nxrrr);
  NodePtr nxrl = node_pool_.Allocate(
      uxlr->GetKey(), uxlr->GetValue(),
      1,
      uxlr->GetLeft(), uxlrr->GetLeft());
  NodePtr nxr = node_pool_.Allocate(
      uxlrr->GetKey(), uxlrr->GetValue(),
      0,
      nxrl, nxrr);
  NodePtr nx = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      ux->GetWeight(),
      uxl->GetLeft(), nxr);

  if (nxrrr == NULL ||
      nxrr == NULL ||
      nxrl == NULL ||
      nxr == NULL ||
      nx == NULL) {
    if (nxrrr) node_pool_.Free(nxrrr);
    if (nxrr) node_pool_.Free(nxrr);
    if (nxrl) node_pool_.Free(nxrl);
    if (nxr) node_pool_.Free(nxr);
    if (nx) node_pool_.Free(nx);
    return false;
  }

  NodePtr& new_subtree_ptr = GetPointerToChild(u, ux);
  new_subtree_ptr = nx;

  node_pool_.Free(ux);
  node_pool_.Free(uxl);
  node_pool_.Free(uxr);
  node_pool_.Free(uxlr);
  node_pool_.Free(uxlrr);

  return true;
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
W4_L(const NodePtr& u,
     const NodePtr& ux,
     const NodePtr& uxl,
     const NodePtr& uxr,
     const NodePtr& uxrl,
     const NodePtr& uxrlr) {
  NodePtr nxll = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      uxl->GetWeight() - 1,
      uxl->GetLeft(), uxl->GetRight());
  NodePtr nxrl = node_pool_.Allocate(
      uxrlr->GetKey(), uxrlr->GetValue(),
      1,
      uxrlr->GetLeft(), uxrlr->GetRight());
  NodePtr nxl = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      1,
      nxll, uxrl->GetLeft());
  NodePtr nxr = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      0,
      nxrl, uxr->GetRight());
  NodePtr nx = node_pool_.Allocate(
      uxrl->GetKey(), uxrl->GetValue(),
      ux->GetWeight(),
      nxl, nxr);

  if (nxll == NULL ||
      nxrl == NULL ||
      nxl == NULL ||
      nxr == NULL ||
      nx == NULL) {
    if (nxll) node_pool_.Free(nxll);
    if (nxrl) node_pool_.Free(nxrl);
    if (nxl) node_pool_.Free(nxl);
    if (nxr) node_pool_.Free(nxr);
    if (nx) node_pool_.Free(nx);
    return false;
  }

  NodePtr& new_subtree_ptr = GetPointerToChild(u, ux);
  new_subtree_ptr = nx;

  node_pool_.Free(ux);
  node_pool_.Free(uxl);
  node_pool_.Free(uxr);
  node_pool_.Free(uxrl);
  node_pool_.Free(uxrlr);

  return true;
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
W4_R(const NodePtr& u,
     const NodePtr& ux,
     const NodePtr& uxl,
     const NodePtr& uxr,
     const NodePtr& uxlr,
     const NodePtr& uxlrl) {
  NodePtr nxrr = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      uxr->GetWeight() - 1,
      uxr->GetLeft(), uxr->GetRight());
  NodePtr nxlr = node_pool_.Allocate(
      uxlrl->GetKey(), uxlrl->GetValue(),
      1,
      uxlrl->GetLeft(), uxlrl->GetRight());
  NodePtr nxr = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      1,
      uxlr->GetRight(), nxrr);
  NodePtr nxl = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      0,
      uxl->GetLeft(), nxlr);
  NodePtr nx = node_pool_.Allocate(
      uxlr->GetKey(), uxlr->GetValue(),
      ux->GetWeight(),
      nxl, nxr);

  if (nxrr == NULL ||
      nxlr == NULL ||
      nxr == NULL ||
      nxl == NULL ||
      nx == NULL) {
    if (nxrr) node_pool_.Free(nxrr);
    if (nxlr) node_pool_.Free(nxlr);
    if (nxr) node_pool_.Free(nxr);
    if (nxl) node_pool_.Free(nxl);
    if (nx) node_pool_.Free(nx);
    return false;
  }

  NodePtr& new_subtree_ptr = GetPointerToChild(u, ux);
  new_subtree_ptr = nx;

  node_pool_.Free(ux);
  node_pool_.Free(uxl);
  node_pool_.Free(uxr);
  node_pool_.Free(uxlr);
  node_pool_.Free(uxlrl);

  return true;
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
W5_L(const NodePtr& u,
     const NodePtr& ux,
     const NodePtr& uxl,
     const NodePtr& uxr,
     const NodePtr& uxrr) {
  NodePtr nxll = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      uxl->GetWeight() - 1,
      uxl->GetLeft(), uxl->GetRight());
  NodePtr nxl = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      1,
      nxll, uxr->GetLeft());
  NodePtr nxr = node_pool_.Allocate(
      uxrr->GetKey(), uxrr->GetValue(),
      1,
      uxrr->GetLeft(), uxrr->GetRight());
  NodePtr nx = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      ux->GetWeight(),
      nxl, nxr);

  if (nxll == NULL ||
      nxl == NULL ||
      nxr == NULL ||
      nx == NULL) {
    if (nxll) node_pool_.Free(nxll);
    if (nxl) node_pool_.Free(nxl);
    if (nxr) node_pool_.Free(nxr);
    if (nx) node_pool_.Free(nx);
    return false;
  }

  NodePtr& new_subtree_ptr = GetPointerToChild(u, ux);
  new_subtree_ptr = nx;

  node_pool_.Free(ux);
  node_pool_.Free(uxl);
  node_pool_.Free(uxr);
  node_pool_.Free(uxrr);

  return true;
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
W5_R(const NodePtr& u,
     const NodePtr& ux,
     const NodePtr& uxl,
     const NodePtr& uxr,
     const NodePtr& uxll) {
  NodePtr nxrr = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      uxr->GetWeight() - 1,
      uxr->GetLeft(), uxr->GetRight());
  NodePtr nxr = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      1,
      uxl->GetRight(), nxrr);
  NodePtr nxl = node_pool_.Allocate(
      uxll->GetKey(), uxll->GetValue(),
      1,
      uxll->GetLeft(), uxll->GetRight());
  NodePtr nx = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      ux->GetWeight(),
      nxl, nxr);

  if (nxrr == NULL ||
      nxr == NULL ||
      nxl == NULL ||
      nx == NULL) {
    if (nxrr) node_pool_.Free(nxrr);
    if (nxr) node_pool_.Free(nxr);
    if (nxl) node_pool_.Free(nxl);
    if (nx) node_pool_.Free(nx);
    return false;
  }

  NodePtr& new_subtree_ptr = GetPointerToChild(u, ux);
  new_subtree_ptr = nx;

  node_pool_.Free(ux);
  node_pool_.Free(uxl);
  node_pool_.Free(uxr);
  node_pool_.Free(uxll);

  return true;
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
W6_L(const NodePtr& u,
     const NodePtr& ux,
     const NodePtr& uxl,
     const NodePtr& uxr,
     const NodePtr& uxrl) {
  NodePtr nxll = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      uxl->GetWeight() - 1,
      uxl->GetLeft(), uxl->GetRight());
  NodePtr nxl = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      1,
      nxll, uxrl->GetLeft());
  NodePtr nxr = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      1,
      uxrl->GetRight(), uxr->GetRight());
  NodePtr nx = node_pool_.Allocate(
      uxrl->GetKey(), uxrl->GetValue(),
      ux->GetWeight(),
      nxl, nxr);

  if (nxll == NULL ||
      nxl == NULL ||
      nxr == NULL ||
      nx == NULL) {
    if (nxll) node_pool_.Free(nxll);
    if (nxl) node_pool_.Free(nxl);
    if (nxr) node_pool_.Free(nxr);
    if (nx) node_pool_.Free(nx);
    return false;
  }

  NodePtr& new_subtree_ptr = GetPointerToChild(u, ux);
  new_subtree_ptr = nx;

  node_pool_.Free(ux);
  node_pool_.Free(uxl);
  node_pool_.Free(uxr);
  node_pool_.Free(uxrl);

  return true;
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
W6_R(const NodePtr& u,
     const NodePtr& ux,
     const NodePtr& uxl,
     const NodePtr& uxr,
     const NodePtr& uxlr) {
  NodePtr nxrr = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      uxr->GetWeight() - 1,
      uxr->GetLeft(), uxr->GetRight());
  NodePtr nxr = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      1,
      uxlr->GetRight(), nxrr);
  NodePtr nxl = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      1,
      uxl->GetLeft(), uxlr->GetLeft());
  NodePtr nx = node_pool_.Allocate(
      uxlr->GetKey(), uxlr->GetValue(),
      ux->GetWeight(),
      nxl, nxr);

  if (nxrr == NULL ||
      nxr == NULL ||
      nxl == NULL ||
      nx == NULL) {
    if (nxrr) node_pool_.Free(nxrr);
    if (nxr) node_pool_.Free(nxr);
    if (nxl) node_pool_.Free(nxl);
    if (nx) node_pool_.Free(nx);
    return false;
  }

  NodePtr& new_subtree_ptr = GetPointerToChild(u, ux);
  new_subtree_ptr = nx;

  node_pool_.Free(ux);
  node_pool_.Free(uxl);
  node_pool_.Free(uxr);
  node_pool_.Free(uxlr);

  return true;
}

template<typename Key, typename Value, typename Compare, typename NodePool>
bool ChromaticTree<Key, Value, Compare, NodePool>::
W7(const NodePtr& u,
   const NodePtr& ux,
   const NodePtr& uxl,
   const NodePtr& uxr) {
  NodePtr nxl = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      uxl->GetWeight() - 1,
      uxl->GetLeft(), uxl->GetRight());
  NodePtr nxr = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      uxr->GetWeight() - 1,
      uxr->GetLeft(), uxr->GetRight());
  NodePtr nx = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      HasFixedWeight(ux) ? 1 : ux->GetWeight() + 1,
      nxl, nxr);

  if (nxl == NULL ||
      nxr == NULL ||
      nx == NULL) {
    if (nxl) node_pool_.Free(nxl);
    if (nxr) node_pool_.Free(nxr);
    if (nx) node_pool_.Free(nx);
    return false;
  }

  NodePtr& new_subtree_ptr = GetPointerToChild(u, ux);
  new_subtree_ptr = nx;

  node_pool_.Free(ux);
  node_pool_.Free(uxl);
  node_pool_.Free(uxr);

  return true;
}


} // namespace containers
} // namespace embb

#endif // EMBB_CONTAINERS_INTERNAL_LOCK_FREE_CHROMATIC_TREE_INL_H_
