template <typename T>
class Producer {
 public:
  explicit Producer(int seed) : seed_(seed) {}
  void Run(T& x) {
    // produce a new value x
    x = SimpleRand(seed_);
  }

 private:
  int seed_;
};
