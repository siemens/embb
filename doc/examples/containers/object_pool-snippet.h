embb::containers::ObjectPool<int> objPool(5); //@\label{lst:object_pool_lst1:line_create}@

int* alloc[5];

for (int i = 0; i != 5; ++i) {
  alloc[i] = objPool.Allocate(); //@\label{lst:object_pool_lst1:line_allocate}@
}

for (int i = 0; i != 5; ++i) {
  objPool.Free(alloc[i]); //@\label{lst:object_pool_lst1:line_free}@
}