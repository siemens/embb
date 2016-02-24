using embb::algorithms::Zip;
using embb::algorithms::ZipPair;
ForEach(Zip(input_range.begin(), output_range.begin()),
        Zip(input_range.end(), output_range.end()),
        [] (ZipPair<int&, int&> pair) {
          pair.Second() = pair.First() * 2;
        });
