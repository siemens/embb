using embb::algorithms::Reduce;
sum = Reduce(range.begin(), range.end(), 0, std::plus<int>());
