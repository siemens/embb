template <typename T>
class Comparator {
public:
  void Run(const T& a, const T& b, T& x, T& y) {
    x = std::min(a,b);
    y = std::max(a,b);
  }
};
