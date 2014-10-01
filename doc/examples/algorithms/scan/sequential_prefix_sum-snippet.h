std::vector<int> output_range(input_range.size());
output_range[0] = input_range[0];
for(size_t i = 1; i < input_range.size(); i++) {
  output_range[i] = output_range[i-1] + input_range[i];
}
