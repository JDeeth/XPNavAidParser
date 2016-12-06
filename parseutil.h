#pragma once

#include <algorithm>
#include <cmath>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

class angle {
private:
  double m_rad;

public:
  angle() = default;
  explicit constexpr angle(long double radians)
      : m_rad{static_cast<float>(radians)} {}

  angle &operator+=(const angle &rhs) {
    m_rad += rhs.m_rad;
    return *this;
  }
  angle &operator-=(const angle &rhs) {
    m_rad -= rhs.m_rad;
    return *this;
  }

  operator double() const { return m_rad; }

  void set_rad(double radians) { m_rad = radians; }
  void set_deg(double degrees) { m_rad = degrees * M_PI / 180.; }

  double deg() const { return m_rad * 180 / M_PI; }
};

inline angle operator+(angle lhs, const angle &rhs) {
  lhs += rhs;
  return lhs;
}
inline angle operator-(angle lhs, const angle &rhs) {
  lhs -= rhs;
  return lhs;
}

constexpr angle operator"" _deg(long double degrees) {
  return angle(degrees * M_PI / 180.);
}

constexpr angle operator"" _rad(long double radians) { return angle(radians); }

struct coord {
  angle lat;
  angle lon;
  angle colat() { return angle(90._deg - lat); }
  coord() = default;
  coord(angle lat, angle lon) : lat(lat), lon(lon) {}
  coord(double lat_deg, double lon_deg) {
    lat.set_deg(lat_deg);
    lon.set_deg(lon_deg);
  }
};

std::ostream &operator<<(std::ostream &out, const coord &c) {
  return out << c.lat.deg() << "," << c.lon.deg();
}

using vec2[[deprecated]] = std::pair<double, double>;

[[deprecated]] vec2 radFromDeg(vec2 degrees) {
  degrees.first *= M_PI / 180.;
  degrees.second *= M_PI / 180.;
  return degrees;
}

[[deprecated]] vec2 degFromRad(vec2 radians) {
  radians.first *= 180. / M_PI;
  radians.second *= 180. / M_PI;
  return radians;
}

[[deprecated]] std::ostream &operator<<(std::ostream &out, const vec2 &v) {
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
