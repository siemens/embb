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

//
// This file was created automatically by a code generator.
// Any direct changes will be lost after rebuild of the project.
//

#ifndef EMBB_CONTAINERS_INTERNAL_LOCK_FREE_CHROMATIC_TREE_REBALANCE_H_
#define EMBB_CONTAINERS_INTERNAL_LOCK_FREE_CHROMATIC_TREE_REBALANCE_H_

embb_errors_t BLK(
    HazardNodePtr& u, UniqueLock& u_lock,
    HazardNodePtr& ux, UniqueLock& ux_lock,
    HazardNodePtr& uxl, UniqueLock& uxl_lock,
    HazardNodePtr& uxr, UniqueLock& uxr_lock) {
  Node* nxl = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      1,
      uxl->GetLeft(), uxl->GetRight());
  Node* nxr = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      1,
      uxr->GetLeft(), uxr->GetRight());
  Node* nx = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      IsSentinel(u) ? 1 : ux->GetWeight() - 1,
      nxl, nxr);

  if (nxl == NULL || 
      nxr == NULL || 
      nx == NULL) {
    if (nxl) FreeNode(nxl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
    return EMBB_NOMEM;
  }

  bool rotation_succeeded = u->ReplaceChild(ux, nx);
  assert(rotation_succeeded); // For now (FGL tree) this CAS may not fail
  if (!rotation_succeeded) return EMBB_BUSY;

  RetireHazardousNode(ux, ux_lock);
  RetireHazardousNode(uxl, uxl_lock);
  RetireHazardousNode(uxr, uxr_lock);
  (void)u_lock; // For now (FGL tree) u_lock is not used here

  return EMBB_SUCCESS;
}

embb_errors_t PUSH_L(
    HazardNodePtr& u, UniqueLock& u_lock,
    HazardNodePtr& ux, UniqueLock& ux_lock,
    HazardNodePtr& uxl, UniqueLock& uxl_lock,
    HazardNodePtr& uxr, UniqueLock& uxr_lock) {
  Node* nxl = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      uxl->GetWeight() - 1,
      uxl->GetLeft(), uxl->GetRight());
  Node* nxr = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      0,
      uxr->GetLeft(), uxr->GetRight());
  Node* nx = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      IsSentinel(u) ? 1 : ux->GetWeight() + 1,
      nxl, nxr);

  if (nxl == NULL || 
      nxr == NULL || 
      nx == NULL) {
    if (nxl) FreeNode(nxl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
    return EMBB_NOMEM;
  }

  bool rotation_succeeded = u->ReplaceChild(ux, nx);
  assert(rotation_succeeded); // For now (FGL tree) this CAS may not fail
  if (!rotation_succeeded) return EMBB_BUSY;

  RetireHazardousNode(ux, ux_lock);
  RetireHazardousNode(uxl, uxl_lock);
  RetireHazardousNode(uxr, uxr_lock);
  (void)u_lock; // For now (FGL tree) u_lock is not used here

  return EMBB_SUCCESS;
}

embb_errors_t PUSH_R(
    HazardNodePtr& u, UniqueLock& u_lock,
    HazardNodePtr& ux, UniqueLock& ux_lock,
    HazardNodePtr& uxl, UniqueLock& uxl_lock,
    HazardNodePtr& uxr, UniqueLock& uxr_lock) {
  Node* nxr = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      uxr->GetWeight() - 1,
      uxr->GetLeft(), uxr->GetRight());
  Node* nxl = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      0,
      uxl->GetLeft(), uxl->GetRight());
  Node* nx = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      IsSentinel(u) ? 1 : ux->GetWeight() + 1,
      nxl, nxr);

  if (nxr == NULL || 
      nxl == NULL || 
      nx == NULL) {
    if (nxr) FreeNode(nxr);
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
    return EMBB_NOMEM;
  }

  bool rotation_succeeded = u->ReplaceChild(ux, nx);
  assert(rotation_succeeded); // For now (FGL tree) this CAS may not fail
  if (!rotation_succeeded) return EMBB_BUSY;

  RetireHazardousNode(ux, ux_lock);
  RetireHazardousNode(uxl, uxl_lock);
  RetireHazardousNode(uxr, uxr_lock);
  (void)u_lock; // For now (FGL tree) u_lock is not used here

  return EMBB_SUCCESS;
}

embb_errors_t RB1_L(
    HazardNodePtr& u, UniqueLock& u_lock,
    HazardNodePtr& ux, UniqueLock& ux_lock,
    HazardNodePtr& uxl, UniqueLock& uxl_lock) {
  Node* nxr = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      0,
      uxl->GetRight(), ux->GetRight());
  Node* nx = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      ux->GetWeight(),
      uxl->GetLeft(), nxr);

  if (nxr == NULL || 
      nx == NULL) {
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
    return EMBB_NOMEM;
  }

  bool rotation_succeeded = u->ReplaceChild(ux, nx);
  assert(rotation_succeeded); // For now (FGL tree) this CAS may not fail
  if (!rotation_succeeded) return EMBB_BUSY;

  RetireHazardousNode(ux, ux_lock);
  RetireHazardousNode(uxl, uxl_lock);
  (void)u_lock; // For now (FGL tree) u_lock is not used here

  return EMBB_SUCCESS;
}

embb_errors_t RB1_R(
    HazardNodePtr& u, UniqueLock& u_lock,
    HazardNodePtr& ux, UniqueLock& ux_lock,
    HazardNodePtr& uxr, UniqueLock& uxr_lock) {
  Node* nxl = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      0,
      ux->GetLeft(), uxr->GetLeft());
  Node* nx = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      ux->GetWeight(),
      nxl, uxr->GetRight());

  if (nxl == NULL || 
      nx == NULL) {
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
    return EMBB_NOMEM;
  }

  bool rotation_succeeded = u->ReplaceChild(ux, nx);
  assert(rotation_succeeded); // For now (FGL tree) this CAS may not fail
  if (!rotation_succeeded) return EMBB_BUSY;

  RetireHazardousNode(ux, ux_lock);
  RetireHazardousNode(uxr, uxr_lock);
  (void)u_lock; // For now (FGL tree) u_lock is not used here

  return EMBB_SUCCESS;
}

embb_errors_t RB2_L(
    HazardNodePtr& u, UniqueLock& u_lock,
    HazardNodePtr& ux, UniqueLock& ux_lock,
    HazardNodePtr& uxl, UniqueLock& uxl_lock,
    HazardNodePtr& uxlr, UniqueLock& uxlr_lock) {
  Node* nxl = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      0,
      uxl->GetLeft(), uxlr->GetLeft());
  Node* nxr = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      0,
      uxlr->GetRight(), ux->GetRight());
  Node* nx = node_pool_.Allocate(
      uxlr->GetKey(), uxlr->GetValue(),
      ux->GetWeight(),
      nxl, nxr);

  if (nxl == NULL || 
      nxr == NULL || 
      nx == NULL) {
    if (nxl) FreeNode(nxl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
    return EMBB_NOMEM;
  }

  bool rotation_succeeded = u->ReplaceChild(ux, nx);
  assert(rotation_succeeded); // For now (FGL tree) this CAS may not fail
  if (!rotation_succeeded) return EMBB_BUSY;

  RetireHazardousNode(ux, ux_lock);
  RetireHazardousNode(uxl, uxl_lock);
  RetireHazardousNode(uxlr, uxlr_lock);
  (void)u_lock; // For now (FGL tree) u_lock is not used here

  return EMBB_SUCCESS;
}

embb_errors_t RB2_R(
    HazardNodePtr& u, UniqueLock& u_lock,
    HazardNodePtr& ux, UniqueLock& ux_lock,
    HazardNodePtr& uxr, UniqueLock& uxr_lock,
    HazardNodePtr& uxrl, UniqueLock& uxrl_lock) {
  Node* nxr = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      0,
      uxrl->GetRight(), uxr->GetRight());
  Node* nxl = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      0,
      ux->GetLeft(), uxrl->GetLeft());
  Node* nx = node_pool_.Allocate(
      uxrl->GetKey(), uxrl->GetValue(),
      ux->GetWeight(),
      nxl, nxr);

  if (nxr == NULL || 
      nxl == NULL || 
      nx == NULL) {
    if (nxr) FreeNode(nxr);
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
    return EMBB_NOMEM;
  }

  bool rotation_succeeded = u->ReplaceChild(ux, nx);
  assert(rotation_succeeded); // For now (FGL tree) this CAS may not fail
  if (!rotation_succeeded) return EMBB_BUSY;

  RetireHazardousNode(ux, ux_lock);
  RetireHazardousNode(uxr, uxr_lock);
  RetireHazardousNode(uxrl, uxrl_lock);
  (void)u_lock; // For now (FGL tree) u_lock is not used here

  return EMBB_SUCCESS;
}

embb_errors_t W1_L(
    HazardNodePtr& u, UniqueLock& u_lock,
    HazardNodePtr& ux, UniqueLock& ux_lock,
    HazardNodePtr& uxl, UniqueLock& uxl_lock,
    HazardNodePtr& uxr, UniqueLock& uxr_lock,
    HazardNodePtr& uxrl, UniqueLock& uxrl_lock) {
  Node* nxll = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      uxl->GetWeight() - 1,
      uxl->GetLeft(), uxl->GetRight());
  Node* nxlr = node_pool_.Allocate(
      uxrl->GetKey(), uxrl->GetValue(),
      uxrl->GetWeight() - 1,
      uxrl->GetLeft(), uxrl->GetRight());
  Node* nxl = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      1,
      nxll, nxlr);
  Node* nx = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      ux->GetWeight(),
      nxl, uxr->GetRight());

  if (nxll == NULL || 
      nxlr == NULL || 
      nxl == NULL || 
      nx == NULL) {
    if (nxll) FreeNode(nxll);
    if (nxlr) FreeNode(nxlr);
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
    return EMBB_NOMEM;
  }

  bool rotation_succeeded = u->ReplaceChild(ux, nx);
  assert(rotation_succeeded); // For now (FGL tree) this CAS may not fail
  if (!rotation_succeeded) return EMBB_BUSY;

  RetireHazardousNode(ux, ux_lock);
  RetireHazardousNode(uxl, uxl_lock);
  RetireHazardousNode(uxr, uxr_lock);
  RetireHazardousNode(uxrl, uxrl_lock);
  (void)u_lock; // For now (FGL tree) u_lock is not used here

  return EMBB_SUCCESS;
}

embb_errors_t W1_R(
    HazardNodePtr& u, UniqueLock& u_lock,
    HazardNodePtr& ux, UniqueLock& ux_lock,
    HazardNodePtr& uxl, UniqueLock& uxl_lock,
    HazardNodePtr& uxr, UniqueLock& uxr_lock,
    HazardNodePtr& uxlr, UniqueLock& uxlr_lock) {
  Node* nxrr = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      uxr->GetWeight() - 1,
      uxr->GetLeft(), uxr->GetRight());
  Node* nxrl = node_pool_.Allocate(
      uxlr->GetKey(), uxlr->GetValue(),
      uxlr->GetWeight() - 1,
      uxlr->GetLeft(), uxlr->GetRight());
  Node* nxr = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      1,
      nxrl, nxrr);
  Node* nx = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      ux->GetWeight(),
      uxl->GetLeft(), nxr);

  if (nxrr == NULL || 
      nxrl == NULL || 
      nxr == NULL || 
      nx == NULL) {
    if (nxrr) FreeNode(nxrr);
    if (nxrl) FreeNode(nxrl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
    return EMBB_NOMEM;
  }

  bool rotation_succeeded = u->ReplaceChild(ux, nx);
  assert(rotation_succeeded); // For now (FGL tree) this CAS may not fail
  if (!rotation_succeeded) return EMBB_BUSY;

  RetireHazardousNode(ux, ux_lock);
  RetireHazardousNode(uxl, uxl_lock);
  RetireHazardousNode(uxr, uxr_lock);
  RetireHazardousNode(uxlr, uxlr_lock);
  (void)u_lock; // For now (FGL tree) u_lock is not used here

  return EMBB_SUCCESS;
}

embb_errors_t W2_L(
    HazardNodePtr& u, UniqueLock& u_lock,
    HazardNodePtr& ux, UniqueLock& ux_lock,
    HazardNodePtr& uxl, UniqueLock& uxl_lock,
    HazardNodePtr& uxr, UniqueLock& uxr_lock,
    HazardNodePtr& uxrl, UniqueLock& uxrl_lock) {
  Node* nxll = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      uxl->GetWeight() - 1,
      uxl->GetLeft(), uxl->GetRight());
  Node* nxlr = node_pool_.Allocate(
      uxrl->GetKey(), uxrl->GetValue(),
      0,
      uxrl->GetLeft(), uxrl->GetRight());
  Node* nxl = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      1,
      nxll, nxlr);
  Node* nx = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      ux->GetWeight(),
      nxl, uxr->GetRight());

  if (nxll == NULL || 
      nxlr == NULL || 
      nxl == NULL || 
      nx == NULL) {
    if (nxll) FreeNode(nxll);
    if (nxlr) FreeNode(nxlr);
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
    return EMBB_NOMEM;
  }

  bool rotation_succeeded = u->ReplaceChild(ux, nx);
  assert(rotation_succeeded); // For now (FGL tree) this CAS may not fail
  if (!rotation_succeeded) return EMBB_BUSY;

  RetireHazardousNode(ux, ux_lock);
  RetireHazardousNode(uxl, uxl_lock);
  RetireHazardousNode(uxr, uxr_lock);
  RetireHazardousNode(uxrl, uxrl_lock);
  (void)u_lock; // For now (FGL tree) u_lock is not used here

  return EMBB_SUCCESS;
}

embb_errors_t W2_R(
    HazardNodePtr& u, UniqueLock& u_lock,
    HazardNodePtr& ux, UniqueLock& ux_lock,
    HazardNodePtr& uxl, UniqueLock& uxl_lock,
    HazardNodePtr& uxr, UniqueLock& uxr_lock,
    HazardNodePtr& uxlr, UniqueLock& uxlr_lock) {
  Node* nxrr = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      uxr->GetWeight() - 1,
      uxr->GetLeft(), uxr->GetRight());
  Node* nxrl = node_pool_.Allocate(
      uxlr->GetKey(), uxlr->GetValue(),
      0,
      uxlr->GetLeft(), uxlr->GetRight());
  Node* nxr = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      1,
      nxrl, nxrr);
  Node* nx = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      ux->GetWeight(),
      uxl->GetLeft(), nxr);

  if (nxrr == NULL || 
      nxrl == NULL || 
      nxr == NULL || 
      nx == NULL) {
    if (nxrr) FreeNode(nxrr);
    if (nxrl) FreeNode(nxrl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
    return EMBB_NOMEM;
  }

  bool rotation_succeeded = u->ReplaceChild(ux, nx);
  assert(rotation_succeeded); // For now (FGL tree) this CAS may not fail
  if (!rotation_succeeded) return EMBB_BUSY;

  RetireHazardousNode(ux, ux_lock);
  RetireHazardousNode(uxl, uxl_lock);
  RetireHazardousNode(uxr, uxr_lock);
  RetireHazardousNode(uxlr, uxlr_lock);
  (void)u_lock; // For now (FGL tree) u_lock is not used here

  return EMBB_SUCCESS;
}

embb_errors_t W3_L(
    HazardNodePtr& u, UniqueLock& u_lock,
    HazardNodePtr& ux, UniqueLock& ux_lock,
    HazardNodePtr& uxl, UniqueLock& uxl_lock,
    HazardNodePtr& uxr, UniqueLock& uxr_lock,
    HazardNodePtr& uxrl, UniqueLock& uxrl_lock,
    HazardNodePtr& uxrll, UniqueLock& uxrll_lock) {
  Node* nxlll = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      uxl->GetWeight() - 1,
      uxl->GetLeft(), uxl->GetRight());
  Node* nxll = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      1,
      nxlll, uxrll->GetLeft());
  Node* nxlr = node_pool_.Allocate(
      uxrl->GetKey(), uxrl->GetValue(),
      1,
      uxrll->GetRight(), uxrl->GetRight());
  Node* nxl = node_pool_.Allocate(
      uxrll->GetKey(), uxrll->GetValue(),
      0,
      nxll, nxlr);
  Node* nx = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      ux->GetWeight(),
      nxl, uxr->GetRight());

  if (nxlll == NULL || 
      nxll == NULL || 
      nxlr == NULL || 
      nxl == NULL || 
      nx == NULL) {
    if (nxlll) FreeNode(nxlll);
    if (nxll) FreeNode(nxll);
    if (nxlr) FreeNode(nxlr);
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
    return EMBB_NOMEM;
  }

  bool rotation_succeeded = u->ReplaceChild(ux, nx);
  assert(rotation_succeeded); // For now (FGL tree) this CAS may not fail
  if (!rotation_succeeded) return EMBB_BUSY;

  RetireHazardousNode(ux, ux_lock);
  RetireHazardousNode(uxl, uxl_lock);
  RetireHazardousNode(uxr, uxr_lock);
  RetireHazardousNode(uxrl, uxrl_lock);
  RetireHazardousNode(uxrll, uxrll_lock);
  (void)u_lock; // For now (FGL tree) u_lock is not used here

  return EMBB_SUCCESS;
}

embb_errors_t W3_R(
    HazardNodePtr& u, UniqueLock& u_lock,
    HazardNodePtr& ux, UniqueLock& ux_lock,
    HazardNodePtr& uxl, UniqueLock& uxl_lock,
    HazardNodePtr& uxr, UniqueLock& uxr_lock,
    HazardNodePtr& uxlr, UniqueLock& uxlr_lock,
    HazardNodePtr& uxlrr, UniqueLock& uxlrr_lock) {
  Node* nxrrr = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      uxr->GetWeight() - 1,
      uxr->GetLeft(), uxr->GetRight());
  Node* nxrr = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      1,
      uxlrr->GetRight(), nxrrr);
  Node* nxrl = node_pool_.Allocate(
      uxlr->GetKey(), uxlr->GetValue(),
      1,
      uxlr->GetLeft(), uxlrr->GetLeft());
  Node* nxr = node_pool_.Allocate(
      uxlrr->GetKey(), uxlrr->GetValue(),
      0,
      nxrl, nxrr);
  Node* nx = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      ux->GetWeight(),
      uxl->GetLeft(), nxr);

  if (nxrrr == NULL || 
      nxrr == NULL || 
      nxrl == NULL || 
      nxr == NULL || 
      nx == NULL) {
    if (nxrrr) FreeNode(nxrrr);
    if (nxrr) FreeNode(nxrr);
    if (nxrl) FreeNode(nxrl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
    return EMBB_NOMEM;
  }

  bool rotation_succeeded = u->ReplaceChild(ux, nx);
  assert(rotation_succeeded); // For now (FGL tree) this CAS may not fail
  if (!rotation_succeeded) return EMBB_BUSY;

  RetireHazardousNode(ux, ux_lock);
  RetireHazardousNode(uxl, uxl_lock);
  RetireHazardousNode(uxr, uxr_lock);
  RetireHazardousNode(uxlr, uxlr_lock);
  RetireHazardousNode(uxlrr, uxlrr_lock);
  (void)u_lock; // For now (FGL tree) u_lock is not used here

  return EMBB_SUCCESS;
}

embb_errors_t W4_L(
    HazardNodePtr& u, UniqueLock& u_lock,
    HazardNodePtr& ux, UniqueLock& ux_lock,
    HazardNodePtr& uxl, UniqueLock& uxl_lock,
    HazardNodePtr& uxr, UniqueLock& uxr_lock,
    HazardNodePtr& uxrl, UniqueLock& uxrl_lock,
    HazardNodePtr& uxrlr, UniqueLock& uxrlr_lock) {
  Node* nxll = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      uxl->GetWeight() - 1,
      uxl->GetLeft(), uxl->GetRight());
  Node* nxrl = node_pool_.Allocate(
      uxrlr->GetKey(), uxrlr->GetValue(),
      1,
      uxrlr->GetLeft(), uxrlr->GetRight());
  Node* nxl = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      1,
      nxll, uxrl->GetLeft());
  Node* nxr = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      0,
      nxrl, uxr->GetRight());
  Node* nx = node_pool_.Allocate(
      uxrl->GetKey(), uxrl->GetValue(),
      ux->GetWeight(),
      nxl, nxr);

  if (nxll == NULL || 
      nxrl == NULL || 
      nxl == NULL || 
      nxr == NULL || 
      nx == NULL) {
    if (nxll) FreeNode(nxll);
    if (nxrl) FreeNode(nxrl);
    if (nxl) FreeNode(nxl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
    return EMBB_NOMEM;
  }

  bool rotation_succeeded = u->ReplaceChild(ux, nx);
  assert(rotation_succeeded); // For now (FGL tree) this CAS may not fail
  if (!rotation_succeeded) return EMBB_BUSY;

  RetireHazardousNode(ux, ux_lock);
  RetireHazardousNode(uxl, uxl_lock);
  RetireHazardousNode(uxr, uxr_lock);
  RetireHazardousNode(uxrl, uxrl_lock);
  RetireHazardousNode(uxrlr, uxrlr_lock);
  (void)u_lock; // For now (FGL tree) u_lock is not used here

  return EMBB_SUCCESS;
}

embb_errors_t W4_R(
    HazardNodePtr& u, UniqueLock& u_lock,
    HazardNodePtr& ux, UniqueLock& ux_lock,
    HazardNodePtr& uxl, UniqueLock& uxl_lock,
    HazardNodePtr& uxr, UniqueLock& uxr_lock,
    HazardNodePtr& uxlr, UniqueLock& uxlr_lock,
    HazardNodePtr& uxlrl, UniqueLock& uxlrl_lock) {
  Node* nxrr = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      uxr->GetWeight() - 1,
      uxr->GetLeft(), uxr->GetRight());
  Node* nxlr = node_pool_.Allocate(
      uxlrl->GetKey(), uxlrl->GetValue(),
      1,
      uxlrl->GetLeft(), uxlrl->GetRight());
  Node* nxr = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      1,
      uxlr->GetRight(), nxrr);
  Node* nxl = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      0,
      uxl->GetLeft(), nxlr);
  Node* nx = node_pool_.Allocate(
      uxlr->GetKey(), uxlr->GetValue(),
      ux->GetWeight(),
      nxl, nxr);

  if (nxrr == NULL || 
      nxlr == NULL || 
      nxr == NULL || 
      nxl == NULL || 
      nx == NULL) {
    if (nxrr) FreeNode(nxrr);
    if (nxlr) FreeNode(nxlr);
    if (nxr) FreeNode(nxr);
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
    return EMBB_NOMEM;
  }

  bool rotation_succeeded = u->ReplaceChild(ux, nx);
  assert(rotation_succeeded); // For now (FGL tree) this CAS may not fail
  if (!rotation_succeeded) return EMBB_BUSY;

  RetireHazardousNode(ux, ux_lock);
  RetireHazardousNode(uxl, uxl_lock);
  RetireHazardousNode(uxr, uxr_lock);
  RetireHazardousNode(uxlr, uxlr_lock);
  RetireHazardousNode(uxlrl, uxlrl_lock);
  (void)u_lock; // For now (FGL tree) u_lock is not used here

  return EMBB_SUCCESS;
}

embb_errors_t W5_L(
    HazardNodePtr& u, UniqueLock& u_lock,
    HazardNodePtr& ux, UniqueLock& ux_lock,
    HazardNodePtr& uxl, UniqueLock& uxl_lock,
    HazardNodePtr& uxr, UniqueLock& uxr_lock,
    HazardNodePtr& uxrr, UniqueLock& uxrr_lock) {
  Node* nxll = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      uxl->GetWeight() - 1,
      uxl->GetLeft(), uxl->GetRight());
  Node* nxl = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      1,
      nxll, uxr->GetLeft());
  Node* nxr = node_pool_.Allocate(
      uxrr->GetKey(), uxrr->GetValue(),
      1,
      uxrr->GetLeft(), uxrr->GetRight());
  Node* nx = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      ux->GetWeight(),
      nxl, nxr);

  if (nxll == NULL || 
      nxl == NULL || 
      nxr == NULL || 
      nx == NULL) {
    if (nxll) FreeNode(nxll);
    if (nxl) FreeNode(nxl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
    return EMBB_NOMEM;
  }

  bool rotation_succeeded = u->ReplaceChild(ux, nx);
  assert(rotation_succeeded); // For now (FGL tree) this CAS may not fail
  if (!rotation_succeeded) return EMBB_BUSY;

  RetireHazardousNode(ux, ux_lock);
  RetireHazardousNode(uxl, uxl_lock);
  RetireHazardousNode(uxr, uxr_lock);
  RetireHazardousNode(uxrr, uxrr_lock);
  (void)u_lock; // For now (FGL tree) u_lock is not used here

  return EMBB_SUCCESS;
}

embb_errors_t W5_R(
    HazardNodePtr& u, UniqueLock& u_lock,
    HazardNodePtr& ux, UniqueLock& ux_lock,
    HazardNodePtr& uxl, UniqueLock& uxl_lock,
    HazardNodePtr& uxr, UniqueLock& uxr_lock,
    HazardNodePtr& uxll, UniqueLock& uxll_lock) {
  Node* nxrr = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      uxr->GetWeight() - 1,
      uxr->GetLeft(), uxr->GetRight());
  Node* nxr = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      1,
      uxl->GetRight(), nxrr);
  Node* nxl = node_pool_.Allocate(
      uxll->GetKey(), uxll->GetValue(),
      1,
      uxll->GetLeft(), uxll->GetRight());
  Node* nx = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      ux->GetWeight(),
      nxl, nxr);

  if (nxrr == NULL || 
      nxr == NULL || 
      nxl == NULL || 
      nx == NULL) {
    if (nxrr) FreeNode(nxrr);
    if (nxr) FreeNode(nxr);
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
    return EMBB_NOMEM;
  }

  bool rotation_succeeded = u->ReplaceChild(ux, nx);
  assert(rotation_succeeded); // For now (FGL tree) this CAS may not fail
  if (!rotation_succeeded) return EMBB_BUSY;

  RetireHazardousNode(ux, ux_lock);
  RetireHazardousNode(uxl, uxl_lock);
  RetireHazardousNode(uxr, uxr_lock);
  RetireHazardousNode(uxll, uxll_lock);
  (void)u_lock; // For now (FGL tree) u_lock is not used here

  return EMBB_SUCCESS;
}

embb_errors_t W6_L(
    HazardNodePtr& u, UniqueLock& u_lock,
    HazardNodePtr& ux, UniqueLock& ux_lock,
    HazardNodePtr& uxl, UniqueLock& uxl_lock,
    HazardNodePtr& uxr, UniqueLock& uxr_lock,
    HazardNodePtr& uxrl, UniqueLock& uxrl_lock) {
  Node* nxll = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      uxl->GetWeight() - 1,
      uxl->GetLeft(), uxl->GetRight());
  Node* nxl = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      1,
      nxll, uxrl->GetLeft());
  Node* nxr = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      1,
      uxrl->GetRight(), uxr->GetRight());
  Node* nx = node_pool_.Allocate(
      uxrl->GetKey(), uxrl->GetValue(),
      ux->GetWeight(),
      nxl, nxr);

  if (nxll == NULL || 
      nxl == NULL || 
      nxr == NULL || 
      nx == NULL) {
    if (nxll) FreeNode(nxll);
    if (nxl) FreeNode(nxl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
    return EMBB_NOMEM;
  }

  bool rotation_succeeded = u->ReplaceChild(ux, nx);
  assert(rotation_succeeded); // For now (FGL tree) this CAS may not fail
  if (!rotation_succeeded) return EMBB_BUSY;

  RetireHazardousNode(ux, ux_lock);
  RetireHazardousNode(uxl, uxl_lock);
  RetireHazardousNode(uxr, uxr_lock);
  RetireHazardousNode(uxrl, uxrl_lock);
  (void)u_lock; // For now (FGL tree) u_lock is not used here

  return EMBB_SUCCESS;
}

embb_errors_t W6_R(
    HazardNodePtr& u, UniqueLock& u_lock,
    HazardNodePtr& ux, UniqueLock& ux_lock,
    HazardNodePtr& uxl, UniqueLock& uxl_lock,
    HazardNodePtr& uxr, UniqueLock& uxr_lock,
    HazardNodePtr& uxlr, UniqueLock& uxlr_lock) {
  Node* nxrr = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      uxr->GetWeight() - 1,
      uxr->GetLeft(), uxr->GetRight());
  Node* nxr = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      1,
      uxlr->GetRight(), nxrr);
  Node* nxl = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      1,
      uxl->GetLeft(), uxlr->GetLeft());
  Node* nx = node_pool_.Allocate(
      uxlr->GetKey(), uxlr->GetValue(),
      ux->GetWeight(),
      nxl, nxr);

  if (nxrr == NULL || 
      nxr == NULL || 
      nxl == NULL || 
      nx == NULL) {
    if (nxrr) FreeNode(nxrr);
    if (nxr) FreeNode(nxr);
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
    return EMBB_NOMEM;
  }

  bool rotation_succeeded = u->ReplaceChild(ux, nx);
  assert(rotation_succeeded); // For now (FGL tree) this CAS may not fail
  if (!rotation_succeeded) return EMBB_BUSY;

  RetireHazardousNode(ux, ux_lock);
  RetireHazardousNode(uxl, uxl_lock);
  RetireHazardousNode(uxr, uxr_lock);
  RetireHazardousNode(uxlr, uxlr_lock);
  (void)u_lock; // For now (FGL tree) u_lock is not used here

  return EMBB_SUCCESS;
}

embb_errors_t W7(
    HazardNodePtr& u, UniqueLock& u_lock,
    HazardNodePtr& ux, UniqueLock& ux_lock,
    HazardNodePtr& uxl, UniqueLock& uxl_lock,
    HazardNodePtr& uxr, UniqueLock& uxr_lock) {
  Node* nxl = node_pool_.Allocate(
      uxl->GetKey(), uxl->GetValue(),
      uxl->GetWeight() - 1,
      uxl->GetLeft(), uxl->GetRight());
  Node* nxr = node_pool_.Allocate(
      uxr->GetKey(), uxr->GetValue(),
      uxr->GetWeight() - 1,
      uxr->GetLeft(), uxr->GetRight());
  Node* nx = node_pool_.Allocate(
      ux->GetKey(), ux->GetValue(),
      IsSentinel(u) ? 1 : ux->GetWeight() + 1,
      nxl, nxr);

  if (nxl == NULL || 
      nxr == NULL || 
      nx == NULL) {
    if (nxl) FreeNode(nxl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
    return EMBB_NOMEM;
  }

  bool rotation_succeeded = u->ReplaceChild(ux, nx);
  assert(rotation_succeeded); // For now (FGL tree) this CAS may not fail
  if (!rotation_succeeded) return EMBB_BUSY;

  RetireHazardousNode(ux, ux_lock);
  RetireHazardousNode(uxl, uxl_lock);
  RetireHazardousNode(uxr, uxr_lock);
  (void)u_lock; // For now (FGL tree) u_lock is not used here

  return EMBB_SUCCESS;
}

#endif // EMBB_CONTAINERS_INTERNAL_LOCK_FREE_CHROMATIC_TREE_REBALANCE_H_
