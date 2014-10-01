void ReplaceFunction(std::string const & istr, std::string & ostr) {
  ostr = istr;
  repl(ostr, what, with);
}
