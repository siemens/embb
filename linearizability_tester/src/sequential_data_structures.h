/**
/* Sequential implementation of the datastractures to test
*/

#ifndef __SEQUENTIAL_DATASTRUCTURES
#define __SEQUENTIAL_DATASTRUCTURES

namespace lt{
namespace state{
class Set
{
 public:
  typedef char Value;
  typedef bool Ret;

 private:
  static constexpr char s_empty_op_name[6] = "empty";
  static constexpr char s_contains_op_name[9] = "contains";
  static constexpr char s_insert_op_name[7] = "insert";
  static constexpr char s_erase_op_name[6] = "erase";

 public:
  struct RetOp : public internal::RetOp<Set, Ret>
  {
    RetOp(Ret r) : Base(r) {}
  };

  struct EmptyCallOp : public internal::ZeroArgOp<Set, s_empty_op_name>
  {
    EmptyCallOp() : Base() {}

    std::pair<bool, Set> internal_apply(const Set& set, const Op<Set>& op) override
    {
      const RetOp& empty_ret = dynamic_cast<const RetOp&>(op);
      bool ret = set.is_empty();
      return{ ret == empty_ret.ret, set };
    }
};

  struct ContainsCallOp : public internal::ArgOp<Set, Value, s_contains_op_name>
  {
    ContainsCallOp(Value v) : Base(v) {}

    std::pair<bool, Set> internal_apply(const Set& set, const Op<Set>& op) override
    {
      const RetOp& contains_ret = dynamic_cast<const RetOp&>(op);
      bool ret = set.contains(value);
      return{ ret == contains_ret.ret, set };
    }
  };

  struct InsertCallOp : public internal::ArgOp<Set, Value, s_insert_op_name>
  {
    InsertCallOp(Value v) : Base(v) {}

    std::pair<bool, Set> internal_apply(const Set& set, const Op<Set>& op) override
    {
      bool ret;
      Set new_set;
      const RetOp& insert_ret = dynamic_cast<const RetOp&>(op);
      std::tie(ret, new_set) = set.insert(value);
      return{ ret == insert_ret.ret, std::move(new_set) };
    }
  };

  struct EraseCallOp : public internal::ArgOp<Set, Value, s_erase_op_name>
  {
    EraseCallOp(Value v) : Base(v) {}

    std::pair<bool, Set> internal_apply(const Set& set, const Op<Set>& op) override
    {
      bool ret;
      Set new_set;
      const RetOp& erase_ret = dynamic_cast<const RetOp&>(op);
      std::tie(ret, new_set) = set.erase(value);
      return{ ret == erase_ret.ret, std::move(new_set) };
    }
  };

  typedef std::unique_ptr<Op<Set>> SetOpPtr;
  FlexibleBitset m_bitset;

  Set(FlexibleBitset&& bitset)
    : m_bitset(std::move(bitset)) {}

 public:
  static SetOpPtr make_empty_call()
  {
    return make_unique<EmptyCallOp>();
  }

  static SetOpPtr make_contains_call(Value value)
  {
    return make_unique<ContainsCallOp>(value);
  }

  static SetOpPtr make_insert_call(Value value)
  {
    return make_unique<InsertCallOp>(value);
  }

  static SetOpPtr make_erase_call(Value value)
  {
    return make_unique<EraseCallOp>(value);
  }

  static SetOpPtr make_ret(Ret ret)
  {
    return make_unique<RetOp>(ret);
  }

  Set()
    : m_bitset{} {}

  const FlexibleBitset& bitset() const
  {
    return m_bitset;
  }

  bool is_empty() const
  {
    return m_bitset.is_empty();
  }

  bool contains(const Value& value) const
  {
    return m_bitset.is_set(value);
  }

  std::pair<bool, Set> insert(const Value& value) const
  {
    FlexibleBitset copy_bitset{ m_bitset };
    bool ok = copy_bitset.set(value);
    return{ ok, Set(std::move(copy_bitset)) };
  }

  std::pair<bool, Set> erase(const Value& value) const
  {
    FlexibleBitset copy_bitset{ m_bitset };
    bool ok = copy_bitset.reset(value);
    return{ ok, Set(std::move(copy_bitset)) };
  }

  bool operator==(const Set& set) const
  {
    return m_bitset == set.m_bitset;
  }

  bool operator!=(const Set& set) const
  {
    return m_bitset != set.m_bitset;
  }
};

constexpr char Set::s_empty_op_name[];
constexpr char Set::s_contains_op_name[];
constexpr char Set::s_insert_op_name[];
constexpr char Set::s_erase_op_name[];

template<>
struct Hash<Set>
{
  std::size_t operator()(const Set& set) const noexcept
  {
    return set.bitset().hash_code();
  }
};

/// Bounded stack
/// N - stack capacity
template<std::size_t N>
class Stack
{
 public:
  typedef char Value;

 private:
  static constexpr char s_try_push_op_name[9] = "try_push";
  static constexpr char s_try_pop_op_name[8] = "try_pop";

 public:
  struct TryPushCallOp : public internal::ArgOp<Stack<N>, Value, s_try_push_op_name>
  {
    typedef internal::ArgOp<Stack<N>, Value, s_try_push_op_name> Base;
    TryPushCallOp(Value v) : Base(false, v) {}

    std::pair<bool, Stack<N>> internal_apply(const Stack<N>& stack, const Op<Stack<N>>& op) override
    {
      typedef internal::RetOp<Stack<N>, bool> RetOp;

      const RetOp& try_push_ret = dynamic_cast<const RetOp&>(op);
      if (!stack.is_full())
        return{ try_push_ret.ret, stack.push(Base::value) };

      return{ !try_push_ret.ret, stack };
    }
  };

  struct TryPopRetOp : public Op<Stack<N>>
  {
   private:
    const bool m_ok;
    const Value m_value;

   public:
    TryPopRetOp(bool ok, Value value)
     : Op<Stack<N>>(),
     m_ok{ ok },
     m_value{ value } {}

    TryPopRetOp(bool ok, std::size_t height, Value value)
     : Op<Stack<N>>(height),
     m_ok{ ok },
     m_value{ value } {}

    bool ok() const
    {
      return m_ok;
    }

    /// \pre: ok()
    Value value() const
    {
      assert(ok());
      return m_value;
    }

#ifdef _LT_DEBUG_
  std::ostream& print(std::ostream& os) const override
  {
    return os << "ret: [ok: " << ok() << ", value: " << (ok() ? std::to_string(value()) : "undefined") << "]";
  }
#endif
  };

  struct TryPopCallOp : public internal::ZeroArgOp<Stack<N>, s_try_pop_op_name>
  {
    typedef internal::ZeroArgOp<Stack<N>, s_try_pop_op_name> Base;
    TryPopCallOp() : Base() {}

    std::pair<bool, Stack<N>> internal_apply(const Stack<N>& stack, const Op<Stack<N>>& op) override
    {
      const TryPopRetOp& try_pop_ret = dynamic_cast<const TryPopRetOp&>(op);

      if (stack.is_empty())
        return{ !try_pop_ret.ok(), stack };

      Value value{ stack.top() };
      return{ try_pop_ret.ok() && value == try_pop_ret.value(), stack.pop() };
    }
  };

  typedef std::unique_ptr<Op<Stack<N>>> StackOpPtr;

  class Node;
  typedef Node* NodePtr;

  /// Version tree of stacks
  class Node
  {
   private:
    friend class Stack<N>;

    // if m_prev is null, then m_size is zero
    const std::size_t m_size;

    // top of stack is defined if m_prev != nullptr
    const Value m_top;
    const NodePtr m_prev;
    unsigned m_ref_counter;

    std::vector<NodePtr> m_vector;

   public:
    ~Node()
    {
      if (m_prev == nullptr)
        return;

      m_prev->m_vector[m_top] = nullptr;
      if (--m_prev->m_ref_counter == 0)
        delete m_prev;
    }

    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;

    Node()
     : m_size{ 0 },
     m_top{},
     m_prev{ nullptr },
     m_ref_counter{ 0U },
     m_vector{} {}

    /// \pre: prev != nullptr
    Node(std::size_t size, Value value, const NodePtr& prev)
     : m_size{ size },
     m_top{ value },
     m_prev{ prev },
     m_ref_counter{ 0U },
     m_vector{}
    {
      assert(prev != nullptr);
      ++prev->m_ref_counter;
    }

    /// Returns non-null pointer

    /// \pre: prev != nullptr
    NodePtr get_or_update_next(Value value, NodePtr& prev)
    {
      assert(prev != nullptr);

      if (m_vector.size() < value + 1U)
        m_vector.resize(value + 1U);

      assert(value < m_vector.size());

      NodePtr& node_ptr = m_vector[value];
      if (node_ptr == nullptr)
        node_ptr = new Node(prev->size() + 1U, value, prev);

      assert(node_ptr != nullptr);
      assert(node_ptr->m_top == value);
      return node_ptr;
    }

    std::size_t size() const
    {
      assert(m_prev != nullptr || m_size == 0);
      return m_size;
    }

    const NodePtr& prev() const
    {
      return m_prev;
    }

    /// Defined if prev() != nullptr
    Value top() const noexcept
    {
      return m_top;
    }
  };

 public:
  static StackOpPtr make_try_push_call(Value value)
  {
    return make_unique<TryPushCallOp>(value);
  }

  static StackOpPtr make_try_push_ret(bool ok)
  {
    typedef internal::RetOp<Stack<N>, bool> RetOp;
    return make_unique<RetOp>(ok);
  }

  static StackOpPtr make_try_push_ret(bool ok, std::size_t height)
  {
    typedef internal::RetOp<Stack<N>, bool> RetOp;
    return make_unique<RetOp>(ok, height);
  }

  static StackOpPtr make_try_pop_call()
  {
    return make_unique<TryPopCallOp>();
  }

  static StackOpPtr make_try_pop_ret(bool ok, Value v)
  {
    return make_unique<TryPopRetOp>(ok, v);
  }

  static StackOpPtr make_try_pop_ret(bool ok, std::size_t height, Value v)
  {
    return make_unique<TryPopRetOp>(ok, height, v);
  }

 private:
  // never null, m_curr->size() <= N
  mutable NodePtr m_curr;

  void inc_ref_counter() const noexcept
  {
    if (m_curr != nullptr)
      ++m_curr->m_ref_counter;
  }

  void dec_ref_counter() const
  {
    assert(m_curr == nullptr || 0 < m_curr->m_ref_counter);

    if (m_curr != nullptr && --m_curr->m_ref_counter == 0)
      delete m_curr;
  }

  Stack(NodePtr curr)
   : m_curr{ curr }
  {
    inc_ref_counter();
  }

 public:
  ~Stack()
  {
    dec_ref_counter();
  }

  Stack(const Stack& other)
   : m_curr{ other.m_curr }
  {
    inc_ref_counter();
  }

  Stack(Stack&& other)
   : m_curr{ other.m_curr }
  {
    other.m_curr = nullptr;
  }

  Stack& operator=(const Stack& other)
  {
    dec_ref_counter();
    m_curr = other.m_curr;
    inc_ref_counter();
    return *this;
  }

  Stack& operator=(Stack&& other)
  {
    dec_ref_counter();
    m_curr = other.m_curr;
    other.m_curr = nullptr;
    return *this;
  }

  Stack()
   : m_curr{ new Node() }
  {
    inc_ref_counter();
  }

  bool is_empty() const
  {
    return m_curr->prev() == nullptr;
  }

  bool is_full() const
  {
    return m_curr->size() == N;
  }

  Stack<N> push(const Value& value) const
  {
    assert(!is_full());
    return{ m_curr->get_or_update_next(value, m_curr) };
  }

  /// \pre: !is_empty()
  Stack<N> pop() const
  {
    assert(!is_empty());
    return{ m_curr->prev() };
  }

  Value top() const
  {
    assert(!is_empty());
    return m_curr->top();
  }

  bool operator==(const Stack<N>& stack) const
  {
    return stack.m_curr == m_curr;
  }

  bool operator!=(const Stack<N>& stack) const
  {
    return stack.m_curr != m_curr;
  }

  std::size_t hash_code() const
  {
  // On many platforms (except systems with segmented addressing)
  // std::size_t is synonymous with std::uintptr_t.
  //
  // \see_also http://en.cppreference.com/w/cpp/types/size_t
    return reinterpret_cast<uintptr_t>(m_curr);
  }
};

template<std::size_t N>
constexpr char Stack<N>::s_try_push_op_name[];

template<std::size_t N>
constexpr char Stack<N>::s_try_pop_op_name[];

template<std::size_t N>
struct Hash<Stack<N>>
{
std::size_t operator()(const Stack<N>& stack) const noexcept
{
  return stack.hash_code();
}
};

/// Bounded queue

/// N - queue capacity
template<std::size_t N>
class Queue
{
 public:
  typedef char Value;

 private:
  static constexpr char s_try_enqueue_op_name[12] = "try_enqueue";
  static constexpr char s_try_dequeue_op_name[12] = "try_dequeue";

 public:
  struct TryEnqueueCallOp : public internal::ArgOp<Queue<N>, Value, s_try_enqueue_op_name>
  {
    typedef internal::ArgOp<Queue<N>, Value, s_try_enqueue_op_name> Base;
    TryEnqueueCallOp(Value v) : Base(false, v) {}

    std::pair<bool, Queue<N>> internal_apply(const Queue<N>& queue, const Op<Queue<N>>& op) override
    {
      typedef internal::RetOp<Queue<N>, bool> RetOp;

      const RetOp& try_enqueue_ret = dynamic_cast<const RetOp&>(op);
      if (!queue.is_full())
        return{ try_enqueue_ret.ret, queue.enqueue(Base::value) };

      return{ !try_enqueue_ret.ret, queue };
    }
  };

  struct TryDequeueRetOp : public Op<Queue<N>>
  {
   private:
    const bool m_ok;
    const Value m_value;

   public:
    TryDequeueRetOp(bool ok, Value value)
     : Op<Queue<N>>(),
     m_ok{ ok },
     m_value{ value } {}

    TryDequeueRetOp(bool ok, std::size_t height, Value value)
     : Op<Queue<N>>(height),
     m_ok{ ok },
     m_value{ value } {}

    bool ok() const
    {
      return m_ok;
    }

    /// \pre: ok()
    Value value() const
    {
      assert(ok());
      return m_value;
    }

    #ifdef _LT_DEBUG_
    std::ostream& print(std::ostream& os) const override
    {
      return os << "ret: [ok: " << ok() << ", value: " << (ok() ? std::to_string(value()) : "undefined") << "]";
    }
    #endif
  };

  struct TryDequeueCallOp : public internal::ZeroArgOp<Queue<N>, s_try_dequeue_op_name>
  {
    typedef internal::ZeroArgOp<Queue<N>, s_try_dequeue_op_name> Base;
    TryDequeueCallOp() : Base() {}

    std::pair<bool, Queue<N>> internal_apply(const Queue<N>& queue, const Op<Queue<N>>& op) override
    {
      const TryDequeueRetOp& try_dequeue_ret = dynamic_cast<const TryDequeueRetOp&>(op);

      if (queue.is_empty())
        return{ !try_dequeue_ret.ok(), queue };

      Value value{ queue.get_value() };
      return{ try_dequeue_ret.ok() && value == try_dequeue_ret.value(), queue.dequeue() };
    }
  };


  typedef std::unique_ptr<Op<Queue<N>>> QueueOpPtr;
  class Node;
  typedef Node* NodePtr;

  class Node
  {
   private:
    friend class Queue<N>;


    // value of current node
    const Value m_value;

    // pointers to next and prev objects in the queue, possibly null
    NodePtr m_next;
    NodePtr m_prev;

    // number of next elements from the current node. The pointer m_next
    // points only at the last successor added. When m_nexts_num != 0, it
    // is necessary to traverse the queue starting from the tail to find the
    // corresponding successor at the given node. 
    // Example:
    // a <--> b <--> c <--  d <--> e
    //                 <--> f <--> g
    // In the example c.m_nexts points to f. In order to find the successor of 
    // c for the queue with head = a and tail = e the queue must be traversed 
    // from e until the point with multiple successors.
    unsigned m_nexts_num;

    // number of references to the current node (necessary for garbage collection)
    unsigned m_ref_counter;


   public:
    ~Node()
    {
      if (m_prev != nullptr) {
        if (m_prev->m_next == this)
          m_prev->m_next = nullptr;
      }

      if (m_next != nullptr) {
        m_next->m_prev = nullptr;
      } 
    }

    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;

    Node()
     : m_value{},
     m_next{ nullptr },
     m_prev{ nullptr },
     m_ref_counter{ 0U },
     m_nexts_num{ 0U } {}

    /// \pre: prev != nullptr
    Node(Value value, const NodePtr& prev, const NodePtr& next)
     : m_value{ value },
     m_prev{ prev },
     m_next{ next },
     m_ref_counter{ 0U },
     m_nexts_num{ 0U } {

      if (prev != nullptr) {
        prev->m_ref_counter++;
      }
      if (next != nullptr) {
        next->m_ref_counter++;
      }
    }

    NodePtr update_next(Value value)
    {
      NodePtr node_ptr = new Node(value, this, nullptr);
      m_next = node_ptr;
      node_ptr->m_ref_counter++;
      m_nexts_num++;
      return node_ptr;
    }


    const NodePtr& next() const
    {
      return m_next;
    }

    const NodePtr& prev() const
    {
      return m_prev;
    }

    Value value() const noexcept
    {
      return m_value;
    }
  };

 public:
  static QueueOpPtr make_try_enqueue_call(Value value)
  {
    return make_unique<TryEnqueueCallOp>(value);
  }

  static QueueOpPtr make_try_enqueue_ret(bool ok)
  {
    typedef internal::RetOp<Queue<N>, bool> RetOp;
    return make_unique<RetOp>(ok);
  }

  static QueueOpPtr make_try_enqueue_ret(bool ok, std::size_t height)
  {
    typedef internal::RetOp<Queue<N>, bool> RetOp;
    return make_unique<RetOp>(ok, height);
  }

  static QueueOpPtr make_try_dequeue_call()
  {
    return make_unique<TryDequeueCallOp>();
  }

  static QueueOpPtr make_try_dequeue_ret(bool ok, Value v)
  {
    return make_unique<TryDequeueRetOp>(ok, v);
  }

  static QueueOpPtr make_try_dequeue_ret(bool ok, std::size_t height, Value v)
  {
    return make_unique<TryDequeueRetOp>(ok, height, v);
  }

 private:

  // m_head points to the element BEFORE the actual head
  NodePtr m_head;
  NodePtr m_tail;
  size_t  m_size;


  Queue(NodePtr head, NodePtr tail, size_t size)
   : m_head{ head },
   m_tail{ tail },
   m_size{ size }
  {
    inc_ref_counter();
  }


  // Decrement reference counter of  head and tail, delete them if necessary
  void dec_ref_counter() const
  {   
    NodePtr curNode = m_head;
    NodePtr aux = nullptr;

    if (m_head != nullptr && m_tail != nullptr) {
      m_head->m_ref_counter--;
      m_tail->m_ref_counter--;
    }

    while (curNode != nullptr && curNode->prev() != m_tail && curNode->m_ref_counter <= 1){
      aux = curNode;
      if (curNode->m_nexts_num <= 1)
        curNode = curNode->next();
      else
        curNode = find_next(curNode);
      if (curNode != nullptr && curNode == aux->next())
        curNode->m_ref_counter--;
      delete aux;
    }

    if (aux == m_tail) {
      return;
    }
  
    curNode = m_tail;
    while (curNode != nullptr && curNode->next() != m_head && curNode->m_ref_counter <= 1){
      aux = curNode;
      curNode = curNode->prev();

      if (curNode != nullptr) {
        --curNode->m_ref_counter;
      }
      delete aux;
    }      
  }

  // Find the successor of a specific node
  NodePtr find_next(NodePtr node) const {
    if (node == m_tail) {
      return nullptr;
    }
    NodePtr curr = m_tail;

    while (curr != nullptr && curr->prev() != node) {
      curr = curr->prev();
    }

    return curr;
  }


  // Increment reference counter of head and tail
  void inc_ref_counter() const
  {
    if (m_head != nullptr)
      m_head->m_ref_counter++;

    if (m_tail != nullptr)
      m_tail->m_ref_counter++;
  }

  // Used when m_head has more than one successor to recover the direction in which to
  // dequeue
  NodePtr find_next() const {
    return find_next(m_head);
  }

 public:

  ~Queue()
  {   
    dec_ref_counter();
  }

  Queue(const Queue& other)
   : m_head{ other.m_head },
     m_tail{ other.m_tail },
     m_size{ other.m_size }
  {
    inc_ref_counter();
  }

  Queue(Queue&& other)
   : m_head{ other.m_head },
     m_tail{ other.m_tail },
     m_size{ other.m_size }
  {
    other.m_head = nullptr;
    other.m_tail = nullptr;
    other.m_size = 0U;
  }

  Queue& operator=(const Queue& other)
  {
    dec_ref_counter();
    m_head = other.m_head;
    m_tail = other.m_tail;
    m_size = other.m_size;
    inc_ref_counter();
    return *this;
  }

  Queue& operator=(Queue&& other)
  {
    dec_ref_counter();
    m_size = other.m_size;
    m_head = other.m_head;
    m_tail = other.m_tail;
    other.m_head = nullptr;
    other.m_tail = nullptr;
    other.m_size = 0U;
    return *this;
  }

  Queue()
  {
    NodePtr newNode = new Node();
    m_head = newNode;
    m_tail = newNode;
    m_size = 0U;
    inc_ref_counter();
  }

  bool is_empty() const
  {
    return m_size == 0U;
  }

  bool is_full() const
  {
    return m_size == N;
  }

  // If enqueing is possible returns the updated stack, otherwise returns current stack.
  Queue<N> enqueue(const Value& value) const
  {
    if (!is_full()) {
      return{ m_head, m_tail->update_next(value), m_size + 1 };
    }
    return *this;
  }

  // If dequeing is possible returns the updated stack, otherwise returns current stack.
  // In case of multiple successors we have to check which is the right one starting
  // from the tail.
  Queue<N> dequeue() const
  {
    if (!is_empty()) {
      if (m_head->m_nexts_num <= 1) {
        return{ m_head->next(), m_tail, m_size - 1 };
      }
      else {
        return{ find_next(), m_tail, m_size - 1 };
      }
    }
    return *this;
  }

  // Returns the first value of the queue i.e. the value stored in the successor of m_head.
  // If head has more than one successor the correct one must be retrieved
  Value get_value() const
  {
    assert(!is_empty());
    assert(m_head != nullptr);

    if (m_head->m_nexts_num != 1)
      return find_next()->value();

    return m_head->m_next->value();
  }


  bool operator==(const Queue<N>& queue) const
  {
    return (queue.m_head == m_head && queue.m_tail == m_tail && queue.m_size == m_size);
  }

  bool operator!=(const Queue<N>& queue) const
  {
    return !(*this == queue);
  }

  std::size_t hash_code() const
  {
    // On many platforms (except systems with segmented addressing)
    // std::size_t is synonymous with std::uintptr_t.
    //
    // \see_also http://en.cppreference.com/w/cpp/types/size_t
    return reinterpret_cast<uintptr_t>(m_head);
  }
};

template<std::size_t N>
constexpr char Queue<N>::s_try_enqueue_op_name[];

template<std::size_t N>
constexpr char Queue<N>::s_try_dequeue_op_name[];

template<std::size_t N>
struct Hash<Queue<N>>
{
std::size_t operator()(const Queue<N>& queue) const noexcept
{
  return queue.hash_code();
}
};

} // state
} // lt 
#endif