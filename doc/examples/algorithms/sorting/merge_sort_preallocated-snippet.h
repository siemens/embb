using embb::algorithms::MergeSort;
std::vector<int> temporary_range(range.size());
MergeSort(range.begin(), range.end(), temporary_range.begin());
