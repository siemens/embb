  EMBB_TRY {
    int result = fibonacci(6);
    std::cout << "result: " << result << std::endl;
  } EMBB_CATCH(embb::mtapi::StatusException &) {
    std::cout << "MTAPI error occured." << std::endl;
  }
