using embb::algorithms::CountIf;
count = CountIf(range, range + 8,
                [](const int& value) -> bool { return value > 0; });
