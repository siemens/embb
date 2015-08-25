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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chromatic_tree_operations.h"

static const char INCLUDE_GUARD[] =
    "EMBB_CONTAINERS_INTERNAL_LOCK_FREE_CHROMATIC_TREE_REBALANCE_H_";

static const char GENERATOR_NOTICE[] =
    "//\n"
    "// This file was created automatically by a code generator.\n"
    "// Any direct changes will be lost after rebuild of the project.\n"
    "//";

static const char RETURN_TYPE[]  = "embb_errors_t";
static const char NODEARG_TYPE[] = "HazardNodePtr";
static const char LOCKARG_TYPE[] = "HazardOperationPtr";
static const char NEWNODE_TYPE[] = "Node*";

void PrintOperationSourceCode(FILE* file, const RebalancingOperation& op);

int main(int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(stderr, "USAGE:\n  %s <output_file>\n", argv[0]);
    return 1;
  }

  const char* filename = argv[1];
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif
  FILE *file = fopen(filename, "w");
#ifdef _MSC_VER
#pragma warning(pop)
#endif
  if (file == NULL) {
    fprintf(stderr, "Error: Cannot open file '%s' for writing!\n", filename);
    return 1;
  }

  // Printing header
  fprintf(file, "#ifndef %s\n#define %s\n\n", INCLUDE_GUARD, INCLUDE_GUARD);
  fprintf(file, "%s\n\n", GENERATOR_NOTICE);

  // Printing methods code
  int num_operations = (sizeof(REBALANCING_OPERATIONS) /
                        sizeof(REBALANCING_OPERATIONS[0]));
  for (int i = 0; i < num_operations; ++i) {
    PrintOperationSourceCode(file, REBALANCING_OPERATIONS[i]);
  }

  // Printing trailer
  fprintf(file, "#endif // %s\n", INCLUDE_GUARD);

  fclose(file);

  return 0;
}

void PrintOperationSourceCode(FILE* file, const RebalancingOperation& op) {
  // Method signature
  fprintf(file, "%s %s(", RETURN_TYPE, op.name);

  // Method arguments
  fprintf(file, "%s& u, %s& u_op", NODEARG_TYPE, LOCKARG_TYPE);
  int offset = static_cast<int>(strlen(NODEARG_TYPE) + strlen(op.name) + 2);
  for (int i = 0; i < op.num_nodes; ++i) {
    fprintf(file, ",\n%*s%s& %s, %s& %s_op", offset, "",
            NODEARG_TYPE, op.old_nodes[i].name,
            LOCKARG_TYPE, op.old_nodes[i].name);
  }
  fprintf(file, ") {\n"
      "  embb_errors_t result = EMBB_NOMEM;\n");

  // Define nodes
  for (int i = 0; i < op.num_nodes; ++i) {
    fprintf(file, "  %s %s = NULL;\n", NEWNODE_TYPE, op.new_nodes[i].name);
  }

  fprintf(file, "\n"
      "  while (result != EMBB_SUCCESS) {\n");

  // Construct new nodes
  for (int i = 0; i < op.num_nodes; ++i) {
    fprintf(file, "    %s = node_pool_.Allocate(\n"
                  "        %s->GetKey(), %s->GetValue(), %s,\n"
                  "        %s, %s, Operation::INITIAL_DUMMY);\n"
                  "    if (%s == NULL) break;\n",
            op.new_nodes[i].name,
            op.new_nodes[i].orig_node, op.new_nodes[i].orig_node,
            op.new_nodes[i].weight,
            op.new_nodes[i].left, op.new_nodes[i].right,
            op.new_nodes[i].name);
  }

  fprintf(file, "\n"
      "    HazardOperationPtr op(GetOperationGuard(HIDX_CURRENT_OP));\n"
      "    op.ProtectSafe(operation_pool_.Allocate());\n"
      "    if (op == NULL) break;\n");

  // Create and fill the operation object
  fprintf(file, "\n"
                "    op->SetRoot(u, u_op);\n"
                "    op->SetNewChild(nx);\n"
                "    op->SetOldNodes(%s, %s_op",
          op.old_nodes[0].name, op.old_nodes[0].name);
  for (int i = 1; i < op.num_nodes; ++i) {
    fprintf(file, ",\n                    %s, %s_op",
            op.old_nodes[i].name, op.old_nodes[i].name);
  }
  fprintf(file, ");\n");

  // Execute operation
  fprintf(file, "\n"
      "    bool succeeded = op->Help(GetNodeGuard(HIDX_HELPING),\n"
      "                              GetOperationGuard(HIDX_HELPING));\n"
      "    op->CleanUp();\n"
      "    \n"
      "    if (!succeeded) {\n"
      "      RetireOperation(op);\n"
      "      result = EMBB_BUSY;\n"
      "      break;\n"
      "    }\n");

  // Release original nodes and operations
  fprintf(file, "\n"
      "    RetireOperation(u_op);\n");
  for (int i = 0; i < op.num_nodes; ++i) {
    fprintf(file, "    RetireNode(%s); RetireOperation(%s_op);\n",
            op.old_nodes[i].name, op.old_nodes[i].name);
  }
  fprintf(file, "\n"
      "    result = EMBB_SUCCESS;\n"
      "  }\n");

  // Delete new nodes if operation failed
  fprintf(file, "\n"
      "  if (result != EMBB_SUCCESS) {\n");
  for (int i = 0; i < op.num_nodes; ++i) {
    fprintf(file, "    if (%s) FreeNode(%s);\n",
            op.new_nodes[i].name, op.new_nodes[i].name);
  }
  fprintf(file, "  }\n");

  // Return statement
  fprintf(file, "\n"
      "  return result;\n"
      "}\n"
      "\n");
};
