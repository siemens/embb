  Network::ParallelProcess<
    Network::Inputs<std::string>::Type,
    Network::Outputs<std::string>::Type> replace(
      embb::base::MakeFunction(ReplaceFunction)
    );
