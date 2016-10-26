#pragma once

#include <algorithm>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

using vec2 = std::pair<double, double>;

std::ostream &operator<<(std::ostream &out, const vec2 &v) {
  return out << v.first << "," << v.second;
}

using tokens = std::vector<std::string>;

tokens tokenise(std::string line) {
  std::stringstream sstr(line);
  std::istream_iterator<std::string> it(sstr);
  std::istream_iterator<std::string> end;
  return {it, end};
}

std::string detokenise(tokens::iterator start, tokens::iterator end) {
  std::string dtkn = *start;
  if (start != end)
    start++;
  for (auto i = start, e = end; i != e; ++i) {
    dtkn += " ";
    dtkn += *i;
  }
  return dtkn;
}

std::string to_upper(std::string str) {
  std::transform(str.begin(), str.end(), str.begin(), ::toupper);
  return str;
}
