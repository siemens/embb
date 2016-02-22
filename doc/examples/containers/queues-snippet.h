embb::containers::LockFreeMPMCQueue<int> queue(10); //@\label{lst:queue_lst1:line_create}@

int i, j;
bool result = queue.TryDequeue(i); //@\label{lst:queue_lst1:fail_pop}@
assert(result == false);

for (i = 0; i <= 4; ++i) { //@\label{lst:queue_lst1:loop1}@
  result = queue.TryEnqueue(i); //@\label{lst:queue_lst1:push}@
  assert(result == true);
}

for (i = 0; i <= 4; ++i) { //@\label{lst:queue_lst1:loop2}@
  result = queue.TryDequeue(j); //@\label{lst:queue_lst1:pop}@
  assert(result == true && i == j); //@\label{lst:queue_lst1:assert}@
}