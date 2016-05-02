  Network::Sink<std::string> write(
    network, embb::base::MakeFunction(SinkFunction)
  );
