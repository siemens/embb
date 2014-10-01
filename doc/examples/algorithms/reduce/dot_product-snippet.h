using embb::algorithms::Zip;
using embb::algorithms::ZipPair;
int dot_product = Reduce(Zip(range.begin(), second_range.begin()),
                         Zip(range.end(),   second_range.end()),
                         0,
                         std::plus<int>(),
                         [](const ZipPair<int&, int&>& pair) {
                           return pair.First() * pair.Second();
                         });
