bool SourceFunction(std::string & str) {
  std::getline(file, str);
  return !file.eof();
}
