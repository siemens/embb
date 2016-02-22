bool SourceFunction(std::string & str) {
  if (!file.eof()) {
    std::getline(file, str);
    return true;
  } else {
    return false;
  }
}
