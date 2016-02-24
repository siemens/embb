using embb::algorithms::Scan;
Scan(input_range.begin(), input_range.end(), output_range.begin(),
     0, std::plus<int>());
