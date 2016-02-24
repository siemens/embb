  Network::Sink<std::string> write(
    embb::base::MakeFunction(SinkFunction)
  );
