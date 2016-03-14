#include <linearizability_tester.h>

#include <embb/base/thread.h>
#include <embb/containers/lock_free_stack.h>
#include <embb/containers/lock_free_mpmc_queue.h>

template<std::size_t N, class S>
static void embb_worker_stack(
	const WorkerConfiguration& worker_configuration,
	ConcurrentLog<state::Stack<N>>& concurrent_log,
	S& concurrent_stack)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> value_dist('\0', worker_configuration.max_value);
	std::uniform_int_distribution<> percentage_dist(0, 100);

	// each operation returns false
	bool ret;

	char value;
	unsigned percentage;
	EntryPtr<state::Stack<N>> call_entry_ptr;
	for (unsigned number_of_ops{ 0U };
	number_of_ops < worker_configuration.number_of_ops;
		++number_of_ops)
	{
		value = value_dist(rd);
		percentage = percentage_dist(rd);
		if (percentage < 30)
		{
			call_entry_ptr = concurrent_log.push_back(state::Stack<N>::make_try_push_call(value));
			ret = concurrent_stack.TryPush(value);
			concurrent_log.push_back(call_entry_ptr, state::Stack<N>::make_try_push_ret(ret));
		}
		else
		{
			call_entry_ptr = concurrent_log.push_back(state::Stack<N>::make_try_pop_call());
			ret = concurrent_stack.TryPop(value);
			concurrent_log.push_back(call_entry_ptr, state::Stack<N>::make_try_pop_ret(ret, value));
		}
	}
}

template<std::size_t N, class S>
static void embb_worker_queue(
	const WorkerConfiguration& worker_configuration,
	ConcurrentLog<state::Queue<N>>& concurrent_log,
	S& concurrent_queue)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> value_dist('\0', worker_configuration.max_value);
	std::uniform_int_distribution<> percentage_dist(0, 100);

	// each operation returns false
	bool ret;

	char value;
	unsigned percentage;
	EntryPtr<state::Queue<N>> call_entry_ptr;
	for (unsigned number_of_ops{ 0U };
	number_of_ops < worker_configuration.number_of_ops;
		++number_of_ops)
	{
		value = value_dist(rd);
		percentage = percentage_dist(rd);
		if (percentage < 20)
		{
			call_entry_ptr = concurrent_log.push_back(state::Queue<N>::make_try_enqueue_call(value));
			ret = concurrent_queue.TryEnqueue(value);
			concurrent_log.push_back(call_entry_ptr, state::Queue<N>::make_try_enqueue_ret(ret));
		}
		else
		{
			call_entry_ptr = concurrent_log.push_back(state::Queue<N>::make_try_dequeue_call());
			ret = concurrent_queue.TryDequeue(value);
			concurrent_log.push_back(call_entry_ptr, state::Queue<N>::make_try_dequeue_ret(ret, value));
		}
	}
}

template <class S>
static void embb_experiment_stack(bool is_linearizable)
{
	constexpr std::chrono::hours max_duration{ 1 };
	constexpr std::size_t N = 560000U;
	constexpr unsigned number_of_threads = 4U;
	constexpr WorkerConfiguration worker_configuration = { '\24', 70000U };
	constexpr unsigned log_size = number_of_threads * worker_configuration.number_of_ops;

	std::cout << "embb_experiment_stack : " << (is_linearizable ? "" : "!") << "linearizable" << std::endl;

	Result<state::Stack<N>> result;
	ConcurrentLog<state::Stack<N>> concurrent_log{ 2U * log_size };
	S concurrent_stack(N);

	if (!is_linearizable)
	{
		bool ok = concurrent_stack.TryPush(5);
		assert(ok);
	}

	// create history
	start_threads(number_of_threads, embb_worker_stack<N, S>, std::cref(worker_configuration),
		std::ref(concurrent_log), std::ref(concurrent_stack));

	const std::size_t number_of_entries{ concurrent_log.number_of_entries() };
	const LogInfo<state::Stack<N>> log_info{ concurrent_log.info() };
	// std::cout << log_info << std::endl;

	auto start = std::chrono::system_clock::now();
	auto end = std::chrono::system_clock::now();
	std::chrono::seconds seconds;

	start = std::chrono::system_clock::now();
	{
		Log<state::Stack<N>> log_copy{ log_info };
		assert(log_copy.number_of_entries() == number_of_entries);

		LinearizabilityTester<state::Stack<N>, Option::LRU_CACHE> tester{ log_copy.info(), max_duration };
		tester.check(result);
		assert(result.is_timeout() || result.is_linearizable() == is_linearizable);
	}
	end = std::chrono::system_clock::now();
	seconds = std::chrono::duration_cast<std::chrono::seconds>(end - start);
	std::cout << "History length: " << number_of_entries
		<< ", enabled state cache (LRU=on), O(1) hash: "
		<< seconds.count() << " s " << std::endl;
}

template <class S>
static void embb_experiment_queue(bool is_linearizable)
{
	constexpr std::chrono::hours max_duration{ 1 };
	constexpr std::size_t N = 560000U;
	constexpr unsigned number_of_threads = 4U;
	constexpr WorkerConfiguration worker_configuration = { '\24', 70000U };
	constexpr unsigned log_size = number_of_threads * worker_configuration.number_of_ops;

	std::cout << "embb_experiment_queue : " << (is_linearizable ? "" : "!") << "linearizable" << std::endl;

	Result<state::Queue<N>> result;
	ConcurrentLog<state::Queue<N>> concurrent_log{ 2U * log_size };
	S concurrent_queue(N);

	if (!is_linearizable)
	{
		bool ok = concurrent_queue.TryEnqueue(5);
		assert(ok);
	}

	// create history
	start_threads(number_of_threads, embb_worker_queue<N, S>, std::cref(worker_configuration),
		std::ref(concurrent_log), std::ref(concurrent_queue));
	const std::size_t number_of_entries{ concurrent_log.number_of_entries() };
	const LogInfo<state::Queue<N>> log_info{ concurrent_log.info() };
	// std::cout << log_info << std::endl;

	auto start = std::chrono::system_clock::now();
	auto end = std::chrono::system_clock::now();
	std::chrono::seconds seconds;

	start = std::chrono::system_clock::now();
	{
		Log<state::Queue<N>> log_copy{ log_info };
		assert(log_copy.number_of_entries() == number_of_entries);
		LinearizabilityTester<state::Queue<N>, Option::LRU_CACHE> tester{ log_copy.info(), max_duration };
		tester.check(result);
		assert(result.is_timeout() || result.is_linearizable() == is_linearizable);
	}
	end = std::chrono::system_clock::now();
	seconds = std::chrono::duration_cast<std::chrono::seconds>(end - start);
	std::cout << "History length: " << number_of_entries
		<< ", enabled state cache (LRU=on), O(1) hash: "
		<< seconds.count() << " s " << std::endl;
}


int main()
{	

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
   // debug();
  #endif

  concurrent_log();
  fuzzy_functional_test();


  embb::base::Thread::SetThreadsMaxCount(255);
  
  std::cout << "Linearizability experiment on LockFreeMPMCQueue" << std::endl;
  embb_experiment_queue<embb::containers::LockFreeMPMCQueue<char>>(true);

  std::cout << "Linearizability experiment on LockFreeStack" << std::endl;
  embb_experiment_stack<embb::containers::LockFreeStack<char>>(true);
  return 0;
}

