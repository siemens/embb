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

embb_errors_t BLK(HazardNodePtr& u, HazardOperationPtr& u_op,
                  HazardNodePtr& ux, HazardOperationPtr& ux_op,
                  HazardNodePtr& uxl, HazardOperationPtr& uxl_op,
                  HazardNodePtr& uxr, HazardOperationPtr& uxr_op) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxl;
  Node* nxr;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxl = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), 1,
        uxl->GetLeft(), uxl->GetRight(), Operation::INITIAL_DUMMY);
    if (nxl == NULL) break;
    nxr = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), 1,
        uxr->GetLeft(), uxr->GetRight(), Operation::INITIAL_DUMMY);
    if (nxr == NULL) break;
    nx = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), IsSentinel(u) ? 1 : ux->GetWeight() - 1,
        nxl, nxr, Operation::INITIAL_DUMMY);
    if (nx == NULL) break;

    HazardOperationPtr op(GetOperationGuard(HIDX_CURRENT_OP));
    op.ProtectSafe(operation_pool_.Allocate());
    if (op == NULL) break;

    op->SetRoot(u, u_op);
    op->SetNewChild(nx);
    op->SetOldNodes(ux, ux_op,
                    uxl, uxl_op,
                    uxr, uxr_op);

    bool succeeded = op->Help(GetNodeGuard(HIDX_HELPING),
                              GetOperationGuard(HIDX_HELPING));
    op->CleanUp();
    
    if (!succeeded) {
      RetireOperation(op);
      result = EMBB_BUSY;
      break;
    }

    RetireOperation(u_op);
    RetireNode(ux); RetireOperation(ux_op);
    RetireNode(uxl); RetireOperation(uxl_op);
    RetireNode(uxr); RetireOperation(uxr_op);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxl) FreeNode(nxl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t PUSH_L(HazardNodePtr& u, HazardOperationPtr& u_op,
                     HazardNodePtr& ux, HazardOperationPtr& ux_op,
                     HazardNodePtr& uxl, HazardOperationPtr& uxl_op,
                     HazardNodePtr& uxr, HazardOperationPtr& uxr_op) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxl;
  Node* nxr;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxl = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), uxl->GetWeight() - 1,
        uxl->GetLeft(), uxl->GetRight(), Operation::INITIAL_DUMMY);
    if (nxl == NULL) break;
    nxr = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), 0,
        uxr->GetLeft(), uxr->GetRight(), Operation::INITIAL_DUMMY);
    if (nxr == NULL) break;
    nx = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), IsSentinel(u) ? 1 : ux->GetWeight() + 1,
        nxl, nxr, Operation::INITIAL_DUMMY);
    if (nx == NULL) break;

    HazardOperationPtr op(GetOperationGuard(HIDX_CURRENT_OP));
    op.ProtectSafe(operation_pool_.Allocate());
    if (op == NULL) break;

    op->SetRoot(u, u_op);
    op->SetNewChild(nx);
    op->SetOldNodes(ux, ux_op,
                    uxl, uxl_op,
                    uxr, uxr_op);

    bool succeeded = op->Help(GetNodeGuard(HIDX_HELPING),
                              GetOperationGuard(HIDX_HELPING));
    op->CleanUp();
    
    if (!succeeded) {
      RetireOperation(op);
      result = EMBB_BUSY;
      break;
    }

    RetireOperation(u_op);
    RetireNode(ux); RetireOperation(ux_op);
    RetireNode(uxl); RetireOperation(uxl_op);
    RetireNode(uxr); RetireOperation(uxr_op);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxl) FreeNode(nxl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t PUSH_R(HazardNodePtr& u, HazardOperationPtr& u_op,
                     HazardNodePtr& ux, HazardOperationPtr& ux_op,
                     HazardNodePtr& uxl, HazardOperationPtr& uxl_op,
                     HazardNodePtr& uxr, HazardOperationPtr& uxr_op) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxr;
  Node* nxl;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxr = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), uxr->GetWeight() - 1,
        uxr->GetLeft(), uxr->GetRight(), Operation::INITIAL_DUMMY);
    if (nxr == NULL) break;
    nxl = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), 0,
        uxl->GetLeft(), uxl->GetRight(), Operation::INITIAL_DUMMY);
    if (nxl == NULL) break;
    nx = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), IsSentinel(u) ? 1 : ux->GetWeight() + 1,
        nxl, nxr, Operation::INITIAL_DUMMY);
    if (nx == NULL) break;

    HazardOperationPtr op(GetOperationGuard(HIDX_CURRENT_OP));
    op.ProtectSafe(operation_pool_.Allocate());
    if (op == NULL) break;

    op->SetRoot(u, u_op);
    op->SetNewChild(nx);
    op->SetOldNodes(ux, ux_op,
                    uxl, uxl_op,
                    uxr, uxr_op);

    bool succeeded = op->Help(GetNodeGuard(HIDX_HELPING),
                              GetOperationGuard(HIDX_HELPING));
    op->CleanUp();
    
    if (!succeeded) {
      RetireOperation(op);
      result = EMBB_BUSY;
      break;
    }

    RetireOperation(u_op);
    RetireNode(ux); RetireOperation(ux_op);
    RetireNode(uxl); RetireOperation(uxl_op);
    RetireNode(uxr); RetireOperation(uxr_op);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxr) FreeNode(nxr);
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t RB1_L(HazardNodePtr& u, HazardOperationPtr& u_op,
                    HazardNodePtr& ux, HazardOperationPtr& ux_op,
                    HazardNodePtr& uxl, HazardOperationPtr& uxl_op) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxr;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxr = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), 0,
        uxl->GetRight(), ux->GetRight(), Operation::INITIAL_DUMMY);
    if (nxr == NULL) break;
    nx = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), ux->GetWeight(),
        uxl->GetLeft(), nxr, Operation::INITIAL_DUMMY);
    if (nx == NULL) break;

    HazardOperationPtr op(GetOperationGuard(HIDX_CURRENT_OP));
    op.ProtectSafe(operation_pool_.Allocate());
    if (op == NULL) break;

    op->SetRoot(u, u_op);
    op->SetNewChild(nx);
    op->SetOldNodes(ux, ux_op,
                    uxl, uxl_op);

    bool succeeded = op->Help(GetNodeGuard(HIDX_HELPING),
                              GetOperationGuard(HIDX_HELPING));
    op->CleanUp();
    
    if (!succeeded) {
      RetireOperation(op);
      result = EMBB_BUSY;
      break;
    }

    RetireOperation(u_op);
    RetireNode(ux); RetireOperation(ux_op);
    RetireNode(uxl); RetireOperation(uxl_op);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t RB1_R(HazardNodePtr& u, HazardOperationPtr& u_op,
                    HazardNodePtr& ux, HazardOperationPtr& ux_op,
                    HazardNodePtr& uxr, HazardOperationPtr& uxr_op) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxl;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxl = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), 0,
        ux->GetLeft(), uxr->GetLeft(), Operation::INITIAL_DUMMY);
    if (nxl == NULL) break;
    nx = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), ux->GetWeight(),
        nxl, uxr->GetRight(), Operation::INITIAL_DUMMY);
    if (nx == NULL) break;

    HazardOperationPtr op(GetOperationGuard(HIDX_CURRENT_OP));
    op.ProtectSafe(operation_pool_.Allocate());
    if (op == NULL) break;

    op->SetRoot(u, u_op);
    op->SetNewChild(nx);
    op->SetOldNodes(ux, ux_op,
                    uxr, uxr_op);

    bool succeeded = op->Help(GetNodeGuard(HIDX_HELPING),
                              GetOperationGuard(HIDX_HELPING));
    op->CleanUp();
    
    if (!succeeded) {
      RetireOperation(op);
      result = EMBB_BUSY;
      break;
    }

    RetireOperation(u_op);
    RetireNode(ux); RetireOperation(ux_op);
    RetireNode(uxr); RetireOperation(uxr_op);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t RB2_L(HazardNodePtr& u, HazardOperationPtr& u_op,
                    HazardNodePtr& ux, HazardOperationPtr& ux_op,
                    HazardNodePtr& uxl, HazardOperationPtr& uxl_op,
                    HazardNodePtr& uxlr, HazardOperationPtr& uxlr_op) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxl;
  Node* nxr;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxl = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), 0,
        uxl->GetLeft(), uxlr->GetLeft(), Operation::INITIAL_DUMMY);
    if (nxl == NULL) break;
    nxr = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), 0,
        uxlr->GetRight(), ux->GetRight(), Operation::INITIAL_DUMMY);
    if (nxr == NULL) break;
    nx = node_pool_.Allocate(
        uxlr->GetKey(), uxlr->GetValue(), ux->GetWeight(),
        nxl, nxr, Operation::INITIAL_DUMMY);
    if (nx == NULL) break;

    HazardOperationPtr op(GetOperationGuard(HIDX_CURRENT_OP));
    op.ProtectSafe(operation_pool_.Allocate());
    if (op == NULL) break;

    op->SetRoot(u, u_op);
    op->SetNewChild(nx);
    op->SetOldNodes(ux, ux_op,
                    uxl, uxl_op,
                    uxlr, uxlr_op);

    bool succeeded = op->Help(GetNodeGuard(HIDX_HELPING),
                              GetOperationGuard(HIDX_HELPING));
    op->CleanUp();
    
    if (!succeeded) {
      RetireOperation(op);
      result = EMBB_BUSY;
      break;
    }

    RetireOperation(u_op);
    RetireNode(ux); RetireOperation(ux_op);
    RetireNode(uxl); RetireOperation(uxl_op);
    RetireNode(uxlr); RetireOperation(uxlr_op);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxl) FreeNode(nxl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t RB2_R(HazardNodePtr& u, HazardOperationPtr& u_op,
                    HazardNodePtr& ux, HazardOperationPtr& ux_op,
                    HazardNodePtr& uxr, HazardOperationPtr& uxr_op,
                    HazardNodePtr& uxrl, HazardOperationPtr& uxrl_op) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxr;
  Node* nxl;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxr = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), 0,
        uxrl->GetRight(), uxr->GetRight(), Operation::INITIAL_DUMMY);
    if (nxr == NULL) break;
    nxl = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), 0,
        ux->GetLeft(), uxrl->GetLeft(), Operation::INITIAL_DUMMY);
    if (nxl == NULL) break;
    nx = node_pool_.Allocate(
        uxrl->GetKey(), uxrl->GetValue(), ux->GetWeight(),
        nxl, nxr, Operation::INITIAL_DUMMY);
    if (nx == NULL) break;

    HazardOperationPtr op(GetOperationGuard(HIDX_CURRENT_OP));
    op.ProtectSafe(operation_pool_.Allocate());
    if (op == NULL) break;

    op->SetRoot(u, u_op);
    op->SetNewChild(nx);
    op->SetOldNodes(ux, ux_op,
                    uxr, uxr_op,
                    uxrl, uxrl_op);

    bool succeeded = op->Help(GetNodeGuard(HIDX_HELPING),
                              GetOperationGuard(HIDX_HELPING));
    op->CleanUp();
    
    if (!succeeded) {
      RetireOperation(op);
      result = EMBB_BUSY;
      break;
    }

    RetireOperation(u_op);
    RetireNode(ux); RetireOperation(ux_op);
    RetireNode(uxr); RetireOperation(uxr_op);
    RetireNode(uxrl); RetireOperation(uxrl_op);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxr) FreeNode(nxr);
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t W1_L(HazardNodePtr& u, HazardOperationPtr& u_op,
                   HazardNodePtr& ux, HazardOperationPtr& ux_op,
                   HazardNodePtr& uxl, HazardOperationPtr& uxl_op,
                   HazardNodePtr& uxr, HazardOperationPtr& uxr_op,
                   HazardNodePtr& uxrl, HazardOperationPtr& uxrl_op) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxll;
  Node* nxlr;
  Node* nxl;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxll = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), uxl->GetWeight() - 1,
        uxl->GetLeft(), uxl->GetRight(), Operation::INITIAL_DUMMY);
    if (nxll == NULL) break;
    nxlr = node_pool_.Allocate(
        uxrl->GetKey(), uxrl->GetValue(), uxrl->GetWeight() - 1,
        uxrl->GetLeft(), uxrl->GetRight(), Operation::INITIAL_DUMMY);
    if (nxlr == NULL) break;
    nxl = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), 1,
        nxll, nxlr, Operation::INITIAL_DUMMY);
    if (nxl == NULL) break;
    nx = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), ux->GetWeight(),
        nxl, uxr->GetRight(), Operation::INITIAL_DUMMY);
    if (nx == NULL) break;

    HazardOperationPtr op(GetOperationGuard(HIDX_CURRENT_OP));
    op.ProtectSafe(operation_pool_.Allocate());
    if (op == NULL) break;

    op->SetRoot(u, u_op);
    op->SetNewChild(nx);
    op->SetOldNodes(ux, ux_op,
                    uxl, uxl_op,
                    uxr, uxr_op,
                    uxrl, uxrl_op);

    bool succeeded = op->Help(GetNodeGuard(HIDX_HELPING),
                              GetOperationGuard(HIDX_HELPING));
    op->CleanUp();
    
    if (!succeeded) {
      RetireOperation(op);
      result = EMBB_BUSY;
      break;
    }

    RetireOperation(u_op);
    RetireNode(ux); RetireOperation(ux_op);
    RetireNode(uxl); RetireOperation(uxl_op);
    RetireNode(uxr); RetireOperation(uxr_op);
    RetireNode(uxrl); RetireOperation(uxrl_op);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxll) FreeNode(nxll);
    if (nxlr) FreeNode(nxlr);
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t W1_R(HazardNodePtr& u, HazardOperationPtr& u_op,
                   HazardNodePtr& ux, HazardOperationPtr& ux_op,
                   HazardNodePtr& uxl, HazardOperationPtr& uxl_op,
                   HazardNodePtr& uxr, HazardOperationPtr& uxr_op,
                   HazardNodePtr& uxlr, HazardOperationPtr& uxlr_op) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxrr;
  Node* nxrl;
  Node* nxr;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxrr = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), uxr->GetWeight() - 1,
        uxr->GetLeft(), uxr->GetRight(), Operation::INITIAL_DUMMY);
    if (nxrr == NULL) break;
    nxrl = node_pool_.Allocate(
        uxlr->GetKey(), uxlr->GetValue(), uxlr->GetWeight() - 1,
        uxlr->GetLeft(), uxlr->GetRight(), Operation::INITIAL_DUMMY);
    if (nxrl == NULL) break;
    nxr = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), 1,
        nxrl, nxrr, Operation::INITIAL_DUMMY);
    if (nxr == NULL) break;
    nx = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), ux->GetWeight(),
        uxl->GetLeft(), nxr, Operation::INITIAL_DUMMY);
    if (nx == NULL) break;

    HazardOperationPtr op(GetOperationGuard(HIDX_CURRENT_OP));
    op.ProtectSafe(operation_pool_.Allocate());
    if (op == NULL) break;

    op->SetRoot(u, u_op);
    op->SetNewChild(nx);
    op->SetOldNodes(ux, ux_op,
                    uxl, uxl_op,
                    uxr, uxr_op,
                    uxlr, uxlr_op);

    bool succeeded = op->Help(GetNodeGuard(HIDX_HELPING),
                              GetOperationGuard(HIDX_HELPING));
    op->CleanUp();
    
    if (!succeeded) {
      RetireOperation(op);
      result = EMBB_BUSY;
      break;
    }

    RetireOperation(u_op);
    RetireNode(ux); RetireOperation(ux_op);
    RetireNode(uxl); RetireOperation(uxl_op);
    RetireNode(uxr); RetireOperation(uxr_op);
    RetireNode(uxlr); RetireOperation(uxlr_op);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxrr) FreeNode(nxrr);
    if (nxrl) FreeNode(nxrl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t W2_L(HazardNodePtr& u, HazardOperationPtr& u_op,
                   HazardNodePtr& ux, HazardOperationPtr& ux_op,
                   HazardNodePtr& uxl, HazardOperationPtr& uxl_op,
                   HazardNodePtr& uxr, HazardOperationPtr& uxr_op,
                   HazardNodePtr& uxrl, HazardOperationPtr& uxrl_op) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxll;
  Node* nxlr;
  Node* nxl;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxll = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), uxl->GetWeight() - 1,
        uxl->GetLeft(), uxl->GetRight(), Operation::INITIAL_DUMMY);
    if (nxll == NULL) break;
    nxlr = node_pool_.Allocate(
        uxrl->GetKey(), uxrl->GetValue(), 0,
        uxrl->GetLeft(), uxrl->GetRight(), Operation::INITIAL_DUMMY);
    if (nxlr == NULL) break;
    nxl = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), 1,
        nxll, nxlr, Operation::INITIAL_DUMMY);
    if (nxl == NULL) break;
    nx = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), ux->GetWeight(),
        nxl, uxr->GetRight(), Operation::INITIAL_DUMMY);
    if (nx == NULL) break;

    HazardOperationPtr op(GetOperationGuard(HIDX_CURRENT_OP));
    op.ProtectSafe(operation_pool_.Allocate());
    if (op == NULL) break;

    op->SetRoot(u, u_op);
    op->SetNewChild(nx);
    op->SetOldNodes(ux, ux_op,
                    uxl, uxl_op,
                    uxr, uxr_op,
                    uxrl, uxrl_op);

    bool succeeded = op->Help(GetNodeGuard(HIDX_HELPING),
                              GetOperationGuard(HIDX_HELPING));
    op->CleanUp();
    
    if (!succeeded) {
      RetireOperation(op);
      result = EMBB_BUSY;
      break;
    }

    RetireOperation(u_op);
    RetireNode(ux); RetireOperation(ux_op);
    RetireNode(uxl); RetireOperation(uxl_op);
    RetireNode(uxr); RetireOperation(uxr_op);
    RetireNode(uxrl); RetireOperation(uxrl_op);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxll) FreeNode(nxll);
    if (nxlr) FreeNode(nxlr);
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t W2_R(HazardNodePtr& u, HazardOperationPtr& u_op,
                   HazardNodePtr& ux, HazardOperationPtr& ux_op,
                   HazardNodePtr& uxl, HazardOperationPtr& uxl_op,
                   HazardNodePtr& uxr, HazardOperationPtr& uxr_op,
                   HazardNodePtr& uxlr, HazardOperationPtr& uxlr_op) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxrr;
  Node* nxrl;
  Node* nxr;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxrr = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), uxr->GetWeight() - 1,
        uxr->GetLeft(), uxr->GetRight(), Operation::INITIAL_DUMMY);
    if (nxrr == NULL) break;
    nxrl = node_pool_.Allocate(
        uxlr->GetKey(), uxlr->GetValue(), 0,
        uxlr->GetLeft(), uxlr->GetRight(), Operation::INITIAL_DUMMY);
    if (nxrl == NULL) break;
    nxr = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), 1,
        nxrl, nxrr, Operation::INITIAL_DUMMY);
    if (nxr == NULL) break;
    nx = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), ux->GetWeight(),
        uxl->GetLeft(), nxr, Operation::INITIAL_DUMMY);
    if (nx == NULL) break;

    HazardOperationPtr op(GetOperationGuard(HIDX_CURRENT_OP));
    op.ProtectSafe(operation_pool_.Allocate());
    if (op == NULL) break;

    op->SetRoot(u, u_op);
    op->SetNewChild(nx);
    op->SetOldNodes(ux, ux_op,
                    uxl, uxl_op,
                    uxr, uxr_op,
                    uxlr, uxlr_op);

    bool succeeded = op->Help(GetNodeGuard(HIDX_HELPING),
                              GetOperationGuard(HIDX_HELPING));
    op->CleanUp();
    
    if (!succeeded) {
      RetireOperation(op);
      result = EMBB_BUSY;
      break;
    }

    RetireOperation(u_op);
    RetireNode(ux); RetireOperation(ux_op);
    RetireNode(uxl); RetireOperation(uxl_op);
    RetireNode(uxr); RetireOperation(uxr_op);
    RetireNode(uxlr); RetireOperation(uxlr_op);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxrr) FreeNode(nxrr);
    if (nxrl) FreeNode(nxrl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t W3_L(HazardNodePtr& u, HazardOperationPtr& u_op,
                   HazardNodePtr& ux, HazardOperationPtr& ux_op,
                   HazardNodePtr& uxl, HazardOperationPtr& uxl_op,
                   HazardNodePtr& uxr, HazardOperationPtr& uxr_op,
                   HazardNodePtr& uxrl, HazardOperationPtr& uxrl_op,
                   HazardNodePtr& uxrll, HazardOperationPtr& uxrll_op) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxlll;
  Node* nxll;
  Node* nxlr;
  Node* nxl;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxlll = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), uxl->GetWeight() - 1,
        uxl->GetLeft(), uxl->GetRight(), Operation::INITIAL_DUMMY);
    if (nxlll == NULL) break;
    nxll = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), 1,
        nxlll, uxrll->GetLeft(), Operation::INITIAL_DUMMY);
    if (nxll == NULL) break;
    nxlr = node_pool_.Allocate(
        uxrl->GetKey(), uxrl->GetValue(), 1,
        uxrll->GetRight(), uxrl->GetRight(), Operation::INITIAL_DUMMY);
    if (nxlr == NULL) break;
    nxl = node_pool_.Allocate(
        uxrll->GetKey(), uxrll->GetValue(), 0,
        nxll, nxlr, Operation::INITIAL_DUMMY);
    if (nxl == NULL) break;
    nx = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), ux->GetWeight(),
        nxl, uxr->GetRight(), Operation::INITIAL_DUMMY);
    if (nx == NULL) break;

    HazardOperationPtr op(GetOperationGuard(HIDX_CURRENT_OP));
    op.ProtectSafe(operation_pool_.Allocate());
    if (op == NULL) break;

    op->SetRoot(u, u_op);
    op->SetNewChild(nx);
    op->SetOldNodes(ux, ux_op,
                    uxl, uxl_op,
                    uxr, uxr_op,
                    uxrl, uxrl_op,
                    uxrll, uxrll_op);

    bool succeeded = op->Help(GetNodeGuard(HIDX_HELPING),
                              GetOperationGuard(HIDX_HELPING));
    op->CleanUp();
    
    if (!succeeded) {
      RetireOperation(op);
      result = EMBB_BUSY;
      break;
    }

    RetireOperation(u_op);
    RetireNode(ux); RetireOperation(ux_op);
    RetireNode(uxl); RetireOperation(uxl_op);
    RetireNode(uxr); RetireOperation(uxr_op);
    RetireNode(uxrl); RetireOperation(uxrl_op);
    RetireNode(uxrll); RetireOperation(uxrll_op);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxlll) FreeNode(nxlll);
    if (nxll) FreeNode(nxll);
    if (nxlr) FreeNode(nxlr);
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t W3_R(HazardNodePtr& u, HazardOperationPtr& u_op,
                   HazardNodePtr& ux, HazardOperationPtr& ux_op,
                   HazardNodePtr& uxl, HazardOperationPtr& uxl_op,
                   HazardNodePtr& uxr, HazardOperationPtr& uxr_op,
                   HazardNodePtr& uxlr, HazardOperationPtr& uxlr_op,
                   HazardNodePtr& uxlrr, HazardOperationPtr& uxlrr_op) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxrrr;
  Node* nxrr;
  Node* nxrl;
  Node* nxr;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxrrr = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), uxr->GetWeight() - 1,
        uxr->GetLeft(), uxr->GetRight(), Operation::INITIAL_DUMMY);
    if (nxrrr == NULL) break;
    nxrr = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), 1,
        uxlrr->GetRight(), nxrrr, Operation::INITIAL_DUMMY);
    if (nxrr == NULL) break;
    nxrl = node_pool_.Allocate(
        uxlr->GetKey(), uxlr->GetValue(), 1,
        uxlr->GetLeft(), uxlrr->GetLeft(), Operation::INITIAL_DUMMY);
    if (nxrl == NULL) break;
    nxr = node_pool_.Allocate(
        uxlrr->GetKey(), uxlrr->GetValue(), 0,
        nxrl, nxrr, Operation::INITIAL_DUMMY);
    if (nxr == NULL) break;
    nx = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), ux->GetWeight(),
        uxl->GetLeft(), nxr, Operation::INITIAL_DUMMY);
    if (nx == NULL) break;

    HazardOperationPtr op(GetOperationGuard(HIDX_CURRENT_OP));
    op.ProtectSafe(operation_pool_.Allocate());
    if (op == NULL) break;

    op->SetRoot(u, u_op);
    op->SetNewChild(nx);
    op->SetOldNodes(ux, ux_op,
                    uxl, uxl_op,
                    uxr, uxr_op,
                    uxlr, uxlr_op,
                    uxlrr, uxlrr_op);

    bool succeeded = op->Help(GetNodeGuard(HIDX_HELPING),
                              GetOperationGuard(HIDX_HELPING));
    op->CleanUp();
    
    if (!succeeded) {
      RetireOperation(op);
      result = EMBB_BUSY;
      break;
    }

    RetireOperation(u_op);
    RetireNode(ux); RetireOperation(ux_op);
    RetireNode(uxl); RetireOperation(uxl_op);
    RetireNode(uxr); RetireOperation(uxr_op);
    RetireNode(uxlr); RetireOperation(uxlr_op);
    RetireNode(uxlrr); RetireOperation(uxlrr_op);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxrrr) FreeNode(nxrrr);
    if (nxrr) FreeNode(nxrr);
    if (nxrl) FreeNode(nxrl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t W4_L(HazardNodePtr& u, HazardOperationPtr& u_op,
                   HazardNodePtr& ux, HazardOperationPtr& ux_op,
                   HazardNodePtr& uxl, HazardOperationPtr& uxl_op,
                   HazardNodePtr& uxr, HazardOperationPtr& uxr_op,
                   HazardNodePtr& uxrl, HazardOperationPtr& uxrl_op,
                   HazardNodePtr& uxrlr, HazardOperationPtr& uxrlr_op) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxll;
  Node* nxrl;
  Node* nxl;
  Node* nxr;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxll = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), uxl->GetWeight() - 1,
        uxl->GetLeft(), uxl->GetRight(), Operation::INITIAL_DUMMY);
    if (nxll == NULL) break;
    nxrl = node_pool_.Allocate(
        uxrlr->GetKey(), uxrlr->GetValue(), 1,
        uxrlr->GetLeft(), uxrlr->GetRight(), Operation::INITIAL_DUMMY);
    if (nxrl == NULL) break;
    nxl = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), 1,
        nxll, uxrl->GetLeft(), Operation::INITIAL_DUMMY);
    if (nxl == NULL) break;
    nxr = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), 0,
        nxrl, uxr->GetRight(), Operation::INITIAL_DUMMY);
    if (nxr == NULL) break;
    nx = node_pool_.Allocate(
        uxrl->GetKey(), uxrl->GetValue(), ux->GetWeight(),
        nxl, nxr, Operation::INITIAL_DUMMY);
    if (nx == NULL) break;

    HazardOperationPtr op(GetOperationGuard(HIDX_CURRENT_OP));
    op.ProtectSafe(operation_pool_.Allocate());
    if (op == NULL) break;

    op->SetRoot(u, u_op);
    op->SetNewChild(nx);
    op->SetOldNodes(ux, ux_op,
                    uxl, uxl_op,
                    uxr, uxr_op,
                    uxrl, uxrl_op,
                    uxrlr, uxrlr_op);

    bool succeeded = op->Help(GetNodeGuard(HIDX_HELPING),
                              GetOperationGuard(HIDX_HELPING));
    op->CleanUp();
    
    if (!succeeded) {
      RetireOperation(op);
      result = EMBB_BUSY;
      break;
    }

    RetireOperation(u_op);
    RetireNode(ux); RetireOperation(ux_op);
    RetireNode(uxl); RetireOperation(uxl_op);
    RetireNode(uxr); RetireOperation(uxr_op);
    RetireNode(uxrl); RetireOperation(uxrl_op);
    RetireNode(uxrlr); RetireOperation(uxrlr_op);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxll) FreeNode(nxll);
    if (nxrl) FreeNode(nxrl);
    if (nxl) FreeNode(nxl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t W4_R(HazardNodePtr& u, HazardOperationPtr& u_op,
                   HazardNodePtr& ux, HazardOperationPtr& ux_op,
                   HazardNodePtr& uxl, HazardOperationPtr& uxl_op,
                   HazardNodePtr& uxr, HazardOperationPtr& uxr_op,
                   HazardNodePtr& uxlr, HazardOperationPtr& uxlr_op,
                   HazardNodePtr& uxlrl, HazardOperationPtr& uxlrl_op) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxrr;
  Node* nxlr;
  Node* nxr;
  Node* nxl;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxrr = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), uxr->GetWeight() - 1,
        uxr->GetLeft(), uxr->GetRight(), Operation::INITIAL_DUMMY);
    if (nxrr == NULL) break;
    nxlr = node_pool_.Allocate(
        uxlrl->GetKey(), uxlrl->GetValue(), 1,
        uxlrl->GetLeft(), uxlrl->GetRight(), Operation::INITIAL_DUMMY);
    if (nxlr == NULL) break;
    nxr = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), 1,
        uxlr->GetRight(), nxrr, Operation::INITIAL_DUMMY);
    if (nxr == NULL) break;
    nxl = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), 0,
        uxl->GetLeft(), nxlr, Operation::INITIAL_DUMMY);
    if (nxl == NULL) break;
    nx = node_pool_.Allocate(
        uxlr->GetKey(), uxlr->GetValue(), ux->GetWeight(),
        nxl, nxr, Operation::INITIAL_DUMMY);
    if (nx == NULL) break;

    HazardOperationPtr op(GetOperationGuard(HIDX_CURRENT_OP));
    op.ProtectSafe(operation_pool_.Allocate());
    if (op == NULL) break;

    op->SetRoot(u, u_op);
    op->SetNewChild(nx);
    op->SetOldNodes(ux, ux_op,
                    uxl, uxl_op,
                    uxr, uxr_op,
                    uxlr, uxlr_op,
                    uxlrl, uxlrl_op);

    bool succeeded = op->Help(GetNodeGuard(HIDX_HELPING),
                              GetOperationGuard(HIDX_HELPING));
    op->CleanUp();
    
    if (!succeeded) {
      RetireOperation(op);
      result = EMBB_BUSY;
      break;
    }

    RetireOperation(u_op);
    RetireNode(ux); RetireOperation(ux_op);
    RetireNode(uxl); RetireOperation(uxl_op);
    RetireNode(uxr); RetireOperation(uxr_op);
    RetireNode(uxlr); RetireOperation(uxlr_op);
    RetireNode(uxlrl); RetireOperation(uxlrl_op);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxrr) FreeNode(nxrr);
    if (nxlr) FreeNode(nxlr);
    if (nxr) FreeNode(nxr);
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t W5_L(HazardNodePtr& u, HazardOperationPtr& u_op,
                   HazardNodePtr& ux, HazardOperationPtr& ux_op,
                   HazardNodePtr& uxl, HazardOperationPtr& uxl_op,
                   HazardNodePtr& uxr, HazardOperationPtr& uxr_op,
                   HazardNodePtr& uxrr, HazardOperationPtr& uxrr_op) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxll;
  Node* nxl;
  Node* nxr;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxll = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), uxl->GetWeight() - 1,
        uxl->GetLeft(), uxl->GetRight(), Operation::INITIAL_DUMMY);
    if (nxll == NULL) break;
    nxl = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), 1,
        nxll, uxr->GetLeft(), Operation::INITIAL_DUMMY);
    if (nxl == NULL) break;
    nxr = node_pool_.Allocate(
        uxrr->GetKey(), uxrr->GetValue(), 1,
        uxrr->GetLeft(), uxrr->GetRight(), Operation::INITIAL_DUMMY);
    if (nxr == NULL) break;
    nx = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), ux->GetWeight(),
        nxl, nxr, Operation::INITIAL_DUMMY);
    if (nx == NULL) break;

    HazardOperationPtr op(GetOperationGuard(HIDX_CURRENT_OP));
    op.ProtectSafe(operation_pool_.Allocate());
    if (op == NULL) break;

    op->SetRoot(u, u_op);
    op->SetNewChild(nx);
    op->SetOldNodes(ux, ux_op,
                    uxl, uxl_op,
                    uxr, uxr_op,
                    uxrr, uxrr_op);

    bool succeeded = op->Help(GetNodeGuard(HIDX_HELPING),
                              GetOperationGuard(HIDX_HELPING));
    op->CleanUp();
    
    if (!succeeded) {
      RetireOperation(op);
      result = EMBB_BUSY;
      break;
    }

    RetireOperation(u_op);
    RetireNode(ux); RetireOperation(ux_op);
    RetireNode(uxl); RetireOperation(uxl_op);
    RetireNode(uxr); RetireOperation(uxr_op);
    RetireNode(uxrr); RetireOperation(uxrr_op);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxll) FreeNode(nxll);
    if (nxl) FreeNode(nxl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t W5_R(HazardNodePtr& u, HazardOperationPtr& u_op,
                   HazardNodePtr& ux, HazardOperationPtr& ux_op,
                   HazardNodePtr& uxl, HazardOperationPtr& uxl_op,
                   HazardNodePtr& uxr, HazardOperationPtr& uxr_op,
                   HazardNodePtr& uxll, HazardOperationPtr& uxll_op) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxrr;
  Node* nxr;
  Node* nxl;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxrr = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), uxr->GetWeight() - 1,
        uxr->GetLeft(), uxr->GetRight(), Operation::INITIAL_DUMMY);
    if (nxrr == NULL) break;
    nxr = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), 1,
        uxl->GetRight(), nxrr, Operation::INITIAL_DUMMY);
    if (nxr == NULL) break;
    nxl = node_pool_.Allocate(
        uxll->GetKey(), uxll->GetValue(), 1,
        uxll->GetLeft(), uxll->GetRight(), Operation::INITIAL_DUMMY);
    if (nxl == NULL) break;
    nx = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), ux->GetWeight(),
        nxl, nxr, Operation::INITIAL_DUMMY);
    if (nx == NULL) break;

    HazardOperationPtr op(GetOperationGuard(HIDX_CURRENT_OP));
    op.ProtectSafe(operation_pool_.Allocate());
    if (op == NULL) break;

    op->SetRoot(u, u_op);
    op->SetNewChild(nx);
    op->SetOldNodes(ux, ux_op,
                    uxl, uxl_op,
                    uxr, uxr_op,
                    uxll, uxll_op);

    bool succeeded = op->Help(GetNodeGuard(HIDX_HELPING),
                              GetOperationGuard(HIDX_HELPING));
    op->CleanUp();
    
    if (!succeeded) {
      RetireOperation(op);
      result = EMBB_BUSY;
      break;
    }

    RetireOperation(u_op);
    RetireNode(ux); RetireOperation(ux_op);
    RetireNode(uxl); RetireOperation(uxl_op);
    RetireNode(uxr); RetireOperation(uxr_op);
    RetireNode(uxll); RetireOperation(uxll_op);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxrr) FreeNode(nxrr);
    if (nxr) FreeNode(nxr);
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t W6_L(HazardNodePtr& u, HazardOperationPtr& u_op,
                   HazardNodePtr& ux, HazardOperationPtr& ux_op,
                   HazardNodePtr& uxl, HazardOperationPtr& uxl_op,
                   HazardNodePtr& uxr, HazardOperationPtr& uxr_op,
                   HazardNodePtr& uxrl, HazardOperationPtr& uxrl_op) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxll;
  Node* nxl;
  Node* nxr;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxll = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), uxl->GetWeight() - 1,
        uxl->GetLeft(), uxl->GetRight(), Operation::INITIAL_DUMMY);
    if (nxll == NULL) break;
    nxl = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), 1,
        nxll, uxrl->GetLeft(), Operation::INITIAL_DUMMY);
    if (nxl == NULL) break;
    nxr = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), 1,
        uxrl->GetRight(), uxr->GetRight(), Operation::INITIAL_DUMMY);
    if (nxr == NULL) break;
    nx = node_pool_.Allocate(
        uxrl->GetKey(), uxrl->GetValue(), ux->GetWeight(),
        nxl, nxr, Operation::INITIAL_DUMMY);
    if (nx == NULL) break;

    HazardOperationPtr op(GetOperationGuard(HIDX_CURRENT_OP));
    op.ProtectSafe(operation_pool_.Allocate());
    if (op == NULL) break;

    op->SetRoot(u, u_op);
    op->SetNewChild(nx);
    op->SetOldNodes(ux, ux_op,
                    uxl, uxl_op,
                    uxr, uxr_op,
                    uxrl, uxrl_op);

    bool succeeded = op->Help(GetNodeGuard(HIDX_HELPING),
                              GetOperationGuard(HIDX_HELPING));
    op->CleanUp();
    
    if (!succeeded) {
      RetireOperation(op);
      result = EMBB_BUSY;
      break;
    }

    RetireOperation(u_op);
    RetireNode(ux); RetireOperation(ux_op);
    RetireNode(uxl); RetireOperation(uxl_op);
    RetireNode(uxr); RetireOperation(uxr_op);
    RetireNode(uxrl); RetireOperation(uxrl_op);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxll) FreeNode(nxll);
    if (nxl) FreeNode(nxl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t W6_R(HazardNodePtr& u, HazardOperationPtr& u_op,
                   HazardNodePtr& ux, HazardOperationPtr& ux_op,
                   HazardNodePtr& uxl, HazardOperationPtr& uxl_op,
                   HazardNodePtr& uxr, HazardOperationPtr& uxr_op,
                   HazardNodePtr& uxlr, HazardOperationPtr& uxlr_op) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxrr;
  Node* nxr;
  Node* nxl;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxrr = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), uxr->GetWeight() - 1,
        uxr->GetLeft(), uxr->GetRight(), Operation::INITIAL_DUMMY);
    if (nxrr == NULL) break;
    nxr = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), 1,
        uxlr->GetRight(), nxrr, Operation::INITIAL_DUMMY);
    if (nxr == NULL) break;
    nxl = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), 1,
        uxl->GetLeft(), uxlr->GetLeft(), Operation::INITIAL_DUMMY);
    if (nxl == NULL) break;
    nx = node_pool_.Allocate(
        uxlr->GetKey(), uxlr->GetValue(), ux->GetWeight(),
        nxl, nxr, Operation::INITIAL_DUMMY);
    if (nx == NULL) break;

    HazardOperationPtr op(GetOperationGuard(HIDX_CURRENT_OP));
    op.ProtectSafe(operation_pool_.Allocate());
    if (op == NULL) break;

    op->SetRoot(u, u_op);
    op->SetNewChild(nx);
    op->SetOldNodes(ux, ux_op,
                    uxl, uxl_op,
                    uxr, uxr_op,
                    uxlr, uxlr_op);

    bool succeeded = op->Help(GetNodeGuard(HIDX_HELPING),
                              GetOperationGuard(HIDX_HELPING));
    op->CleanUp();
    
    if (!succeeded) {
      RetireOperation(op);
      result = EMBB_BUSY;
      break;
    }

    RetireOperation(u_op);
    RetireNode(ux); RetireOperation(ux_op);
    RetireNode(uxl); RetireOperation(uxl_op);
    RetireNode(uxr); RetireOperation(uxr_op);
    RetireNode(uxlr); RetireOperation(uxlr_op);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxrr) FreeNode(nxrr);
    if (nxr) FreeNode(nxr);
    if (nxl) FreeNode(nxl);
    if (nx) FreeNode(nx);
  }

  return result;
}

embb_errors_t W7(HazardNodePtr& u, HazardOperationPtr& u_op,
                 HazardNodePtr& ux, HazardOperationPtr& ux_op,
                 HazardNodePtr& uxl, HazardOperationPtr& uxl_op,
                 HazardNodePtr& uxr, HazardOperationPtr& uxr_op) {
  embb_errors_t result = EMBB_NOMEM;
  Node* nxl;
  Node* nxr;
  Node* nx;

  while (result != EMBB_SUCCESS) {
    nxl = node_pool_.Allocate(
        uxl->GetKey(), uxl->GetValue(), uxl->GetWeight() - 1,
        uxl->GetLeft(), uxl->GetRight(), Operation::INITIAL_DUMMY);
    if (nxl == NULL) break;
    nxr = node_pool_.Allocate(
        uxr->GetKey(), uxr->GetValue(), uxr->GetWeight() - 1,
        uxr->GetLeft(), uxr->GetRight(), Operation::INITIAL_DUMMY);
    if (nxr == NULL) break;
    nx = node_pool_.Allocate(
        ux->GetKey(), ux->GetValue(), IsSentinel(u) ? 1 : ux->GetWeight() + 1,
        nxl, nxr, Operation::INITIAL_DUMMY);
    if (nx == NULL) break;

    HazardOperationPtr op(GetOperationGuard(HIDX_CURRENT_OP));
    op.ProtectSafe(operation_pool_.Allocate());
    if (op == NULL) break;

    op->SetRoot(u, u_op);
    op->SetNewChild(nx);
    op->SetOldNodes(ux, ux_op,
                    uxl, uxl_op,
                    uxr, uxr_op);

    bool succeeded = op->Help(GetNodeGuard(HIDX_HELPING),
                              GetOperationGuard(HIDX_HELPING));
    op->CleanUp();
    
    if (!succeeded) {
      RetireOperation(op);
      result = EMBB_BUSY;
      break;
    }

    RetireOperation(u_op);
    RetireNode(ux); RetireOperation(ux_op);
    RetireNode(uxl); RetireOperation(uxl_op);
    RetireNode(uxr); RetireOperation(uxr_op);

    result = EMBB_SUCCESS;
  }

  if (result != EMBB_SUCCESS) {
    if (nxl) FreeNode(nxl);
    if (nxr) FreeNode(nxr);
    if (nx) FreeNode(nx);
  }

  return result;
}

#endif // EMBB_CONTAINERS_INTERNAL_LOCK_FREE_CHROMATIC_TREE_REBALANCE_H_
