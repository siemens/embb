#ifndef OPERATIONS_H
#define	OPERATIONS_H

static const int MAX_NODES = 5;

typedef struct {
  const char *name;
} OldNode;

typedef struct {
  const char *name;
  const char *orig_node;
  const char *weight;
  const char *left;
  const char *right;
} NewNode;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4510 4512 4610)
#endif
typedef struct {
  const char    *name;
  const int      num_nodes;
  const OldNode  old_nodes[MAX_NODES];
  const NewNode  new_nodes[MAX_NODES];
} RebalancingOperation;
#ifdef _MSC_VER
#pragma warning(pop)
#endif

static const RebalancingOperation REBALANCING_OPERATIONS[] = {
  {
    "BLK",
    3,
    {"ux", "uxl", "uxr"},
    {
      {"nxl", "uxl", "1", "uxl->GetLeft()", "uxl->GetRight()"},
      {"nxr", "uxr", "1", "uxr->GetLeft()", "uxr->GetRight()"},
      {"nx", "ux", "u->IsSentinel() ? 1 : ux->GetWeight() - 1", "nxl", "nxr"}
    }
  },

  {
    "PUSH_L",
    3,
    {"ux", "uxl", "uxr"},
    {
      {"nxl", "uxl", "uxl->GetWeight() - 1", "uxl->GetLeft()", "uxl->GetRight()"},
      {"nxr", "uxr", "0", "uxr->GetLeft()", "uxr->GetRight()"},
      {"nx", "ux", "u->IsSentinel() ? 1 : ux->GetWeight() + 1", "nxl", "nxr"}
    }
  },

  {
    "PUSH_R",
    3,
    {"ux", "uxl", "uxr"},
    {
      {"nxr", "uxr", "uxr->GetWeight() - 1", "uxr->GetLeft()", "uxr->GetRight()"},
      {"nxl", "uxl", "0", "uxl->GetLeft()", "uxl->GetRight()"},
      {"nx", "ux", "u->IsSentinel() ? 1 : ux->GetWeight() + 1", "nxl", "nxr"}
    }
  },

  {
    "RB1_L",
    2,
    {"ux", "uxl"},
    {
      {"nxr", "ux", "0", "uxl->GetRight()", "ux->GetRight()"},
      {"nx", "uxl", "ux->GetWeight()", "uxl->GetLeft()", "nxr"}
    }
  },

  {
    "RB1_R",
    2,
    {"ux", "uxr"},
    {
      {"nxl", "ux", "0", "ux->GetLeft()", "uxr->GetLeft()"},
      {"nx", "uxr", "ux->GetWeight()", "nxl", "uxr->GetRight()"}
    }
  },

  {
    "RB2_L",
    3,
    {"ux", "uxl", "uxlr"},
    {
      {"nxl", "uxl", "0", "uxl->GetLeft()", "uxlr->GetLeft()"},
      {"nxr", "ux", "0", "uxlr->GetRight()", "ux->GetRight()"},
      {"nx", "uxlr", "ux->GetWeight()", "nxl", "nxr"}
    }
  },

  {
    "RB2_R",
    3,
    {"ux", "uxr", "uxrl"},
    {
      {"nxr", "uxr", "0", "uxrl->GetRight()", "uxr->GetRight()"},
      {"nxl", "ux", "0", "ux->GetLeft()", "uxrl->GetLeft()"},
      {"nx", "uxrl", "ux->GetWeight()", "nxl", "nxr"}
    }
  },

  {
    "W1_L",
    4,
    {"ux", "uxl", "uxr", "uxrl"},
    {
      {"nxll", "uxl", "uxl->GetWeight() - 1", "uxl->GetLeft()", "uxl->GetRight()"},
      {"nxlr", "uxrl", "uxrl->GetWeight() - 1", "uxrl->GetLeft()", "uxrl->GetRight()"},
      {"nxl", "ux", "1", "nxll", "nxlr"},
      {"nx", "uxr", "ux->GetWeight()", "nxl", "uxr->GetRight()"}
    }
  },

  {
    "W1_R",
    4,
    {"ux", "uxl", "uxr", "uxlr"},
    {
      {"nxrr", "uxr", "uxr->GetWeight() - 1", "uxr->GetLeft()", "uxr->GetRight()"},
      {"nxrl", "uxlr", "uxlr->GetWeight() - 1", "uxlr->GetLeft()", "uxlr->GetRight()"},
      {"nxr", "ux", "1", "nxrl", "nxrr"},
      {"nx", "uxl", "ux->GetWeight()", "uxl->GetLeft()", "nxr"}
    }
  },

  {
    "W2_L",
    4,
    {"ux", "uxl", "uxr", "uxrl"},
    {
      {"nxll", "uxl", "uxl->GetWeight() - 1", "uxl->GetLeft()", "uxl->GetRight()"},
      {"nxlr", "uxrl", "0", "uxrl->GetLeft()", "uxrl->GetRight()"},
      {"nxl", "ux", "1", "nxll", "nxlr"},
      {"nx", "uxr", "ux->GetWeight()", "nxl", "uxr->GetRight()"}
    }
  },

  {
    "W2_R",
    4,
    {"ux", "uxl", "uxr", "uxlr"},
    {
      {"nxrr", "uxr", "uxr->GetWeight() - 1", "uxr->GetLeft()", "uxr->GetRight()"},
      {"nxrl", "uxlr", "0", "uxlr->GetLeft()", "uxlr->GetRight()"},
      {"nxr", "ux", "1", "nxrl", "nxrr"},
      {"nx", "uxl", "ux->GetWeight()", "uxl->GetLeft()", "nxr"}
    }
  },

  {
    "W3_L",
    5,
    {"ux", "uxl", "uxr", "uxrl", "uxrll"},
    {
      {"nxlll", "uxl", "uxl->GetWeight() - 1", "uxl->GetLeft()", "uxl->GetRight()"},
      {"nxll", "ux", "1", "nxlll", "uxrll->GetLeft()"},
      {"nxlr", "uxrl", "1", "uxrll->GetRight()", "uxrl->GetRight()"},
      {"nxl", "uxrll", "0", "nxll", "nxlr"},
      {"nx", "uxr", "ux->GetWeight()", "nxl", "uxr->GetRight()"}
    }
  },

  {
    "W3_R",
    5,
    {"ux", "uxl", "uxr", "uxlr", "uxlrr"},
    {
      {"nxrrr", "uxr", "uxr->GetWeight() - 1", "uxr->GetLeft()", "uxr->GetRight()"},
      {"nxrr", "ux", "1", "uxlrr->GetRight()", "nxrrr"},
      {"nxrl", "uxlr", "1", "uxlr->GetLeft()", "uxlrr->GetLeft()"},
      {"nxr", "uxlrr", "0", "nxrl", "nxrr"},
      {"nx", "uxl", "ux->GetWeight()", "uxl->GetLeft()", "nxr"}
    }
  },

  {
    "W4_L",
    5,
    {"ux", "uxl", "uxr", "uxrl", "uxrlr"},
    {
      {"nxll", "uxl", "uxl->GetWeight() - 1", "uxl->GetLeft()", "uxl->GetRight()"},
      {"nxrl", "uxrlr", "1", "uxrlr->GetLeft()", "uxrlr->GetRight()"},
      {"nxl", "ux", "1", "nxll", "uxrl->GetLeft()"},
      {"nxr", "uxr", "0", "nxrl", "uxr->GetRight()"},
      {"nx", "uxrl", "ux->GetWeight()", "nxl", "nxr"}
    }
  },

  {
    "W4_R",
    5,
    {"ux", "uxl", "uxr", "uxlr", "uxlrl"},
    {
      {"nxrr", "uxr", "uxr->GetWeight() - 1", "uxr->GetLeft()", "uxr->GetRight()"},
      {"nxlr", "uxlrl", "1", "uxlrl->GetLeft()", "uxlrl->GetRight()"},
      {"nxr", "ux", "1", "uxlr->GetRight()", "nxrr"},
      {"nxl", "uxl", "0", "uxl->GetLeft()", "nxlr"},
      {"nx", "uxlr", "ux->GetWeight()", "nxl", "nxr"}
    }
  },

  {
    "W5_L",
    4,
    {"ux", "uxl", "uxr", "uxrr"},
    {
      {"nxll", "uxl", "uxl->GetWeight() - 1", "uxl->GetLeft()", "uxl->GetRight()"},
      {"nxl", "ux", "1", "nxll", "uxr->GetLeft()"},
      {"nxr", "uxrr", "1", "uxrr->GetLeft()", "uxrr->GetRight()"},
      {"nx", "uxr", "ux->GetWeight()", "nxl", "nxr"}
    }
  },

  {
    "W5_R",
    4,
    {"ux", "uxl", "uxr", "uxll"},
    {
      {"nxrr", "uxr", "uxr->GetWeight() - 1", "uxr->GetLeft()", "uxr->GetRight()"},
      {"nxr", "ux", "1", "uxl->GetRight()", "nxrr"},
      {"nxl", "uxll", "1", "uxll->GetLeft()", "uxll->GetRight()"},
      {"nx", "uxl", "ux->GetWeight()", "nxl", "nxr"}
    }
  },

  {
    "W6_L",
    4,
    {"ux", "uxl", "uxr", "uxrl"},
    {
      {"nxll", "uxl", "uxl->GetWeight() - 1", "uxl->GetLeft()", "uxl->GetRight()"},
      {"nxl", "ux", "1", "nxll", "uxrl->GetLeft()"},
      {"nxr", "uxr", "1", "uxrl->GetRight()", "uxr->GetRight()"},
      {"nx", "uxrl", "ux->GetWeight()", "nxl", "nxr"}
    }
  },

  {
    "W6_R",
    4,
    {"ux", "uxl", "uxr", "uxlr"},
    {
      {"nxrr", "uxr", "uxr->GetWeight() - 1", "uxr->GetLeft()", "uxr->GetRight()"},
      {"nxr", "ux", "1", "uxlr->GetRight()", "nxrr"},
      {"nxl", "uxl", "1", "uxl->GetLeft()", "uxlr->GetLeft()"},
      {"nx", "uxlr", "ux->GetWeight()", "nxl", "nxr"}
    }
  },

  {
    "W7",
    3,
    {"ux", "uxl", "uxr"},
    {
      {"nxl", "uxl", "uxl->GetWeight() - 1", "uxl->GetLeft()", "uxl->GetRight()"},
      {"nxr", "uxr", "uxr->GetWeight() - 1", "uxr->GetLeft()", "uxr->GetRight()"},
      {"nx", "ux", "u->IsSentinel() ? 1 : ux->GetWeight() + 1", "nxl", "nxr"}
    }
  }
};

#endif	// OPERATIONS_H

