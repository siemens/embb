/**
/* Internal tests for the linearizability checker
*/

#include <linearizability_tester.h>
#include <sequential_data_structures.h>

using namespace lt;

static void test_lru_cache()
{
  LruCache<char> lru_cache{ 3 };
  assert(lru_cache.insert('\1'));
  assert(!lru_cache.insert('\1'));

  assert(lru_cache.insert('\2'));
  assert(lru_cache.insert('\3'));
  assert(lru_cache.insert('\1'));
  assert(!lru_cache.insert('\3'));
  assert(lru_cache.insert('\4'));
  assert(lru_cache.insert('\1'));
}

static void test_atomic_op()
{
  bool ok;
  state::Atomic atomic, new_atomic;

  OpPtr<state::Atomic> read_call_op_ptr;

  OpPtr<state::Atomic> read_0_ret_op_ptr, read_0_pending_op_ptr;
  OpPtr<state::Atomic> read_1_ret_op_ptr;
  OpPtr<state::Atomic> read_2_ret_op_ptr;

  OpPtr<state::Atomic> write_1_call_op_ptr, write_1_ret_op_ptr, write_1_pending_op_ptr;
  OpPtr<state::Atomic> cas_2_succeeded_call_op_ptr, cas_2_succeeded_ret_op_ptr, cas_2_pending_op_ptr;
  OpPtr<state::Atomic> cas_2_failed_call_op_ptr, cas_2_failed_ret_op_ptr;

  read_call_op_ptr = state::Atomic::make_read_call();

  read_0_ret_op_ptr = state::Atomic::make_read_ret('\0');
  read_0_pending_op_ptr = state::Atomic::make_read_pending();
  read_1_ret_op_ptr = state::Atomic::make_read_ret('\1');
  read_2_ret_op_ptr = state::Atomic::make_read_ret('\2');

  write_1_call_op_ptr = state::Atomic::make_write_call('\1');
  write_1_ret_op_ptr = state::Atomic::make_write_ret();
  write_1_pending_op_ptr = state::Atomic::make_write_pending();

  cas_2_succeeded_call_op_ptr = state::Atomic::make_cas_call('\1', '\2');
  cas_2_succeeded_ret_op_ptr = state::Atomic::make_cas_ret(true);
  cas_2_pending_op_ptr = state::Atomic::make_cas_pending();

  cas_2_failed_call_op_ptr = state::Atomic::make_cas_call('\2', '\1');
  cas_2_failed_ret_op_ptr = state::Atomic::make_cas_ret(false);

  Op<state::Atomic>& read_call_op = *read_call_op_ptr;

  Op<state::Atomic>& read_0_ret_op = *read_0_ret_op_ptr;
  Op<state::Atomic>& read_0_pending_op = *read_0_pending_op_ptr;
  Op<state::Atomic>& read_1_ret_op = *read_1_ret_op_ptr;
  Op<state::Atomic>& read_2_ret_op = *read_2_ret_op_ptr;

  Op<state::Atomic>& write_1_call_op = *write_1_call_op_ptr;
  Op<state::Atomic>& write_1_ret_op = *write_1_ret_op_ptr;
  Op<state::Atomic>& write_1_pending_op = *write_1_pending_op_ptr;

  Op<state::Atomic>& cas_2_succeeded_call_op = *cas_2_succeeded_call_op_ptr;
  Op<state::Atomic>& cas_2_succeeded_ret_op = *cas_2_succeeded_ret_op_ptr;
  Op<state::Atomic>& cas_2_pending_op = *cas_2_pending_op_ptr;

  Op<state::Atomic>& cas_2_failed_call_op = *cas_2_failed_call_op_ptr;
  Op<state::Atomic>& cas_2_failed_ret_op = *cas_2_failed_ret_op_ptr;

  assert(!read_call_op.is_partitionable());
  assert(!read_0_ret_op.is_partitionable());
  assert(!read_0_pending_op.is_partitionable());
  assert(!read_1_ret_op.is_partitionable());
  assert(!read_2_ret_op.is_partitionable());

  assert(!write_1_call_op.is_partitionable());
  assert(!write_1_ret_op.is_partitionable());
  assert(!write_1_pending_op.is_partitionable());

  assert(!cas_2_succeeded_call_op.is_partitionable());
  assert(!cas_2_succeeded_ret_op.is_partitionable());
  assert(!cas_2_pending_op.is_partitionable());
  assert(!cas_2_failed_call_op.is_partitionable());
  assert(!cas_2_failed_ret_op.is_partitionable());

  std::tie(ok, new_atomic) = read_call_op.apply(atomic, read_0_ret_op);
  assert(atomic == new_atomic);
  assert(!ok);

  std::tie(ok, new_atomic) = read_call_op.apply(atomic, read_1_ret_op);
  assert(atomic == new_atomic);
  assert(!ok);

  std::tie(ok, new_atomic) = read_call_op.apply(atomic, read_0_pending_op);
  assert(atomic == new_atomic);
  assert(ok);

  std::tie(ok, new_atomic) = read_call_op.apply(atomic, read_1_ret_op);
  assert(atomic == new_atomic);
  assert(!ok);

  std::tie(ok, new_atomic) = write_1_call_op.apply(atomic, write_1_pending_op);
  assert(atomic != new_atomic);
  assert(ok);

  std::tie(ok, new_atomic) = write_1_call_op.apply(atomic, write_1_ret_op);
  assert(atomic != new_atomic);
  assert(ok);

  atomic = new_atomic;

  std::tie(ok, new_atomic) = read_call_op.apply(atomic, read_1_ret_op);
  assert(atomic == new_atomic);
  assert(ok);

  std::tie(ok, new_atomic) = read_call_op.apply(atomic, read_2_ret_op);
  assert(atomic == new_atomic);
  assert(!ok);

  std::tie(ok, new_atomic) = cas_2_failed_call_op.apply(atomic, cas_2_failed_ret_op);
  assert(atomic == new_atomic);
  assert(ok);

  std::tie(ok, new_atomic) = cas_2_failed_call_op.apply(atomic, cas_2_succeeded_ret_op);
  assert(atomic == new_atomic);
  assert(!ok);

  std::tie(ok, new_atomic) = cas_2_succeeded_call_op.apply(atomic, cas_2_pending_op);
  assert(atomic != new_atomic);
  assert(ok);

  std::tie(ok, new_atomic) = cas_2_succeeded_call_op.apply(atomic, cas_2_succeeded_ret_op);
  assert(atomic != new_atomic);
  assert(ok);

  atomic = new_atomic;

  std::tie(ok, new_atomic) = read_call_op.apply(atomic, read_2_ret_op);
  assert(atomic == new_atomic);
  assert(ok);
}

/// a few sanity checks
static void test_stack()
{
  Entry<state::Set> ret, call;
  call.set_op(state::Set::make_contains_call('\1'));
  ret.set_op(state::Set::make_ret(true));
  call.set_match(&ret);

  EntryPtr<state::Set> A{ &call }, B{ &call }, C{ &call };

  Stack<state::Set> stack{ 3 };

  assert(stack.is_empty());
  assert(!stack.is_full());
  assert(stack.size() == 0U);

  stack.push(A, state::Set());

  assert(std::get<0>(stack.top()) == A);
  assert(!stack.is_empty());
  assert(!stack.is_full());
  assert(stack.size() == 1U);

  stack.push(B, state::Set());

  assert(std::get<0>(stack.top()) == B);
  assert(!stack.is_empty());
  assert(!stack.is_full());
  assert(stack.size() == 2U);

  stack.push(C, state::Set());

  assert(std::get<0>(stack.top()) == C);
  assert(!stack.is_empty());
  assert(stack.is_full());
  assert(stack.size() == 3U);

  stack.pop();

  assert(std::get<0>(stack.top()) == B);
  assert(!stack.is_empty());
  assert(!stack.is_full());
  assert(stack.size() == 2U);

  stack.push(C, state::Set());

  assert(std::get<0>(stack.top()) == C);
  assert(!stack.is_empty());
  assert(stack.is_full());
  assert(stack.size() == 3U);

  // pop multiple entries at once
  stack.pop(2);

  assert(!stack.is_empty());
  assert(!stack.is_full());
  assert(stack.size() == 1U);

  stack.pop();

  assert(stack.is_empty());
  assert(!stack.is_full());
  assert(stack.size() == 0U);
}

static void test_bitset()
{
  constexpr unsigned bits_per_block = static_cast<unsigned>(sizeof(unsigned long) * CHAR_BIT);
  constexpr unsigned N = bits_per_block + 7;

  FlexibleBitset bitset;

  for (unsigned k{ 0U }; k < N; ++k)
    assert(!bitset.is_set(k));

  assert(bitset.is_empty());

  bitset.set(0);
  assert(bitset.is_set(0));
  assert(!bitset.is_empty());

  for (unsigned k{ 1U }; k < N; ++k)
    assert(!bitset.is_set(k));

  bitset.reset(0);
  assert(!bitset.is_set(0));
  assert(bitset.is_empty());

  bitset.set(1);
  assert(!bitset.is_set(0));
  assert(bitset.is_set(1));
  assert(!bitset.is_empty());

  for (unsigned k{ 2U }; k < N; ++k)
    assert(!bitset.is_set(k));

  bitset.set(N-1);
  assert(bitset.is_set(N-1));
  assert(!bitset.is_empty());

  for (unsigned k{ 2U }; k < N - 1U; ++k)
    assert(!bitset.is_set(k));

  FlexibleBitset another_bitset;
  another_bitset.set(1);
  another_bitset.set(N-1);
  
  FlexibleBitset yet_another_bitset(another_bitset);

  assert(bitset == another_bitset);
  assert(bitset == yet_another_bitset);

  assert(!bitset.is_set(bits_per_block - 1U));
  assert(bitset.set(bits_per_block - 1U));
  assert(bitset.is_set(bits_per_block - 1U));

  assert(!bitset.is_set(bits_per_block));
  assert(bitset.set(bits_per_block));
  assert(bitset.is_set(bits_per_block));

  assert(!bitset.is_set(bits_per_block + 1U));
  assert(bitset.set(bits_per_block + 1U));
  assert(bitset.is_set(bits_per_block + 1U));

  assert(!bitset.is_set(2U * bits_per_block - 1U));
  assert(bitset.set(2U * bits_per_block - 1U));
  assert(bitset.is_set(2U * bits_per_block - 1U));

  assert(!bitset.is_set(2U * bits_per_block));
  assert(bitset.set(2U * bits_per_block));
  assert(bitset.is_set(2U * bits_per_block));

  assert(!bitset.is_set(2U * bits_per_block + 1U));
  assert(bitset.set(2U * bits_per_block + 1U));
  assert(bitset.is_set(2U * bits_per_block + 1U));

  bitset = another_bitset;

  assert(bitset.set(2U * bits_per_block - 1U));
  assert(bitset.reset(2U * bits_per_block - 1U));
  assert(bitset == another_bitset);

  assert(bitset.set(2U * bits_per_block));
  assert(bitset.reset(2U * bits_per_block));
  // different number of blocks
  assert(bitset != another_bitset);

  assert(bitset.set(2U * bits_per_block + 1U));
  assert(bitset.reset(2U * bits_per_block + 1U));

  // different number of blocks
  assert(bitset != another_bitset);
}

static void test_state_set()
{
  bool ok;
  state::Set set;

  assert(!set.contains('\1'));

  std::tie(ok, set) = set.insert('\1');
  assert(ok);

  assert(set.contains('\1'));

  // item is already in the set
  std::tie(ok, set) = set.insert('\1');
  assert(!ok);
  assert(set.contains('\1'));

  std::tie(ok, set) = set.erase('\1');
  assert(ok);
  assert(!set.contains('\1'));

  // item has been already erased from the set
  std::tie(ok, set) = set.erase('\1');
  assert(!ok);
  assert(!set.contains('\1'));
}

static void test_state_set_op()
{
  bool ok;
  state::Set set, new_set;

  OpPtr<state::Set> empty_op_ptr;
  OpPtr<state::Set> contains_op_ptr;
  OpPtr<state::Set> insert_op_ptr;
  OpPtr<state::Set> erase_op_ptr;

  OpPtr<state::Set> true_ret_op_ptr{ state::Set::make_ret(true) };
  OpPtr<state::Set> false_ret_op_ptr{ state::Set::make_ret(false) };

  const Op<state::Set>& true_ret_op = *true_ret_op_ptr;
  const Op<state::Set>& false_ret_op = *false_ret_op_ptr;

  empty_op_ptr = state::Set::make_empty_call();
  contains_op_ptr = state::Set::make_contains_call('\1');
  insert_op_ptr = state::Set::make_insert_call('\1');
  erase_op_ptr = state::Set::make_erase_call('\1');

  Op<state::Set>& empty_op = *empty_op_ptr;
  Op<state::Set>& contains_op = *contains_op_ptr;
  Op<state::Set>& insert_op = *insert_op_ptr;
  Op<state::Set>& erase_op = *erase_op_ptr;

  std::tie(ok, new_set) = empty_op.apply(set, true_ret_op);
  assert(set == new_set);
  assert(ok);

  std::tie(ok, new_set) = contains_op.apply(set, false_ret_op);
  assert(set == new_set);
  assert(ok);

  std::tie(ok, new_set) = insert_op.apply(set, true_ret_op);
  assert(set != new_set);
  assert(ok);

  set = new_set;

  std::tie(ok, new_set) = empty_op.apply(set, false_ret_op);
  assert(set == new_set);
  assert(ok);

  std::tie(ok, new_set) = contains_op.apply(set, true_ret_op);
  assert(set == new_set);
  assert(ok);

  // item is already in the set, so insertion is unsuccessful
  std::tie(ok, new_set) = insert_op.apply(set, false_ret_op);
  assert(set == new_set);
  assert(ok);

  std::tie(ok, new_set) = contains_op.apply(set, true_ret_op);
  assert(set == new_set);
  assert(ok);

  std::tie(ok, new_set) = erase_op.apply(set, true_ret_op);
  assert(set != new_set);
  assert(ok);

  assert(std::get<0>(contains_op.apply(set, true_ret_op)));
  assert(!std::get<0>(contains_op.apply(set, false_ret_op)));

  assert(!std::get<0>(contains_op.apply(new_set, true_ret_op)));
  assert(std::get<0>(contains_op.apply(new_set, false_ret_op)));
}

static void test_state_stack()
{
  constexpr unsigned N = 2;

  state::Stack<N> stack;
  state::Stack<N> new_stack;
  state::Stack<N> stack_1, stack_2;

  assert(stack.is_empty());
  assert(!stack.is_full());

  new_stack = stack.push('\1');

  assert(stack != new_stack);
  stack = stack_1 = new_stack;

  assert(!stack.is_empty());
  assert(!stack.is_full());

  assert(stack.top() == '\1');

  new_stack = stack.push('\2');

  assert(stack != new_stack);
  stack = stack_2 = new_stack;

  assert(stack_1 != stack_2);

  assert(!stack.is_empty());
  assert(stack.is_full());

  assert(stack.top() == '\2');

  new_stack = stack.pop();
  assert(new_stack == stack_1);

  stack = new_stack;

  assert(!stack.is_empty());
  assert(!stack.is_full());

  assert(stack.top() == '\1');

  new_stack = stack.push('\2');

  assert(stack != new_stack);
  assert(new_stack == stack_2);

  stack = new_stack;

  assert(!stack.is_empty());
  assert(stack.is_full());

  assert(stack.top() == '\2');

  new_stack = stack.pop();
  assert(new_stack == stack_1);

  stack = new_stack;

  assert(!stack.is_empty());
  assert(!stack.is_full());

  assert(stack.top() == '\1');

  new_stack = stack.push('\3');

  assert(stack != new_stack);
  assert(new_stack != stack_1);
  assert(new_stack != stack_2);

  stack = new_stack;

  assert(!stack.is_empty());
  assert(stack.is_full());

  assert(stack.top() == '\3');
}

static void test_state_queue()
{
  constexpr unsigned N = 2;

  state::Queue<N> queue;

  state::Queue<N> new_queue;
  state::Queue<N> queue_1, queue_2;

  assert(queue.is_empty());
  assert(!queue.is_full());
  
  queue.enqueue('\1');
  new_queue = queue.enqueue('\1');

  assert(queue != new_queue);
  queue = queue_1 = new_queue;

  assert(!queue.is_empty());
  assert(!queue.is_full());

  assert(queue.get_value() == '\1');

  new_queue = queue.enqueue('\2');

  assert(queue != new_queue);
  queue = queue_2 = new_queue;

  assert(queue_1 != queue_2);

  assert(!queue.is_empty());
  assert(queue.is_full());

  assert(queue.get_value() == '\1');

  new_queue = queue.dequeue();
  assert(new_queue != queue_1);

  queue = new_queue;
  
  assert(!queue.is_empty());
  assert(!queue.is_full());

  assert(queue.get_value() == '\2');

  new_queue = queue.enqueue('\1');

  assert(queue != new_queue);
  assert(new_queue != queue_2);

  queue = new_queue;

  assert(!queue.is_empty());
  assert(queue.is_full());

  assert(queue.get_value() == '\2');

  new_queue = queue.dequeue();
  assert(new_queue != queue_1);
  queue = new_queue;

  assert(!queue.is_empty());
  assert(!queue.is_full());

  assert(queue.get_value() == '\1');

  new_queue = queue.enqueue('\3');

  assert(queue != new_queue);
  assert(new_queue != queue_1);
  assert(new_queue != queue_2);

  queue = new_queue;

  assert(!queue.is_empty());
  assert(queue.is_full());

  assert(queue.get_value() == '\1');
  
}

static void test_state_stack_op()
{
  constexpr unsigned N = 1;

  bool ok;
  state::Stack<N> stack, new_stack;

  OpPtr<state::Stack<N>> try_push_1_op_ptr, try_push_2_op_ptr;
  OpPtr<state::Stack<N>> try_pop_op_ptr;

  OpPtr<state::Stack<N>> true_try_push_ret_op_ptr{ state::Stack<N>::make_try_push_ret(true) };
  OpPtr<state::Stack<N>> false_try_push_ret_op_ptr{ state::Stack<N>::make_try_push_ret(false) };

  const Op<state::Stack<N>>& true_try_push_ret_op = *true_try_push_ret_op_ptr;
  const Op<state::Stack<N>>& false_try_push_ret_op = *false_try_push_ret_op_ptr;

  OpPtr<state::Stack<N>> true_1_try_pop_ret_op_ptr{ state::Stack<N>::make_try_pop_ret(true, '\1') };
  OpPtr<state::Stack<N>> true_2_try_pop_ret_op_ptr{ state::Stack<N>::make_try_pop_ret(true, '\2') };
  OpPtr<state::Stack<N>> false_try_pop_ret_op_ptr{ state::Stack<N>::make_try_pop_ret(false, '\0') };

  const Op<state::Stack<N>>& true_1_try_pop_ret_op = *true_1_try_pop_ret_op_ptr;
  const Op<state::Stack<N>>& true_2_try_pop_ret_op = *true_2_try_pop_ret_op_ptr;
  const Op<state::Stack<N>>& false_try_pop_ret_op = *false_try_pop_ret_op_ptr;

  try_pop_op_ptr = state::Stack<N>::make_try_pop_call();
  Op<state::Stack<N>>& try_pop_op = *try_pop_op_ptr;
  
  std::tie(ok, new_stack) = try_pop_op.apply(stack, false_try_pop_ret_op);
  assert(stack == new_stack);
  assert(ok);

  std::tie(ok, new_stack) = try_pop_op.apply(stack, true_1_try_pop_ret_op);
  assert(stack == new_stack);
  assert(!ok);

  std::tie(ok, new_stack) = try_pop_op.apply(stack, true_2_try_pop_ret_op);
  assert(stack == new_stack);
  assert(!ok);

  try_push_2_op_ptr = state::Stack<N>::make_try_push_call('\2');
  Op<state::Stack<N>>& try_push_2_op = *try_push_2_op_ptr;

  std::tie(ok, new_stack) = try_push_2_op.apply(stack, false_try_push_ret_op);
  assert(stack != new_stack);
  assert(!ok);
  
  std::tie(ok, new_stack) = try_push_2_op.apply(stack, true_try_push_ret_op);
  assert(stack != new_stack);
  assert(ok);

  stack = new_stack;

  try_push_1_op_ptr = state::Stack<N>::make_try_push_call('\1');
  Op<state::Stack<N>>& try_push_1_op = *try_push_1_op_ptr;

  // stack is full
  std::tie(ok, new_stack) = try_push_1_op.apply(stack, false_try_push_ret_op);
  assert(stack == new_stack);
  assert(ok);

  std::tie(ok, new_stack) = try_push_1_op.apply(stack, true_try_push_ret_op);
  assert(stack == new_stack);
  assert(!ok);

  std::tie(ok, new_stack) = try_pop_op.apply(stack, false_try_pop_ret_op);
  assert(stack != new_stack);
  assert(!ok);

  std::tie(ok, new_stack) = try_pop_op.apply(stack, true_1_try_pop_ret_op);
  assert(stack != new_stack);
  assert(!ok);

  std::tie(ok, new_stack) = try_pop_op.apply(stack, true_2_try_pop_ret_op);
  assert(stack != new_stack);
  assert(ok);
}

static void test_state_queue_op()
{
  constexpr unsigned N = 1;

  bool ok;
  state::Queue<N> queue, new_queue;

  OpPtr<state::Queue<N>> try_enqueue_1_op_ptr, try_enqueue_2_op_ptr;
  OpPtr<state::Queue<N>> try_dequeue_op_ptr;

  OpPtr<state::Queue<N>> true_try_enqueue_ret_op_ptr{ state::Queue<N>::make_try_enqueue_ret(true) };
  OpPtr<state::Queue<N>> false_try_enqueue_ret_op_ptr{ state::Queue<N>::make_try_enqueue_ret(false) };

  const Op<state::Queue<N>>& true_try_enqueue_ret_op = *true_try_enqueue_ret_op_ptr;
  const Op<state::Queue<N>>& false_try_enqueue_ret_op = *false_try_enqueue_ret_op_ptr;

  OpPtr<state::Queue<N>> true_1_try_dequeue_ret_op_ptr{ state::Queue<N>::make_try_dequeue_ret(true, '\1') };
  OpPtr<state::Queue<N>> true_2_try_dequeue_ret_op_ptr{ state::Queue<N>::make_try_dequeue_ret(true, '\2') };
  OpPtr<state::Queue<N>> false_try_dequeue_ret_op_ptr{ state::Queue<N>::make_try_dequeue_ret(false, '\0') };

  const Op<state::Queue<N>>& true_1_try_dequeue_ret_op = *true_1_try_dequeue_ret_op_ptr;
  const Op<state::Queue<N>>& true_2_try_dequeue_ret_op = *true_2_try_dequeue_ret_op_ptr;
  const Op<state::Queue<N>>& false_try_dequeue_ret_op = *false_try_dequeue_ret_op_ptr;

  try_dequeue_op_ptr = state::Queue<N>::make_try_dequeue_call();
  Op<state::Queue<N>>& try_dequeue_op = *try_dequeue_op_ptr;
  
  std::tie(ok, new_queue) = try_dequeue_op.apply(queue, false_try_dequeue_ret_op);
  assert(queue == new_queue);
  assert(ok);
  
  std::tie(ok, new_queue) = try_dequeue_op.apply(queue, true_1_try_dequeue_ret_op);
  assert(queue == new_queue);
  assert(!ok);

  std::tie(ok, new_queue) = try_dequeue_op.apply(queue, true_2_try_dequeue_ret_op);
  assert(queue == new_queue);
  assert(!ok);
  

  try_enqueue_2_op_ptr = state::Queue<N>::make_try_enqueue_call('\2');
  Op<state::Queue<N>>& try_enqueue_2_op = *try_enqueue_2_op_ptr;

  std::tie(ok, new_queue) = try_enqueue_2_op.apply(queue, false_try_enqueue_ret_op);
  assert(queue != new_queue);
  assert(!ok);
  
  std::tie(ok, new_queue) = try_enqueue_2_op.apply(queue, true_try_enqueue_ret_op);
  assert(queue != new_queue);
  assert(ok);

  queue = new_queue;

  try_enqueue_1_op_ptr = state::Queue<N>::make_try_enqueue_call('\1');
  Op<state::Queue<N>>& try_enqueue_1_op = *try_enqueue_1_op_ptr;

  // queue is full
  std::tie(ok, new_queue) = try_enqueue_1_op.apply(queue, false_try_enqueue_ret_op);
  assert(queue == new_queue);
  assert(ok);

  std::tie(ok, new_queue) = try_enqueue_1_op.apply(queue, true_try_enqueue_ret_op);
  assert(queue == new_queue);
  assert(!ok);

  std::tie(ok, new_queue) = try_dequeue_op.apply(queue, false_try_dequeue_ret_op);
  assert(queue != new_queue);
  assert(!ok);

  std::tie(ok, new_queue) = try_dequeue_op.apply(queue, true_1_try_dequeue_ret_op);
  assert(queue != new_queue);
  assert(!ok);

  std::tie(ok, new_queue) = try_dequeue_op.apply(queue, true_2_try_dequeue_ret_op);
  assert(queue != new_queue);
  assert(ok);
}
/// The empty log is trivially linearizable.
static void test_linearizability_empty_log()
{
  std::size_t number_of_entries{ 0U };
  LogInfo<state::Set> log_info;
  LinearizabilityTester<state::Set> t{ log_info };
  assert(log_info.is_empty());
  assert(t.check());
}

/// a few sanity checks on the raw entry data structure
static void test_raw_single_contains_is_linearizable()
{
  Entry<state::Set> contains_call, contains_ret;

  contains_ret.set_op(state::Set::make_ret(false));
  contains_ret.prev = &contains_call;

  contains_call.set_op(state::Set::make_contains_call('\1'));
  contains_call.next = &contains_ret;
  contains_call.set_match(&contains_ret);

  std::size_t number_of_entries{ 2U };
  LogInfo<state::Set> log_info{ &contains_call, number_of_entries };
  LinearizabilityTester<state::Set> t{ log_info };
  assert(t.check());
}

static void test_raw_single_contains_is_not_linearizable()
{
  Entry<state::Set> contains_call, contains_ret;

  contains_ret.set_op(state::Set::make_ret(true));
  contains_ret.prev = &contains_call;

  contains_call.set_op(state::Set::make_contains_call('\1'));
  contains_call.next = &contains_ret;
  contains_call.set_match(&contains_ret);

  std::size_t number_of_entries{ 2U };
  LogInfo<state::Set> log_info{ &contains_call, number_of_entries };
  LinearizabilityTester<state::Set> t{ log_info };
  assert(!t.check());
}

static void test_single_contains(bool ret)
{
  Log<state::Set> log{ 4U };
  EntryPtr<state::Set> contains_call_entry_ptr, contains_ret_entry_ptr;

  contains_call_entry_ptr = log.add_call(state::Set::make_contains_call('\1'));
  contains_ret_entry_ptr = log.add_ret(contains_call_entry_ptr, state::Set::make_ret(ret));

  assert(log.log_head_ptr() == contains_call_entry_ptr);
  assert(log.number_of_entries() == 2U);

  assert(log.log_head_ptr() == contains_call_entry_ptr);
  assert(contains_call_entry_ptr->prev == nullptr);
  assert(contains_call_entry_ptr->next == contains_ret_entry_ptr);
  assert(contains_call_entry_ptr->match() == contains_ret_entry_ptr);

  assert(contains_ret_entry_ptr->match() == contains_call_entry_ptr);
  assert(contains_ret_entry_ptr->prev == contains_call_entry_ptr);
  assert(contains_ret_entry_ptr->next == nullptr);

  LinearizabilityTester<state::Set> t{ log.info() };
  assert(t.check() == (!ret));

  if (ret)
  {
    // If log cannot be linearized, then all pointers
    // (except the first one) are still the same.
    assert(log.log_head_ptr() == contains_call_entry_ptr);
    assert(contains_call_entry_ptr->prev != nullptr);
    assert(contains_call_entry_ptr->next == contains_ret_entry_ptr);
    assert(contains_call_entry_ptr->match() == contains_ret_entry_ptr);

    assert(contains_ret_entry_ptr->match() == contains_call_entry_ptr);
    assert(contains_ret_entry_ptr->prev == contains_call_entry_ptr);
    assert(contains_ret_entry_ptr->next == nullptr);
  }
}

static void test_log_copy()
{
  constexpr bool ret = true;

  Log<state::Set> log{ 4U };
  EntryPtr<state::Set> contains_call_entry_ptr, contains_ret_entry_ptr;

  contains_call_entry_ptr = log.add_call(state::Set::make_contains_call('\1'));
  contains_ret_entry_ptr = log.add_ret(contains_call_entry_ptr, state::Set::make_ret(ret));

  assert(log.log_head_ptr() == contains_call_entry_ptr);
  assert(log.number_of_entries() == 2U);

  assert(log.log_head_ptr() == contains_call_entry_ptr);
  assert(contains_call_entry_ptr->prev == nullptr);
  assert(contains_call_entry_ptr->next == contains_ret_entry_ptr);
  assert(contains_call_entry_ptr->match() == contains_ret_entry_ptr);

  assert(contains_ret_entry_ptr->match() == contains_call_entry_ptr);
  assert(contains_ret_entry_ptr->prev == contains_call_entry_ptr);
  assert(contains_ret_entry_ptr->next == nullptr);

  Log<state::Set> log_copy{ log.info() };

  assert(log_copy.log_head_ptr() != contains_call_entry_ptr);
  assert(log_copy.log_head_ptr() != contains_call_entry_ptr);
  assert(log_copy.log_head_ptr()->entry_id() == 0U);
  assert(&log_copy.log_head_ptr()->op() == &contains_call_entry_ptr->op());
  assert(log_copy.log_head_ptr() == log_copy.log_head_ptr()->match()->match());
  assert(log_copy.log_head_ptr()->prev == nullptr);
  assert(log_copy.number_of_entries() == 2U);
}

//   contains(x) : contains_ret
// |---------------------------|
//
//         insert(x) : insert_ret
//     |---------------------------|
static void test_000(bool insert_ret, bool contains_ret)
{
  constexpr char x = '\1';

  constexpr unsigned contains_entry_id{ 0U };
  constexpr unsigned insert_entry_id{ 1U };

  Log<state::Set> log{ 4U };
  EntryPtr<state::Set> contains_call_entry_ptr, contains_ret_entry_ptr;
  EntryPtr<state::Set> insert_call_entry_ptr, insert_ret_entry_ptr;

  contains_call_entry_ptr = log.add_call(state::Set::make_contains_call(x));
  insert_call_entry_ptr = log.add_call(state::Set::make_insert_call(x));
  contains_ret_entry_ptr = log.add_ret(contains_call_entry_ptr, state::Set::make_ret(contains_ret));
  insert_ret_entry_ptr = log.add_ret(insert_call_entry_ptr, state::Set::make_ret(insert_ret));

  assert(insert_call_entry_ptr->entry_id() == insert_entry_id);
  assert(insert_ret_entry_ptr->entry_id() == insert_entry_id);

  assert(contains_call_entry_ptr->entry_id() == contains_entry_id);
  assert(contains_ret_entry_ptr->entry_id() == contains_entry_id);

  LinearizabilityTester<state::Set> t{ log.info() };
  assert(t.check() == insert_ret);
}

//        contains(x) : contains_ret
//      |----------------------------|
//
//    insert(x) : insert_ret
// |-------------------------|
static void test_001(bool insert_ret, bool contains_ret)
{
  constexpr char x = '\1';

  constexpr unsigned insert_entry_id{ 0U };
  constexpr unsigned contains_entry_id{ 1U };

  Log<state::Set> log{ 4U };
  EntryPtr<state::Set> contains_call_entry_ptr, contains_ret_entry_ptr;
  EntryPtr<state::Set> insert_call_entry_ptr, insert_ret_entry_ptr;

  insert_call_entry_ptr = log.add_call(state::Set::make_insert_call(x));
  contains_call_entry_ptr = log.add_call(state::Set::make_contains_call(x));
  insert_ret_entry_ptr = log.add_ret(insert_call_entry_ptr, state::Set::make_ret(insert_ret));
  contains_ret_entry_ptr = log.add_ret(contains_call_entry_ptr, state::Set::make_ret(contains_ret));

  assert(insert_call_entry_ptr->entry_id() == insert_entry_id);
  assert(insert_ret_entry_ptr->entry_id() == insert_entry_id);

  assert(contains_call_entry_ptr->entry_id() == contains_entry_id);
  assert(contains_ret_entry_ptr->entry_id() == contains_entry_id);

  LinearizabilityTester<state::Set> t{ log.info() };
  assert(t.check() == insert_ret);
}

//      contains(x) : contains_ret
//    |----------------------------|
//
//       insert(x) : insert_ret
// |----------------------------------|
static void test_002(bool insert_ret, bool contains_ret)
{
  constexpr char x = '\1';

  constexpr unsigned insert_entry_id{ 0U };
  constexpr unsigned contains_entry_id{ 1U };

  Log<state::Set> log{ 4U };
  EntryPtr<state::Set> contains_call_entry_ptr, contains_ret_entry_ptr;
  EntryPtr<state::Set> insert_call_entry_ptr, insert_ret_entry_ptr;

  insert_call_entry_ptr = log.add_call(state::Set::make_insert_call(x));
  contains_call_entry_ptr = log.add_call(state::Set::make_contains_call(x));
  contains_ret_entry_ptr = log.add_ret(contains_call_entry_ptr, state::Set::make_ret(contains_ret));
  insert_ret_entry_ptr = log.add_ret(insert_call_entry_ptr, state::Set::make_ret(insert_ret));

  assert(insert_call_entry_ptr->entry_id() == insert_entry_id);
  assert(insert_ret_entry_ptr->entry_id() == insert_entry_id);

  assert(contains_call_entry_ptr->entry_id() == contains_entry_id);
  assert(contains_ret_entry_ptr->entry_id() == contains_entry_id);

  LinearizabilityTester<state::Set> t{ log.info() };
  assert(t.check() == insert_ret);
}

//     contains(x) : contains_ret
// |----------------------------------|
//
//       insert(x) : insert_ret
//    |---------------------------|
static void test_003(bool insert_ret, bool contains_ret)
{
  constexpr char x = '\1';

  constexpr unsigned contains_entry_id{ 0U };
  constexpr unsigned insert_entry_id{ 1U };

  Log<state::Set> log{ 4U };
  EntryPtr<state::Set> contains_call_entry_ptr, contains_ret_entry_ptr;
  EntryPtr<state::Set> insert_call_entry_ptr, insert_ret_entry_ptr;

  contains_call_entry_ptr = log.add_call(state::Set::make_contains_call(x));
  insert_call_entry_ptr = log.add_call(state::Set::make_insert_call(x));
  insert_ret_entry_ptr = log.add_ret(insert_call_entry_ptr, state::Set::make_ret(insert_ret));
  contains_ret_entry_ptr = log.add_ret(contains_call_entry_ptr, state::Set::make_ret(contains_ret));

  assert(insert_call_entry_ptr->entry_id() == insert_entry_id);
  assert(insert_ret_entry_ptr->entry_id() == insert_entry_id);

  assert(contains_call_entry_ptr->entry_id() == contains_entry_id);
  assert(contains_ret_entry_ptr->entry_id() == contains_entry_id);

  LinearizabilityTester<state::Set> t{ log.info() };
  assert(t.check() == insert_ret);
}

//   insert(x) : insert_ret     contains(x) : contains_ret
// |------------------------| |---------------------------|
static void test_004(bool insert_ret, bool contains_ret)
{
  constexpr char x = '\1';

  constexpr unsigned insert_entry_id{ 0U };
  constexpr unsigned contains_entry_id{ 1U };

  Log<state::Set> log{ 4U };
  EntryPtr<state::Set> contains_call_entry_ptr, contains_ret_entry_ptr;
  EntryPtr<state::Set> insert_call_entry_ptr, insert_ret_entry_ptr;

  insert_call_entry_ptr = log.add_call(state::Set::make_insert_call(x));
  insert_ret_entry_ptr = log.add_ret(insert_call_entry_ptr, state::Set::make_ret(insert_ret));
  contains_call_entry_ptr = log.add_call(state::Set::make_contains_call(x));
  contains_ret_entry_ptr = log.add_ret(contains_call_entry_ptr, state::Set::make_ret(contains_ret));

  assert(insert_call_entry_ptr->entry_id() == insert_entry_id);
  assert(insert_ret_entry_ptr->entry_id() == insert_entry_id);

  assert(contains_call_entry_ptr->entry_id() == contains_entry_id);
  assert(contains_ret_entry_ptr->entry_id() == contains_entry_id);

  LinearizabilityTester<state::Set> t{ log.info() };
  assert(t.check() == (insert_ret && contains_ret));
}

//   contains(x) : contains_ret    insert(x) : insert_ret
// |---------------------------| |------------------------| 
static void test_005(bool insert_ret, bool contains_ret)
{
  constexpr char x = '\1';

  constexpr unsigned contains_entry_id{ 0U };
  constexpr unsigned insert_entry_id{ 1U };

  Log<state::Set> log{ 4U };
  EntryPtr<state::Set> contains_call_entry_ptr, contains_ret_entry_ptr;
  EntryPtr<state::Set> insert_call_entry_ptr, insert_ret_entry_ptr;

  contains_call_entry_ptr = log.add_call(state::Set::make_contains_call(x));
  contains_ret_entry_ptr = log.add_ret(contains_call_entry_ptr, state::Set::make_ret(contains_ret));
  insert_call_entry_ptr = log.add_call(state::Set::make_insert_call(x));
  insert_ret_entry_ptr = log.add_ret(insert_call_entry_ptr, state::Set::make_ret(insert_ret));

  assert(insert_call_entry_ptr->entry_id() == insert_entry_id);
  assert(insert_ret_entry_ptr->entry_id() == insert_entry_id);

  assert(contains_call_entry_ptr->entry_id() == contains_entry_id);
  assert(contains_ret_entry_ptr->entry_id() == contains_entry_id);

  LinearizabilityTester<state::Set> t{ log.info() };
  assert(t.check() == (insert_ret && !contains_ret));
}

//   insert(x) : insert_ret_0
// |--------------------------|
//
//       insert(x) : insert_ret_1
//     |--------------------------|
static void test_006(bool insert_ret_0, bool insert_ret_1)
{
  constexpr char x = '\1';

  constexpr unsigned insert_0_entry_id{ 0U };
  constexpr unsigned insert_1_entry_id{ 1U };

  EntryPtr<state::Set> insert_call_0_entry_ptr, insert_ret_0_entry_ptr;
  EntryPtr<state::Set> insert_call_1_entry_ptr, insert_ret_1_entry_ptr;

  Log<state::Set> log{ 4U };

  insert_call_0_entry_ptr = log.add_call(state::Set::make_insert_call(x));
  insert_call_1_entry_ptr = log.add_call(state::Set::make_insert_call(x));
  insert_ret_0_entry_ptr = log.add_ret(insert_call_0_entry_ptr, state::Set::make_ret(insert_ret_0));
  insert_ret_1_entry_ptr = log.add_ret(insert_call_1_entry_ptr, state::Set::make_ret(insert_ret_1));

  assert(insert_call_0_entry_ptr->entry_id() == insert_0_entry_id);
  assert(insert_ret_0_entry_ptr->entry_id() == insert_0_entry_id);

  assert(insert_call_1_entry_ptr->entry_id() == insert_1_entry_id);
  assert(insert_ret_1_entry_ptr->entry_id() == insert_1_entry_id);

  LinearizabilityTester<state::Set> t{ log.info() };
  assert(t.check() == (!(insert_ret_0 == insert_ret_1)));
}

//   insert(x) : insert_ret_0     insert(x) : insert_ret_1
// |--------------------------| |--------------------------|
static void test_007(bool insert_ret_0, bool insert_ret_1)
{
  constexpr char x = '\1';

  constexpr unsigned insert_0_entry_id{ 0U };
  constexpr unsigned insert_1_entry_id{ 1U };

  EntryPtr<state::Set> insert_call_0_entry_ptr, insert_ret_0_entry_ptr;
  EntryPtr<state::Set> insert_call_1_entry_ptr, insert_ret_1_entry_ptr;

  Log<state::Set> log{ 4U };

  insert_call_0_entry_ptr = log.add_call(state::Set::make_insert_call(x));
  insert_ret_0_entry_ptr = log.add_ret(insert_call_0_entry_ptr, state::Set::make_ret(insert_ret_0));
  insert_call_1_entry_ptr = log.add_call(state::Set::make_insert_call(x));
  insert_ret_1_entry_ptr = log.add_ret(insert_call_1_entry_ptr, state::Set::make_ret(insert_ret_1));

  assert(insert_call_0_entry_ptr->entry_id() == insert_0_entry_id);
  assert(insert_ret_0_entry_ptr->entry_id() == insert_0_entry_id);

  assert(insert_call_1_entry_ptr->entry_id() == insert_1_entry_id);
  assert(insert_ret_1_entry_ptr->entry_id() == insert_1_entry_id);

  LinearizabilityTester<state::Set> t{ log.info() };
  assert(t.check() == (insert_ret_0 && !insert_ret_1));
}

//           insert(x) : insert_ret
// |------------------------------------------|
//
//           insert(x) : insert_ret
//   |-------------------------------------|
//
//         contains(x) : contains_ret
//       |----------------------------|
//
static void test_008(bool insert_ret, bool contains_ret)
{
  constexpr char x = '\1';

  constexpr unsigned insert_0_entry_id{ 0U };
  constexpr unsigned insert_1_entry_id{ 1U };
  constexpr unsigned contains_entry_id{ 2U };

  Log<state::Set> log{ 8U };
  EntryPtr<state::Set> contains_call_entry_ptr, contains_ret_entry_ptr;
  EntryPtr<state::Set> insert_0_call_entry_ptr, insert_0_ret_entry_ptr;
  EntryPtr<state::Set> insert_1_call_entry_ptr, insert_1_ret_entry_ptr;

  insert_0_call_entry_ptr = log.add_call(state::Set::make_insert_call(x));
  insert_1_call_entry_ptr = log.add_call(state::Set::make_insert_call(x));
  contains_call_entry_ptr = log.add_call(state::Set::make_contains_call(x));
  contains_ret_entry_ptr = log.add_ret(contains_call_entry_ptr, state::Set::make_ret(contains_ret));
  insert_1_ret_entry_ptr = log.add_ret(insert_1_call_entry_ptr, state::Set::make_ret(insert_ret));
  insert_0_ret_entry_ptr = log.add_ret(insert_0_call_entry_ptr, state::Set::make_ret(insert_ret));

  assert(insert_0_call_entry_ptr->entry_id() == insert_0_entry_id);
  assert(insert_0_ret_entry_ptr->entry_id() == insert_0_entry_id);

  assert(insert_1_call_entry_ptr->entry_id() == insert_1_entry_id);
  assert(insert_1_ret_entry_ptr->entry_id() == insert_1_entry_id);

  assert(contains_call_entry_ptr->entry_id() == contains_entry_id);
  assert(contains_ret_entry_ptr->entry_id() == contains_entry_id);

  LinearizabilityTester<state::Set> t{ log.info() };
  assert(!t.check());
}

// !linearizable:
//
// Let x && y be distinct values.
//
//   contains(x) : false 
// |---------------------|
//
//      contains(y) : false
//    |---------------------|
//
//          insert(x) : false
//        |---------------------|
static void test_009()
{
  constexpr char x = '\1';
  constexpr char y = '\2';

  constexpr unsigned contains_x_entry_id{ 0U };
  constexpr unsigned contains_y_entry_id{ 1U };
  constexpr unsigned insert_x_entry_id{ 2U };

  Log<state::Set> log{ 6U };

  EntryPtr<state::Set> contains_x_call_entry_ptr, contains_x_ret_entry_ptr;
  EntryPtr<state::Set> contains_y_call_entry_ptr, contains_y_ret_entry_ptr;
  EntryPtr<state::Set> insert_x_call_entry_ptr, insert_x_ret_entry_ptr;

  contains_x_call_entry_ptr = log.add_call(state::Set::make_contains_call(x));
  contains_y_call_entry_ptr = log.add_call(state::Set::make_contains_call(y));
  insert_x_call_entry_ptr = log.add_call(state::Set::make_insert_call(x));

  contains_x_ret_entry_ptr = log.add_ret(contains_x_call_entry_ptr, state::Set::make_ret(false));
  contains_y_ret_entry_ptr = log.add_ret(contains_y_call_entry_ptr, state::Set::make_ret(false));
  insert_x_ret_entry_ptr = log.add_ret(insert_x_call_entry_ptr, state::Set::make_ret(false));

  assert(contains_x_call_entry_ptr->entry_id() == contains_x_entry_id);
  assert(contains_x_ret_entry_ptr->entry_id() == contains_x_entry_id);

  assert(contains_y_call_entry_ptr->entry_id() == contains_y_entry_id);
  assert(contains_y_ret_entry_ptr->entry_id() == contains_y_entry_id);

  assert(insert_x_call_entry_ptr->entry_id() == insert_x_entry_id);
  assert(insert_x_ret_entry_ptr->entry_id() == insert_x_entry_id);

  LinearizabilityTester<state::Set> t{ log.info() };
  assert(!t.check());
}

// !linearizable:
//
// Let x && y be distinct values.
//
//   contains(x) : false 
// |---------------------|
//
//       insert(x) : false
//    |---------------------|
//
//          contains(y) : false
//        |---------------------|
static void test_010()
{
  constexpr char x = '\1';
  constexpr char y = '\2';

  constexpr unsigned contains_x_entry_id{ 0U };
  constexpr unsigned insert_x_entry_id{ 1U };
  constexpr unsigned contains_y_entry_id{ 2U };

  Log<state::Set> log{ 6U };

  EntryPtr<state::Set> contains_x_call_entry_ptr, contains_x_ret_entry_ptr;
  EntryPtr<state::Set> contains_y_call_entry_ptr, contains_y_ret_entry_ptr;
  EntryPtr<state::Set> insert_x_call_entry_ptr, insert_x_ret_entry_ptr;

  contains_x_call_entry_ptr = log.add_call(state::Set::make_contains_call(x));
  insert_x_call_entry_ptr = log.add_call(state::Set::make_insert_call(x));
  contains_y_call_entry_ptr = log.add_call(state::Set::make_contains_call(y));

  contains_x_ret_entry_ptr = log.add_ret(contains_x_call_entry_ptr, state::Set::make_ret(false));
  insert_x_ret_entry_ptr = log.add_ret(insert_x_call_entry_ptr, state::Set::make_ret(false));
  contains_y_ret_entry_ptr = log.add_ret(contains_y_call_entry_ptr, state::Set::make_ret(false));

  assert(contains_x_call_entry_ptr->entry_id() == contains_x_entry_id);
  assert(contains_x_ret_entry_ptr->entry_id() == contains_x_entry_id);

  assert(insert_x_call_entry_ptr->entry_id() == insert_x_entry_id);
  assert(insert_x_ret_entry_ptr->entry_id() == insert_x_entry_id);

  assert(contains_y_call_entry_ptr->entry_id() == contains_y_entry_id);
  assert(contains_y_ret_entry_ptr->entry_id() == contains_y_entry_id);

  LinearizabilityTester<state::Set> t{ log.info() };
  assert(!t.check());
}

// Linearizable:
//
// Let x && y be distinct values.
//
//   insert(x) : true
// |------------------|
//
//      contains(y) : false
//    |---------------------|
//
//         contains(x) : false
//       |---------------------|
static void test_011()
{
  constexpr char x = '\1';
  constexpr char y = '\2';

  constexpr unsigned insert_x_entry_id{ 0U };
  constexpr unsigned contains_y_entry_id{ 1U };
  constexpr unsigned contains_x_entry_id{ 2U };

  Log<state::Set> log{ 6U };

  EntryPtr<state::Set> insert_x_call_entry_ptr, insert_x_ret_entry_ptr;
  EntryPtr<state::Set> contains_y_call_entry_ptr, contains_y_ret_entry_ptr;
  EntryPtr<state::Set> contains_x_call_entry_ptr, contains_x_ret_entry_ptr;

  insert_x_call_entry_ptr = log.add_call(state::Set::make_insert_call(x));
  contains_y_call_entry_ptr = log.add_call(state::Set::make_contains_call(y));
  contains_x_call_entry_ptr = log.add_call(state::Set::make_contains_call(x));

  insert_x_ret_entry_ptr = log.add_ret(insert_x_call_entry_ptr, state::Set::make_ret(true));
  contains_y_ret_entry_ptr = log.add_ret(contains_y_call_entry_ptr, state::Set::make_ret(false));
  contains_x_ret_entry_ptr = log.add_ret(contains_x_call_entry_ptr, state::Set::make_ret(false));

  assert(insert_x_call_entry_ptr->entry_id() == insert_x_entry_id);
  assert(insert_x_ret_entry_ptr->entry_id() == insert_x_entry_id);

  assert(contains_y_call_entry_ptr->entry_id() == contains_y_entry_id);
  assert(contains_y_ret_entry_ptr->entry_id() == contains_y_entry_id);

  assert(contains_x_call_entry_ptr->entry_id() == contains_x_entry_id);
  assert(contains_x_ret_entry_ptr->entry_id() == contains_x_entry_id);

  LinearizabilityTester<state::Set> t{ log.info() };
  assert(t.check());
}

// Linearizable:
//
// Let x && y be distinct values.
//
//   erase(x) : false     insert(y) : true
// |------------------| |------------------|
//
//                                               contains(x) : true
//                                 |------------------------------------------------|
//
//                                      contains(y) : false     insert(x) : true
//                                    |---------------------| |------------------|
static void test_012()
{
  constexpr char x = '\1';
  constexpr char y = '\2';

  Log<state::Set> log{ 10U };

  EntryPtr<state::Set> erase_x_call_entry_ptr, erase_x_ret_entry_ptr;
  EntryPtr<state::Set> insert_x_call_entry_ptr, insert_x_ret_entry_ptr;
  EntryPtr<state::Set> contains_x_call_entry_ptr, contains_x_ret_entry_ptr;
  EntryPtr<state::Set> insert_y_call_entry_ptr, insert_y_ret_entry_ptr;
  EntryPtr<state::Set> contains_y_call_entry_ptr, contains_y_ret_entry_ptr;

  erase_x_call_entry_ptr = log.add_call(state::Set::make_erase_call(x));
  erase_x_ret_entry_ptr = log.add_ret(erase_x_call_entry_ptr, state::Set::make_ret(false));
  insert_y_call_entry_ptr = log.add_call(state::Set::make_insert_call(y));
  contains_x_call_entry_ptr = log.add_call(state::Set::make_contains_call(x));
  contains_y_call_entry_ptr = log.add_call(state::Set::make_contains_call(y));
  insert_y_ret_entry_ptr = log.add_ret(insert_y_call_entry_ptr, state::Set::make_ret(true));
  contains_y_ret_entry_ptr = log.add_ret(contains_y_call_entry_ptr, state::Set::make_ret(false));
  insert_x_call_entry_ptr = log.add_call(state::Set::make_insert_call(x));
  insert_x_ret_entry_ptr = log.add_ret(insert_x_call_entry_ptr, state::Set::make_ret(true));
  contains_x_ret_entry_ptr = log.add_ret(contains_x_call_entry_ptr, state::Set::make_ret(true));

  LinearizabilityTester<state::Set> t{ log.info() };
  assert(t.check());
}

// entry id: X0, call: contains(x)
// entry id: X1, call: insert(x)
// entry id: X0, return: ret: 0
// entry id: X2, call: contains(x)
// entry id: X2, return: ret: 0
// entry id: X3, call: insert(x)
// entry id: X3, return: ret: 1
// entry id: X4, call: contains(x)
// entry id: X4, return: ret: 1
// entry id: X1, return: ret: 0
// entry id: X5, call: contains(x)
// entry id: X5, return: ret: 1
static void test_013()
{
  constexpr char x = '\1';

  Log<state::Set> log{ 12U };

  EntryPtr<state::Set> call_x0_entry_ptr, ret_x0_entry_ptr;
  EntryPtr<state::Set> call_x1_entry_ptr, ret_x1_entry_ptr;
  EntryPtr<state::Set> call_x2_entry_ptr, ret_x2_entry_ptr;
  EntryPtr<state::Set> call_x3_entry_ptr, ret_x3_entry_ptr;
  EntryPtr<state::Set> call_x4_entry_ptr, ret_x4_entry_ptr;
  EntryPtr<state::Set> call_x5_entry_ptr, ret_x5_entry_ptr;

  call_x0_entry_ptr = log.add_call(state::Set::make_contains_call(x));
  call_x1_entry_ptr = log.add_call(state::Set::make_insert_call(x));
  ret_x0_entry_ptr = log.add_ret(call_x0_entry_ptr, state::Set::make_ret(false));
  call_x2_entry_ptr = log.add_call(state::Set::make_contains_call(x));
  ret_x2_entry_ptr = log.add_ret(call_x2_entry_ptr, state::Set::make_ret(false));
  call_x3_entry_ptr = log.add_call(state::Set::make_insert_call(x));
  ret_x3_entry_ptr = log.add_ret(call_x3_entry_ptr, state::Set::make_ret(true));
  call_x4_entry_ptr = log.add_call(state::Set::make_contains_call(x));
  ret_x4_entry_ptr = log.add_ret(call_x4_entry_ptr, state::Set::make_ret(true));
  ret_x1_entry_ptr = log.add_ret(call_x1_entry_ptr, state::Set::make_ret(false));
  call_x5_entry_ptr = log.add_call(state::Set::make_contains_call(x));
  ret_x5_entry_ptr = log.add_ret(call_x5_entry_ptr, state::Set::make_ret(true));

  LinearizabilityTester<state::Set> t{ log.info() };
  assert(t.check());
}

// Let x && y be distinct.
//
// entry id: X0, call: contains(x)
// entry id: X1, call: insert(x)
// entry id: X0, return: ret: 0
// entry id: Y0, call: contains(y) <- !linearizable
// entry id: Y0, return: ret: 1
// entry id: X2, call: contains(x)
// entry id: X2, return: ret: 0
// entry id: X3, call: insert(x)
// entry id: X3, return: ret: 1
// entry id: X4, call: contains(x)
// entry id: X4, return: ret: 1
// entry id: X1, return: ret: 0
// entry id: X5, call: contains(x)
// entry id: X5, return: ret: 1
static void test_014()
{
  constexpr char x = '\1';
  constexpr char y = '\2';

  constexpr unsigned not_linearizable_entry_id = 2U;

  Log<state::Set> log{ 14U };

  EntryPtr<state::Set> call_x0_entry_ptr, ret_x0_entry_ptr;
  EntryPtr<state::Set> call_x1_entry_ptr, ret_x1_entry_ptr;
  EntryPtr<state::Set> call_x2_entry_ptr, ret_x2_entry_ptr;
  EntryPtr<state::Set> call_x3_entry_ptr, ret_x3_entry_ptr;
  EntryPtr<state::Set> call_x4_entry_ptr, ret_x4_entry_ptr;
  EntryPtr<state::Set> call_x5_entry_ptr, ret_x5_entry_ptr;

  EntryPtr<state::Set> call_y0_entry_ptr, ret_y0_entry_ptr;

  call_x0_entry_ptr = log.add_call(state::Set::make_contains_call(x));
  call_x1_entry_ptr = log.add_call(state::Set::make_insert_call(x));
  ret_x0_entry_ptr = log.add_ret(call_x0_entry_ptr, state::Set::make_ret(false));
  call_y0_entry_ptr = log.add_call(state::Set::make_contains_call(y));
  ret_y0_entry_ptr = log.add_ret(call_y0_entry_ptr, state::Set::make_ret(true));
  call_x2_entry_ptr = log.add_call(state::Set::make_contains_call(x));
  ret_x2_entry_ptr = log.add_ret(call_x2_entry_ptr, state::Set::make_ret(false));
  call_x3_entry_ptr = log.add_call(state::Set::make_insert_call(x));
  ret_x3_entry_ptr = log.add_ret(call_x3_entry_ptr, state::Set::make_ret(true));
  call_x4_entry_ptr = log.add_call(state::Set::make_contains_call(x));
  ret_x4_entry_ptr = log.add_ret(call_x4_entry_ptr, state::Set::make_ret(true));
  ret_x1_entry_ptr = log.add_ret(call_x1_entry_ptr, state::Set::make_ret(false));
  call_x5_entry_ptr = log.add_call(state::Set::make_contains_call(x));
  ret_x5_entry_ptr = log.add_ret(call_x5_entry_ptr, state::Set::make_ret(true));

  assert(call_y0_entry_ptr->entry_id() == not_linearizable_entry_id);

  LinearizabilityTester<state::Set> t{ log.info() };
  assert(!t.check());
}

// !linearizable:
//
// Let x && y be distinct values.
//
//   contains(x) : false     contains(y) : true
// |---------------------| |--------------------|
//
//                          insert(x) : true
//                    |----------------------------|
static void test_015()
{
  constexpr char x = '\1';
  constexpr char y = '\2';

  constexpr unsigned contains_x_entry_id{ 0U };
  constexpr unsigned insert_x_entry_id{ 1U };
  constexpr unsigned contains_y_entry_id{ 2U };

  Log<state::Set> log{ 6U };

  EntryPtr<state::Set> contains_x_call_entry_ptr, contains_x_ret_entry_ptr;
  EntryPtr<state::Set> insert_x_call_entry_ptr, insert_x_ret_entry_ptr;
  EntryPtr<state::Set> contains_y_call_entry_ptr, contains_y_ret_entry_ptr;

  contains_x_call_entry_ptr = log.add_call(state::Set::make_contains_call(x));
  insert_x_call_entry_ptr = log.add_call(state::Set::make_insert_call(x));
  contains_x_ret_entry_ptr = log.add_ret(contains_x_call_entry_ptr, state::Set::make_ret(false));
  contains_y_call_entry_ptr = log.add_call(state::Set::make_contains_call(y));
  contains_y_ret_entry_ptr = log.add_ret(contains_y_call_entry_ptr, state::Set::make_ret(true));
  insert_x_ret_entry_ptr = log.add_ret(insert_x_call_entry_ptr, state::Set::make_ret(true));

  assert(contains_x_call_entry_ptr->entry_id() == contains_x_entry_id);
  assert(contains_x_ret_entry_ptr->entry_id() == contains_x_entry_id);

  assert(insert_x_call_entry_ptr->entry_id() == insert_x_entry_id);
  assert(insert_x_ret_entry_ptr->entry_id() == insert_x_entry_id);

  assert(contains_y_call_entry_ptr->entry_id() == contains_y_entry_id);
  assert(contains_y_ret_entry_ptr->entry_id() == contains_y_entry_id);

  LinearizabilityTester<state::Set> t{ log.info() };
  assert(!t.check());
}

// !linearizable:
//
// Let x && y be distinct values.
//
//   contains(x) : false     contains(y) : true     contains(x) : false
// |---------------------| |--------------------| |---------------------|
//
//                             insert(x) : true
//                    |------------------------------|
static void test_016()
{
  constexpr char x = '\1';
  constexpr char y = '\2';

  constexpr unsigned not_linearizable_entry_id = 2U;

  Log<state::Set> log{ 8U };

  EntryPtr<state::Set> call_x0_entry_ptr, ret_x0_entry_ptr;
  EntryPtr<state::Set> call_x1_entry_ptr, ret_x1_entry_ptr;
  EntryPtr<state::Set> call_x2_entry_ptr, ret_x2_entry_ptr;
  EntryPtr<state::Set> call_y0_entry_ptr, ret_y0_entry_ptr;

  call_x0_entry_ptr = log.add_call(state::Set::make_contains_call(x));
  call_x1_entry_ptr = log.add_call(state::Set::make_insert_call(x));
  ret_x0_entry_ptr = log.add_ret(call_x0_entry_ptr, state::Set::make_ret(false));
  call_y0_entry_ptr = log.add_call(state::Set::make_contains_call(y));
  ret_y0_entry_ptr = log.add_ret(call_y0_entry_ptr, state::Set::make_ret(true));
  call_x2_entry_ptr = log.add_call(state::Set::make_contains_call(x));
  ret_x1_entry_ptr = log.add_ret(call_x1_entry_ptr, state::Set::make_ret(true));
  ret_x2_entry_ptr = log.add_ret(call_x2_entry_ptr, state::Set::make_ret(false));

  assert(call_y0_entry_ptr->entry_id() == not_linearizable_entry_id);

  LinearizabilityTester<state::Set> t{ log.info() };
  assert(!t.check());
}

// !linearizable:
//
// Let x && y be distinct values.
//
//   contains(x) : false     contains(y) : true     contains(x) : false
// |---------------------| |--------------------| |---------------------|
//
//                                      insert(x) : true
//                    |-----------------------------------------------------|
static void test_017()
{
  constexpr char x = '\1';
  constexpr char y = '\2';

  constexpr unsigned not_linearizable_entry_id = 2U;

  Log<state::Set> log{ 8U };

  EntryPtr<state::Set> call_x0_entry_ptr, ret_x0_entry_ptr;
  EntryPtr<state::Set> call_x1_entry_ptr, ret_x1_entry_ptr;
  EntryPtr<state::Set> call_x2_entry_ptr, ret_x2_entry_ptr;
  EntryPtr<state::Set> call_y0_entry_ptr, ret_y0_entry_ptr;

  call_x0_entry_ptr = log.add_call(state::Set::make_contains_call(x));
  call_x1_entry_ptr = log.add_call(state::Set::make_insert_call(x));
  ret_x0_entry_ptr = log.add_ret(call_x0_entry_ptr, state::Set::make_ret(false));
  call_y0_entry_ptr = log.add_call(state::Set::make_contains_call(y));
  ret_y0_entry_ptr = log.add_ret(call_y0_entry_ptr, state::Set::make_ret(true));
  call_x2_entry_ptr = log.add_call(state::Set::make_contains_call(x));
  ret_x2_entry_ptr = log.add_ret(call_x2_entry_ptr, state::Set::make_ret(false));
  ret_x1_entry_ptr = log.add_ret(call_x1_entry_ptr, state::Set::make_ret(true));

  assert(call_y0_entry_ptr->entry_id() == not_linearizable_entry_id);

  LinearizabilityTester<state::Set> t{ log.info() };
  assert(!t.check());
}

// !linearizable:
//
// Let x && y be distinct values.
//
//      contains(y) : true     insert(x) : true
//    |--------------------| |------------------|
//
//                insert(x) : false
// |------------------------------------------------|
static void test_018()
{
  constexpr char x = '\1';
  constexpr char y = '\2';

  constexpr unsigned not_linearizable_entry_id = 1U;

  Log<state::Set> log{ 8U };

  EntryPtr<state::Set> call_x0_entry_ptr, ret_x0_entry_ptr;
  EntryPtr<state::Set> call_x1_entry_ptr, ret_x1_entry_ptr;
  EntryPtr<state::Set> call_y0_entry_ptr, ret_y0_entry_ptr;

  call_x0_entry_ptr = log.add_call(state::Set::make_insert_call(x));
  call_y0_entry_ptr = log.add_call(state::Set::make_contains_call(y));
  ret_y0_entry_ptr = log.add_ret(call_y0_entry_ptr, state::Set::make_ret(true));
  call_x1_entry_ptr = log.add_call(state::Set::make_insert_call(x));
  ret_x1_entry_ptr = log.add_ret(call_x1_entry_ptr, state::Set::make_ret(true));
  ret_x0_entry_ptr = log.add_ret(call_x1_entry_ptr, state::Set::make_ret(false));

  assert(call_y0_entry_ptr->entry_id() == not_linearizable_entry_id);

  LinearizabilityTester<state::Set> t{ log.info() };
  assert(!t.check());
}

// Linearizable:
//
//   insert(x) : insert_ret
// |------------------------|
//
//               contains(x) : contains_ret
//            |-----------------------------|
//
//                 empty() : empty_ret 
//             |------------------------|
static void test_019(bool insert_ret, bool contains_ret, bool empty_ret)
{
  constexpr char x = '\1';

  Log<state::Set> log{ 8U };

  EntryPtr<state::Set> call_insert_entry_ptr, ret_insert_entry_ptr;
  EntryPtr<state::Set> call_contains_entry_ptr, ret_contains_entry_ptr;
  EntryPtr<state::Set> call_empty_entry_ptr, ret_empty_entry_ptr;

  call_insert_entry_ptr = log.add_call(state::Set::make_insert_call(x));
  call_contains_entry_ptr = log.add_call(state::Set::make_contains_call(x));
  call_empty_entry_ptr = log.add_call(state::Set::make_empty_call());
  ret_insert_entry_ptr = log.add_ret(call_insert_entry_ptr, state::Set::make_ret(insert_ret));
  ret_empty_entry_ptr = log.add_ret(call_empty_entry_ptr, state::Set::make_ret(empty_ret));
  ret_contains_entry_ptr = log.add_ret(call_contains_entry_ptr, state::Set::make_ret(contains_ret));

  LinearizabilityTester<state::Set> t{ log.info() };
  assert(t.check() == insert_ret);
}

//                                                          insert(x) : insert_ret
//                                                        |------------------------|
//
//   contains(x) :  contains_ret    contains(x) : contains_ret
// |----------------------------| |----------------------------|
//
//                                                     empty() : empty_ret
//                                                   |----------------------|
static void test_020(bool insert_ret, bool contains_ret, bool empty_ret)
{
  constexpr char x = '\1';

  Log<state::Set> log{ 8U };

  EntryPtr<state::Set> call_contains_0_entry_ptr, ret_contains_0_entry_ptr;
  EntryPtr<state::Set> call_contains_1_entry_ptr, ret_contains_1_entry_ptr;
  EntryPtr<state::Set> call_insert_entry_ptr, ret_insert_entry_ptr;
  EntryPtr<state::Set> call_empty_entry_ptr, ret_empty_entry_ptr;

  call_contains_0_entry_ptr = log.add_call(state::Set::make_contains_call(x));
  ret_contains_0_entry_ptr = log.add_ret(call_contains_0_entry_ptr, state::Set::make_ret(contains_ret));
  call_contains_1_entry_ptr = log.add_call(state::Set::make_contains_call(x));
  call_empty_entry_ptr = log.add_call(state::Set::make_empty_call());
  call_insert_entry_ptr = log.add_call(state::Set::make_insert_call(x));
  ret_contains_1_entry_ptr = log.add_ret(call_contains_1_entry_ptr, state::Set::make_ret(contains_ret));
  ret_empty_entry_ptr = log.add_ret(call_empty_entry_ptr, state::Set::make_ret(empty_ret));
  ret_insert_entry_ptr = log.add_ret(call_insert_entry_ptr, state::Set::make_ret(insert_ret));

  LinearizabilityTester<state::Set> t{ log.info() };
  assert(t.check() == (insert_ret && !contains_ret));
}

// Linearizable:
//
// Let x && y be distinct values.
//
//   contains(x) : false
// |---------------------|
//
//              insert(y) : true
//           |-----------------------------------------------------------|
//
//                            contains(x) : false      empty() : true
//                          |---------------------|  |----------------|
static void test_021()
{
  constexpr char x = '\1';
  constexpr char y = '\2';

  Log<state::Set> log{ 8U };

  EntryPtr<state::Set> call_contains_0_entry_ptr, ret_contains_0_entry_ptr;
  EntryPtr<state::Set> call_contains_1_entry_ptr, ret_contains_1_entry_ptr;
  EntryPtr<state::Set> call_insert_entry_ptr, ret_insert_entry_ptr;
  EntryPtr<state::Set> call_empty_entry_ptr, ret_empty_entry_ptr;

  call_contains_0_entry_ptr = log.add_call(state::Set::make_contains_call(x));
  call_insert_entry_ptr = log.add_call(state::Set::make_insert_call(y));
  ret_contains_0_entry_ptr = log.add_ret(call_contains_0_entry_ptr, state::Set::make_ret(false));
  call_contains_1_entry_ptr = log.add_call(state::Set::make_contains_call(x));
  ret_contains_1_entry_ptr = log.add_ret(call_contains_1_entry_ptr, state::Set::make_ret(false));
  call_empty_entry_ptr = log.add_call(state::Set::make_empty_call());
  ret_empty_entry_ptr = log.add_ret(call_empty_entry_ptr, state::Set::make_ret(true));
  ret_insert_entry_ptr = log.add_ret(call_insert_entry_ptr, state::Set::make_ret(1));

  LinearizabilityTester<state::Set> t{ log.info() };
  assert(t.check());
}

// Linearizable:
//
//        push(x) : true
// |------------------------|
//
//        push(y) : true
//   |--------------------|
//
//      pop() : (true, x)
//    |------------------|
static void test_stack_history_000()
{
  constexpr char x = '\1';
  constexpr char y = '\2';
  constexpr std::size_t N = 5;

  Log<state::Stack<N>> log{ 6U };
  EntryPtr<state::Stack<N>> try_push_x_call_entry_ptr, try_push_x_ret_entry_ptr;
  EntryPtr<state::Stack<N>> try_push_y_call_entry_ptr, try_push_y_ret_entry_ptr;
  EntryPtr<state::Stack<N>> try_pop_x_call_entry_ptr, try_pop_x_ret_entry_ptr;

  try_push_x_call_entry_ptr = log.add_call(state::Stack<N>::make_try_push_call(x));
  try_push_y_call_entry_ptr = log.add_call(state::Stack<N>::make_try_push_call(y));
  try_pop_x_call_entry_ptr = log.add_call(state::Stack<N>::make_try_pop_call());
  try_push_x_ret_entry_ptr = log.add_ret(try_push_x_call_entry_ptr, state::Stack<N>::make_try_push_ret(true));
  try_push_y_ret_entry_ptr = log.add_ret(try_push_y_call_entry_ptr, state::Stack<N>::make_try_push_ret(true));
  try_pop_x_ret_entry_ptr = log.add_ret(try_pop_x_call_entry_ptr, state::Stack<N>::make_try_pop_ret(true, x));

  assert(!try_push_x_call_entry_ptr->is_partitionable());
  assert(!try_push_y_call_entry_ptr->is_partitionable());
  assert(!try_pop_x_call_entry_ptr->is_partitionable());

  assert(!try_push_x_ret_entry_ptr->is_partitionable());
  assert(!try_push_y_ret_entry_ptr->is_partitionable());
  assert(!try_pop_x_ret_entry_ptr->is_partitionable());

  LinearizabilityTester<state::Stack<N>> t{ log.info() };
  assert(t.check());
}

// push(x):true; push(y):true; pop():x
static void test_stack_history_001()
{
  constexpr char x = '\1';
  constexpr char y = '\2';
  constexpr std::size_t N = 5;

  Log<state::Stack<N>> log{ 6U };
  EntryPtr<state::Stack<N>> try_push_x_call_entry_ptr, try_push_x_ret_entry_ptr;
  EntryPtr<state::Stack<N>> try_push_y_call_entry_ptr, try_push_y_ret_entry_ptr;
  EntryPtr<state::Stack<N>> try_pop_x_call_entry_ptr, try_pop_x_ret_entry_ptr;

  try_push_x_call_entry_ptr = log.add_call(state::Stack<N>::make_try_push_call(x));
  try_push_x_ret_entry_ptr = log.add_ret(try_push_x_call_entry_ptr, state::Stack<N>::make_try_push_ret(true));
  try_push_y_call_entry_ptr = log.add_call(state::Stack<N>::make_try_push_call(y));
  try_push_y_ret_entry_ptr = log.add_ret(try_push_y_call_entry_ptr, state::Stack<N>::make_try_push_ret(true));
  try_pop_x_call_entry_ptr = log.add_call(state::Stack<N>::make_try_pop_call());
  try_pop_x_ret_entry_ptr = log.add_ret(try_pop_x_call_entry_ptr, state::Stack<N>::make_try_pop_ret(true, x));

  assert(!try_push_x_call_entry_ptr->is_partitionable());
  assert(!try_push_y_call_entry_ptr->is_partitionable());
  assert(!try_pop_x_call_entry_ptr->is_partitionable());

  assert(!try_push_x_ret_entry_ptr->is_partitionable());
  assert(!try_push_y_ret_entry_ptr->is_partitionable());
  assert(!try_pop_x_ret_entry_ptr->is_partitionable());

  LinearizabilityTester<state::Stack<N>> t{ log.info() };
  assert(!t.check());
}

// entry id: 0, thread id: 0x2, call: try_push(x)
// entry id: 0, thread id: 0x2, return: ret: 1
// entry id: 1, thread id: 0x3, call: try_push(y)
// entry id: 2, thread id: 0x4, call: try_pop()
// entry id: 2, thread id: 0x4, return: ret: [ok: 1, value: x]
// entry id: 1, thread id: 0x3, return: ret: 1
static void test_stack_history_002()
{
  constexpr char x = '\1';
  constexpr char y = '\2';
  constexpr std::size_t N = 5;

  Log<state::Stack<N>> log{ 8U };
  EntryPtr<state::Stack<N>> try_push_x_call_entry_ptr, try_push_x_ret_entry_ptr;
  EntryPtr<state::Stack<N>> try_push_y_call_entry_ptr, try_push_y_ret_entry_ptr;
  EntryPtr<state::Stack<N>> try_pop_x_call_entry_ptr, try_pop_x_ret_entry_ptr;

  try_push_x_call_entry_ptr = log.add_call(state::Stack<N>::make_try_push_call(x));
  try_push_x_ret_entry_ptr = log.add_ret(try_push_x_call_entry_ptr, state::Stack<N>::make_try_push_ret(true));
  try_push_y_call_entry_ptr = log.add_call(state::Stack<N>::make_try_push_call(y));
  try_pop_x_call_entry_ptr = log.add_call(state::Stack<N>::make_try_pop_call());
  try_pop_x_ret_entry_ptr = log.add_ret(try_pop_x_call_entry_ptr, state::Stack<N>::make_try_pop_ret(true, x));
  try_push_y_ret_entry_ptr = log.add_ret(try_push_y_call_entry_ptr, state::Stack<N>::make_try_push_ret(true));

  assert(!try_push_x_call_entry_ptr->is_partitionable());
  assert(!try_push_y_call_entry_ptr->is_partitionable());
  assert(!try_pop_x_call_entry_ptr->is_partitionable());

  assert(!try_push_x_ret_entry_ptr->is_partitionable());
  assert(!try_push_y_ret_entry_ptr->is_partitionable());
  assert(!try_pop_x_ret_entry_ptr->is_partitionable());

  LinearizabilityTester<state::Stack<N>> t{ log.info() };
  assert(t.check());
}

// Linearizable:
//
//   push(x) : true       pop() : (true, y)
// |----------------|   |------------------|
//
//     push(y) : true                           push(z) : true
//   |----------------|                |---------------------------------|
//
//                                                  pop() : (true, x)     pop() : (true, z)
//                                               |-------------------| |-------------------|
static void test_stack_history_003()
{
  constexpr char x = '\1';
  constexpr char y = '\2';
  constexpr char z = '\3';
  constexpr std::size_t N = 5;

  Log<state::Stack<N>> log{ 12U };
  EntryPtr<state::Stack<N>> try_push_x_call_entry_ptr, try_push_x_ret_entry_ptr;
  EntryPtr<state::Stack<N>> try_push_y_call_entry_ptr, try_push_y_ret_entry_ptr;
  EntryPtr<state::Stack<N>> try_push_z_call_entry_ptr, try_push_z_ret_entry_ptr;
  EntryPtr<state::Stack<N>> try_pop_x_call_entry_ptr, try_pop_x_ret_entry_ptr;
  EntryPtr<state::Stack<N>> try_pop_y_call_entry_ptr, try_pop_y_ret_entry_ptr;
  EntryPtr<state::Stack<N>> try_pop_z_call_entry_ptr, try_pop_z_ret_entry_ptr;

  try_push_x_call_entry_ptr = log.add_call(state::Stack<N>::make_try_push_call(x));
  try_push_y_call_entry_ptr = log.add_call(state::Stack<N>::make_try_push_call(y));
  try_push_x_ret_entry_ptr = log.add_ret(try_push_x_call_entry_ptr, state::Stack<N>::make_try_push_ret(true));
  try_push_y_ret_entry_ptr = log.add_ret(try_push_y_call_entry_ptr, state::Stack<N>::make_try_push_ret(true));
  try_pop_y_call_entry_ptr = log.add_call(state::Stack<N>::make_try_pop_call());
  try_push_z_call_entry_ptr = log.add_call(state::Stack<N>::make_try_push_call(z));
  try_pop_y_ret_entry_ptr = log.add_ret(try_pop_y_call_entry_ptr, state::Stack<N>::make_try_pop_ret(true, y));
  try_pop_x_call_entry_ptr = log.add_call(state::Stack<N>::make_try_pop_call());
  try_pop_x_ret_entry_ptr = log.add_ret(try_pop_x_call_entry_ptr, state::Stack<N>::make_try_pop_ret(true, x));
  try_pop_z_call_entry_ptr = log.add_call(state::Stack<N>::make_try_pop_call());
  try_push_z_ret_entry_ptr = log.add_ret(try_push_z_call_entry_ptr, state::Stack<N>::make_try_push_ret(true));
  try_pop_z_ret_entry_ptr = log.add_ret(try_pop_z_call_entry_ptr, state::Stack<N>::make_try_pop_ret(true, z));

  LinearizabilityTester<state::Stack<N>> t{ log.info() };
  assert(t.check());
}


// Let x = '\0' && y = '\1'.
//
//   erase(x) : false     insert(y) : true
// |------------------| |------------------|
//
//                                               contains(x) : true
//                                 |------------------------------------------------|
//
//                                      contains(y) : false     insert(x) : true
//                                    |---------------------| |------------------|
static void test_slice_000()
{
  constexpr char x = '\0';
  constexpr char y = '\1';

  Log<state::Set> log{ 10U };

  EntryPtr<state::Set> erase_x_call_entry_ptr, erase_x_ret_entry_ptr;
  EntryPtr<state::Set> insert_x_call_entry_ptr, insert_x_ret_entry_ptr;
  EntryPtr<state::Set> contains_x_call_entry_ptr, contains_x_ret_entry_ptr;
  EntryPtr<state::Set> insert_y_call_entry_ptr, insert_y_ret_entry_ptr;
  EntryPtr<state::Set> contains_y_call_entry_ptr, contains_y_ret_entry_ptr;

  erase_x_call_entry_ptr = log.add_call(state::Set::make_erase_call(x));
  erase_x_ret_entry_ptr = log.add_ret(erase_x_call_entry_ptr, state::Set::make_ret(false));
  insert_y_call_entry_ptr = log.add_call(state::Set::make_insert_call(y));
  contains_x_call_entry_ptr = log.add_call(state::Set::make_contains_call(x));
  contains_y_call_entry_ptr = log.add_call(state::Set::make_contains_call(y));
  insert_y_ret_entry_ptr = log.add_ret(insert_y_call_entry_ptr, state::Set::make_ret(true));
  contains_y_ret_entry_ptr = log.add_ret(contains_y_call_entry_ptr, state::Set::make_ret(false));
  insert_x_call_entry_ptr = log.add_call(state::Set::make_insert_call(x));
  insert_x_ret_entry_ptr = log.add_ret(insert_x_call_entry_ptr, state::Set::make_ret(true));
  contains_x_ret_entry_ptr = log.add_ret(contains_x_call_entry_ptr, state::Set::make_ret(true));

  Slicer<state::Set> slicer{ log.info(), 2U };

  assert(slicer.sublog_info(x).log_head_ptr() == erase_x_call_entry_ptr);
  assert(slicer.sublog_info(y).log_head_ptr() == insert_y_call_entry_ptr);

  assert(slicer.sublog_info(x).number_of_entries() == 6U);
  assert(slicer.sublog_info(y).number_of_entries() == 4U);

  LinearizabilityTester<state::Set> tester_0{ slicer.sublog_info(x) };
  assert(tester_0.check());

  LinearizabilityTester<state::Set> tester_1{ slicer.sublog_info(y) };
  assert(tester_1.check());
}

// Let x = '\0' && y = '\1'.
//
//   contains(x) : false 
// |---------------------|
//
//      contains(y) : false
//    |---------------------|
//
//          insert(x) : false
//        |---------------------|
static void test_slice_001()
{
  constexpr char x = '\0';
  constexpr char y = '\1';

  Log<state::Set> log{ 6U };
  EntryPtr<state::Set> contains_x_call_entry_ptr, contains_x_ret_entry_ptr;
  EntryPtr<state::Set> contains_y_call_entry_ptr, contains_y_ret_entry_ptr;
  EntryPtr<state::Set> insert_x_call_entry_ptr, insert_x_ret_entry_ptr;

  contains_x_call_entry_ptr = log.add_call(state::Set::make_contains_call(x));
  contains_y_call_entry_ptr = log.add_call(state::Set::make_contains_call(y));
  insert_x_call_entry_ptr = log.add_call(state::Set::make_insert_call(x));

  contains_x_ret_entry_ptr = log.add_ret(contains_x_call_entry_ptr, state::Set::make_ret(false));
  contains_y_ret_entry_ptr = log.add_ret(contains_y_call_entry_ptr, state::Set::make_ret(false));
  insert_x_ret_entry_ptr = log.add_ret(insert_x_call_entry_ptr, state::Set::make_ret(false));

  Slicer<state::Set> slicer{ log.info(), 2U };

  assert(slicer.sublog_info(x).log_head_ptr() == contains_x_call_entry_ptr);
  assert(slicer.sublog_info(y).log_head_ptr() == contains_y_call_entry_ptr);

  assert(slicer.sublog_info(x).number_of_entries() == 4U);
  assert(slicer.sublog_info(y).number_of_entries() == 2U);

  LinearizabilityTester<state::Set> tester_0{ slicer.sublog_info(x) };
  assert(!tester_0.check());

  LinearizabilityTester<state::Set> tester_1{ slicer.sublog_info(y) };
  assert(tester_1.check());
}

static void debug()
{
  constexpr char x = '\1';

  Log<state::Set> log{ 2U };

  EntryPtr<state::Set> contains_call_entry_ptr, contains_ret_entry_ptr;
  contains_call_entry_ptr = log.add_call(state::Set::make_contains_call(x));
  contains_ret_entry_ptr = log.add_ret(contains_call_entry_ptr, state::Set::make_ret(true));

  LinearizabilityTester<state::Set> t{ log.info() };
  Result<state::Set> result;

  t.check(result);
  assert(!result.is_linearizable());

  std::stringstream os;
  result.debug(os);

  assert(os.str() == "Linearizable: No\n"
    "entry id: 0, thread id: 0, call: contains(1)\n"
    "^ previous entries cannot be linearized\n"
    "entry id: 0, thread id: 0, return: ret: 1\n"
    "^ previous entries cannot be linearized\n");
}


static void concurrent_log()
{
  constexpr unsigned number_of_partitions = 1U;

  constexpr char x = '\0';

  ConcurrentLog<state::Set> log{ 6U };

  EntryPtr<state::Set> contains_x_call_entry_ptr, contains_x_ret_entry_ptr;
  EntryPtr<state::Set> insert_x_call_entry_ptr, insert_x_ret_entry_ptr;

  contains_x_call_entry_ptr = log.push_back(state::Set::make_contains_call(x));
  insert_x_call_entry_ptr = log.push_back(state::Set::make_insert_call(x));
  contains_x_ret_entry_ptr = log.push_back(contains_x_call_entry_ptr, state::Set::make_ret(false));
  insert_x_ret_entry_ptr = log.push_back(insert_x_call_entry_ptr, state::Set::make_ret(false));

  EntryPtr<state::Set> entry_ptr, log_head_ptr{ log.log_head_ptr() };

  assert(log_head_ptr == contains_x_call_entry_ptr);
  assert(log_head_ptr->prev == nullptr);
  assert(log_head_ptr->next == insert_x_call_entry_ptr);
  assert(log_head_ptr->match() == contains_x_ret_entry_ptr);

  entry_ptr = log_head_ptr->next;
  assert(entry_ptr == insert_x_call_entry_ptr);
  assert(entry_ptr->prev == contains_x_call_entry_ptr);
  assert(entry_ptr->next == contains_x_ret_entry_ptr);
  assert(entry_ptr->match() == insert_x_ret_entry_ptr);

  entry_ptr = entry_ptr->next;
  assert(entry_ptr == contains_x_ret_entry_ptr);
  assert(entry_ptr->prev == insert_x_call_entry_ptr);
  assert(entry_ptr->next == insert_x_ret_entry_ptr);
  assert(entry_ptr->match() == contains_x_call_entry_ptr);

  entry_ptr = entry_ptr->next;
  assert(entry_ptr == insert_x_ret_entry_ptr);
  assert(entry_ptr->prev == contains_x_ret_entry_ptr);
  assert(entry_ptr->next == nullptr);
  assert(entry_ptr->match() == insert_x_call_entry_ptr);

  LinearizabilityTester<state::Set> t{ log.info() };
  assert(!t.check());
}

struct WorkerConfiguration
{
  const char max_value;
  const unsigned number_of_ops;
};

static void returns_always_false_worker(
  const WorkerConfiguration& worker_configuration,
  ConcurrentLog<state::Set>& concurrent_log)
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> value_dist('\0', worker_configuration.max_value);
  std::uniform_int_distribution<> percentage_dist(0, 100);

  // each operation returns false
  constexpr bool ret = false;

  char value;
  EntryPtr<state::Set> call_entry_ptr;
  for (unsigned number_of_ops{ 0U };
  number_of_ops < worker_configuration.number_of_ops;
    ++number_of_ops)
  {
    value = value_dist(rd);
    if (percentage_dist(rd) < 30)
    {
      call_entry_ptr = concurrent_log.push_back(state::Set::make_insert_call(value));
      concurrent_log.push_back(call_entry_ptr, state::Set::make_ret(ret));
    }
    else if (percentage_dist(rd) < 50)
    {
      call_entry_ptr = concurrent_log.push_back(state::Set::make_erase_call(value));
      concurrent_log.push_back(call_entry_ptr, state::Set::make_ret(ret));
    }
    else
    {
      call_entry_ptr = concurrent_log.push_back(state::Set::make_contains_call(value));
      concurrent_log.push_back(call_entry_ptr, state::Set::make_ret(ret));
    }
  }
}

template<class F, class ...Args>
void start_threads(unsigned number_of_threads, F&& f, Args&&... args)
{
  std::vector<Thread> threads(number_of_threads);

  for (Thread& thread : threads)
    thread = Thread(std::forward<F>(f), std::forward<Args>(args)...);

  for (Thread& thread : threads)
    thread.join();
}

/// The first "insert" operation should be always marked as non-linearizable.
static void fuzzy_functional_test()
{
  constexpr unsigned number_of_threads = 4U;
  constexpr WorkerConfiguration worker_configuration = { '\7', 12U };
  constexpr unsigned log_size = number_of_threads * worker_configuration.number_of_ops;

  ConcurrentLog<state::Set> concurrent_log{ 2U * log_size };

  // create history
  start_threads(number_of_threads, returns_always_false_worker,
    std::cref(worker_configuration), std::ref(concurrent_log));

  LinearizabilityTester<state::Set> tester{ concurrent_log.info() };

  assert(!tester.check());
}

void run_tests(){
  test_lru_cache();
  test_atomic_op();
  test_stack();
  test_state_set();
  test_bitset();
  test_state_set_op();
  test_state_stack();
  test_state_stack_op();
  test_state_queue();
  test_state_queue_op();
  
  test_linearizability_empty_log();
  test_raw_single_contains_is_linearizable();
  test_raw_single_contains_is_not_linearizable();
  test_log_copy();
  test_single_contains(true);
  test_single_contains(false);

  // Consider a sequential insert(x) && contains(x) operation
  // && their return values on an initially empty set:

  for (bool a : {true, false})
  for (bool b : {true, false})
  {
  test_000(a, b);
  test_001(a, b);
  test_002(a, b);
  test_003(a, b);
  test_004(a, b);
  test_005(a, b);
  test_006(a, b);
  test_007(a, b);
  test_008(a, b);
  }

  // semantically deeper tests

  test_009();
  test_010();
  test_011();
  test_012();
  test_013();
  test_014();
  test_015();
  test_016();
  test_017();
  test_018();

  for (bool a : {true, false})
  for (bool b : {true, false})
  for (bool c : {true, false})
  {
  test_019(a, b, c);
  test_020(a, b, c);
  }

  test_021();

  test_stack_history_000();
  test_stack_history_001();
  test_stack_history_002();
  test_stack_history_003();

  test_slice_000();
  test_slice_001();

  #ifdef _LT_DEBUG_
    debug();
  #endif

  concurrent_log();
  fuzzy_functional_test();

}