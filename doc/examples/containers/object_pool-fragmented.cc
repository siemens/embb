#include<iostream>
#include<embb/containers/object_pool.h>
#include<embb/containers/lock_free_tree_value_pool.h>

void RunObjectPoolExample1()
{
  #include "containers/object_pool-snippet.h"
}

void RunObjectPoolExample2()
{
  #include "containers/object_pool_2-snippet.h"
}

void RunObjectPoolExamples()
{
  RunObjectPoolExample1();
  RunObjectPoolExample2();

}
