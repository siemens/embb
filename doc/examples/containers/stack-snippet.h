embb::containers::LockFreeStack<int> stack(10); //@\label{lst:stack_lst1:line_create}@

int i, j;
bool result = stack.TryPop(i); //@\label{lst:stack_lst1:fail_pop}@
assert(result == false);

for (i = 0; i <= 4; ++i) {//@\label{lst:stack_lst1:loop1}@
  result = stack.TryPush(i); //@\label{lst:stack_lst1:push}@
  assert(result == true);
}

for (i = 4; i >= 0; --i) { //@\label{lst:stack_lst1:loop2}@
  result = stack.TryPop(j); //@\label{lst:stack_lst1:pop}@
  assert(result == true && i == j); //@\label{lst:stack_lst1:assert}@
}