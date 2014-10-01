struct DoubleFunctor {
  void operator()(int& to_double) {
    to_double *= 2;
  }
};
