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

#ifndef EMBB_CONTAINERS_LOCK_FREE_CHROMATIC_TREE_REBALANCE_H_
#define EMBB_CONTAINERS_LOCK_FREE_CHROMATIC_TREE_REBALANCE_H_

bool BLK(const NodePtr& u,
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
    if (nxl) FreeNode(nxl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
    return false;
  }

  NodePtr expected = ux;
  GetPointerToChild(u, ux).CompareAndSwap(expected, nx);

  RetireNode(ux);
  RetireNode(uxl);
  RetireNode(uxr);

  return true;
}

bool PUSH_L(const NodePtr& u,
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
    if (nxl) FreeNode(nxl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
    return false;
  }

  NodePtr expected = ux;
  GetPointerToChild(u, ux).CompareAndSwap(expected, nx);

  RetireNode(ux);
  RetireNode(uxl);
  RetireNode(uxr);

  return true;
}

bool PUSH_R(const NodePtr& u,
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
    if (nxr) FreeNode(nxr);
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
    return false;
  }

  NodePtr expected = ux;
  GetPointerToChild(u, ux).CompareAndSwap(expected, nx);

  RetireNode(ux);
  RetireNode(uxl);
  RetireNode(uxr);

  return true;
}

bool RB1_L(const NodePtr& u,
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
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
    return false;
  }

  NodePtr expected = ux;
  GetPointerToChild(u, ux).CompareAndSwap(expected, nx);

  RetireNode(ux);
  RetireNode(uxl);

  return true;
}

bool RB1_R(const NodePtr& u,
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
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
    return false;
  }

  NodePtr expected = ux;
  GetPointerToChild(u, ux).CompareAndSwap(expected, nx);

  RetireNode(ux);
  RetireNode(uxr);

  return true;
}

bool RB2_L(const NodePtr& u,
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
    if (nxl) FreeNode(nxl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
    return false;
  }

  NodePtr expected = ux;
  GetPointerToChild(u, ux).CompareAndSwap(expected, nx);

  RetireNode(ux);
  RetireNode(uxl);
  RetireNode(uxlr);

  return true;
}

bool RB2_R(const NodePtr& u,
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
    if (nxr) FreeNode(nxr);
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
    return false;
  }

  NodePtr expected = ux;
  GetPointerToChild(u, ux).CompareAndSwap(expected, nx);

  RetireNode(ux);
  RetireNode(uxr);
  RetireNode(uxrl);

  return true;
}

bool W1_L(const NodePtr& u,
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
    if (nxll) FreeNode(nxll);
    if (nxlr) FreeNode(nxlr);
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
    return false;
  }

  NodePtr expected = ux;
  GetPointerToChild(u, ux).CompareAndSwap(expected, nx);

  RetireNode(ux);
  RetireNode(uxl);
  RetireNode(uxr);
  RetireNode(uxrl);

  return true;
}

bool W1_R(const NodePtr& u,
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
    if (nxrr) FreeNode(nxrr);
    if (nxrl) FreeNode(nxrl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
    return false;
  }

  NodePtr expected = ux;
  GetPointerToChild(u, ux).CompareAndSwap(expected, nx);

  RetireNode(ux);
  RetireNode(uxl);
  RetireNode(uxr);
  RetireNode(uxlr);

  return true;
}

bool W2_L(const NodePtr& u,
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
    if (nxll) FreeNode(nxll);
    if (nxlr) FreeNode(nxlr);
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
    return false;
  }

  NodePtr expected = ux;
  GetPointerToChild(u, ux).CompareAndSwap(expected, nx);

  RetireNode(ux);
  RetireNode(uxl);
  RetireNode(uxr);
  RetireNode(uxrl);

  return true;
}

bool W2_R(const NodePtr& u,
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
    if (nxrr) FreeNode(nxrr);
    if (nxrl) FreeNode(nxrl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
    return false;
  }

  NodePtr expected = ux;
  GetPointerToChild(u, ux).CompareAndSwap(expected, nx);

  RetireNode(ux);
  RetireNode(uxl);
  RetireNode(uxr);
  RetireNode(uxlr);

  return true;
}

bool W3_L(const NodePtr& u,
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
    if (nxlll) FreeNode(nxlll);
    if (nxll) FreeNode(nxll);
    if (nxlr) FreeNode(nxlr);
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
    return false;
  }

  NodePtr expected = ux;
  GetPointerToChild(u, ux).CompareAndSwap(expected, nx);

  RetireNode(ux);
  RetireNode(uxl);
  RetireNode(uxr);
  RetireNode(uxrl);
  RetireNode(uxrll);

  return true;
}

bool W3_R(const NodePtr& u,
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
    if (nxrrr) FreeNode(nxrrr);
    if (nxrr) FreeNode(nxrr);
    if (nxrl) FreeNode(nxrl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
    return false;
  }

  NodePtr expected = ux;
  GetPointerToChild(u, ux).CompareAndSwap(expected, nx);

  RetireNode(ux);
  RetireNode(uxl);
  RetireNode(uxr);
  RetireNode(uxlr);
  RetireNode(uxlrr);

  return true;
}

bool W4_L(const NodePtr& u,
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
    if (nxll) FreeNode(nxll);
    if (nxrl) FreeNode(nxrl);
    if (nxl) FreeNode(nxl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
    return false;
  }

  NodePtr expected = ux;
  GetPointerToChild(u, ux).CompareAndSwap(expected, nx);

  RetireNode(ux);
  RetireNode(uxl);
  RetireNode(uxr);
  RetireNode(uxrl);
  RetireNode(uxrlr);

  return true;
}

bool W4_R(const NodePtr& u,
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
    if (nxrr) FreeNode(nxrr);
    if (nxlr) FreeNode(nxlr);
    if (nxr) FreeNode(nxr);
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
    return false;
  }

  NodePtr expected = ux;
  GetPointerToChild(u, ux).CompareAndSwap(expected, nx);

  RetireNode(ux);
  RetireNode(uxl);
  RetireNode(uxr);
  RetireNode(uxlr);
  RetireNode(uxlrl);

  return true;
}

bool W5_L(const NodePtr& u,
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
    if (nxll) FreeNode(nxll);
    if (nxl) FreeNode(nxl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
    return false;
  }

  NodePtr expected = ux;
  GetPointerToChild(u, ux).CompareAndSwap(expected, nx);

  RetireNode(ux);
  RetireNode(uxl);
  RetireNode(uxr);
  RetireNode(uxrr);

  return true;
}

bool W5_R(const NodePtr& u,
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
    if (nxrr) FreeNode(nxrr);
    if (nxr) FreeNode(nxr);
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
    return false;
  }

  NodePtr expected = ux;
  GetPointerToChild(u, ux).CompareAndSwap(expected, nx);

  RetireNode(ux);
  RetireNode(uxl);
  RetireNode(uxr);
  RetireNode(uxll);

  return true;
}

bool W6_L(const NodePtr& u,
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
    if (nxll) FreeNode(nxll);
    if (nxl) FreeNode(nxl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
    return false;
  }

  NodePtr expected = ux;
  GetPointerToChild(u, ux).CompareAndSwap(expected, nx);

  RetireNode(ux);
  RetireNode(uxl);
  RetireNode(uxr);
  RetireNode(uxrl);

  return true;
}

bool W6_R(const NodePtr& u,
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
    if (nxrr) FreeNode(nxrr);
    if (nxr) FreeNode(nxr);
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
    return false;
  }

  NodePtr expected = ux;
  GetPointerToChild(u, ux).CompareAndSwap(expected, nx);

  RetireNode(ux);
  RetireNode(uxl);
  RetireNode(uxr);
  RetireNode(uxlr);

  return true;
}

bool W7(const NodePtr& u,
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
    if (nxl) FreeNode(nxl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
    return false;
  }

  NodePtr expected = ux;
  GetPointerToChild(u, ux).CompareAndSwap(expected, nx);

  RetireNode(ux);
  RetireNode(uxl);
  RetireNode(uxr);

  return true;
}

#endif // EMBB_CONTAINERS_LOCK_FREE_CHROMATIC_TREE_REBALANCE_H_
