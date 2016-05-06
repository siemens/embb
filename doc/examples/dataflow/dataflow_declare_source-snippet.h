  Network::Source<std::string> read(
    network, embb::base::MakeFunction(SourceFunction)
  );
