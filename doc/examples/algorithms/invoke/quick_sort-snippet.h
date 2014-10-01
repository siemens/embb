void QuickSort(int* first, int* last) {
  if (last - first <= 1) return;
  int* mid = Partition(first, last);
  QuickSort(first, mid);
  QuickSort(mid, last);
}
