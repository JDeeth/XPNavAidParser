#pragma once

#include <algorithm>
#include <cmath>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

class angle {
protected:
  double m_deg;

public:
  double deg() const { return m_deg; }
  double rad() const { return m_deg * M_PI / 180.; }
  void set_deg(double degrees) { m_deg = degrees; }
  void set_rad(double rad) { m_deg = rad * 180 / M_PI; }
  angle() = default;
  angle(double degrees) : m_deg{degrees} {}
};

struct coord {
  angle lat;
  angle lon;
  angle colat() { return angle(90. - lat.deg()); }
  coord() = default;
  coord(double lat_deg, double lon_deg) : lat{lat_deg}, lon{lon_deg} {}
};

std::ostream &operator<<(std::ostream &out, const coord &c) {
  return out << c.lat.deg() << "," << c.lon.deg();
}

//

using vec2 = std::pair<double, double>;

vec2 radFromDeg(vec2 degrees) {
  degrees.first *= M_PI / 180.;
  degrees.second *= M_PI / 180.;
  return degrees;
}

vec2 degFromRad(vec2 radians) {
  radians.first *= 180. / M_PI;
  radians.second *= 180. / M_PI;
  return radians;
}

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
