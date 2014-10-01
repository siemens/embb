using embb::algorithms::ForEach;
ForEach(range.begin(), range.end(),
        [] (int& to_double) { to_double *= 2; });
