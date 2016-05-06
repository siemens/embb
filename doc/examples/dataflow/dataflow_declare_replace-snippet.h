  Network::ParallelProcess<
    Network::Inputs<std::string>,
    Network::Outputs<std::string> > replace(
      network, embb::base::MakeFunction(ReplaceFunction)
    );
