void ParallelQuickSort(int* first, int* last) {
  if (last - first <= 1) return;
  int* mid = Partition(first, last);
  using embb::algorithms::Invoke;
  Invoke([=](){ParallelQuickSort(first, mid);},
         [=](){ParallelQuickSort(mid, last);});
}
