  mtapi_status_t status;

  /* initialize node attributes to default values */
  mtapi_node_attributes_t node_attr;
  mtapi_nodeattr_init(&node_attr, &status);
  MTAPI_CHECK_STATUS(status);

  /* set node type to SMP */
  mtapi_nodeattr_set(
    &node_attr,
    MTAPI_NODE_TYPE,
    MTAPI_ATTRIBUTE_VALUE(MTAPI_NODE_TYPE_SMP),
    MTAPI_ATTRIBUTE_POINTER_AS_VALUE,
    &status);
  MTAPI_CHECK_STATUS(status);

  /* initialize the node */
  mtapi_info_t info;
  mtapi_initialize(
    THIS_DOMAIN_ID,
    THIS_NODE_ID,
    &node_attr,
    &info,
    &status);
  MTAPI_CHECK_STATUS(status);
