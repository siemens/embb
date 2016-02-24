template <typename T>
class Consumer {
public:
  void Run(const T& x1, const T& x2, const T& x3, const T& x4) {
    if (x1 <= x2 && x2 <= x3 && x3 <= x4) {
      // consume values
    }
  }
};
