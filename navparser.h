#pragma once

/** XPNavAidParser (NavParser)
 *
 * (c) Jack Deeth 2016
 *
 * Released under Creative Commons Attribution (CC-BY)
 *
 * This will be a library to parse the earth_nav.dat file used by X-Plane 8/9/10
 * to store navaid info. Intended for use by external applications, rather than
 * by plugins.
 */

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <sstream>
#include <vector>

class NavAid {
public:
  enum class Type {
    ndb,
    vor,
    localizer,
    glideslope,
    outer_marker,
    middle_marker,
    inner_marker,
    dme,
    decca_master,
    decca_red,
    decca_green,
    decca_purple,
    other
  };

  NavAid(std::string dat_row)
      : m_freq_khz{0}, m_service_range_nm{0}, m_bearing_true_deg{0},
        m_standalone{true} {
    std::stringstream sstr(dat_row);
    std::istream_iterator<std::string> it(sstr);
    std::istream_iterator<std::string> end;
    std::vector<std::string> tokens(it, end);

    if (tokens.size() < 9) {
      throw(std::invalid_argument("Not enough fields"));
    }

    switch (stoi(tokens[0])) {
    case 2:
      m_type = Type::ndb;
      break;
    case 3:
      m_type = Type::vor;
      break;
    case 5:
      m_standalone = false;
      [[clang::fallthrough]];
    case 4:
      m_type = Type::localizer;
      break;
    case 6:
      m_type = Type::glideslope;
      m_standalone = false;
      break;
    case 7:
      m_type = Type::outer_marker;
      break;
    case 8:
      m_type = Type::middle_marker;
      break;
    case 9:
      m_type = Type::inner_marker;
      break;
    case 12:
      m_standalone = false;
      [[clang::fallthrough]];
    case 13:
      m_type = Type::dme;
      break;
    default:
      m_type = Type::other;
    }

    m_lat = stof(tokens[1]);
    m_lon = stof(tokens[2]);
    m_elev_ft = stoi(tokens[3]);

    if (m_type == Type::outer_marker || m_type == Type::middle_marker ||
        m_type == Type::inner_marker) {
      // do nothing
    } else if (m_type == Type::ndb) {
      m_freq_khz = stoi(tokens[4]);
    } else {
      m_freq_khz = stoi(tokens[4]) * 10;
    }

    // todo: parse range and bearing

    m_ident = tokens[7];

    m_name = tokens[8];

    if (tokens.size() > 9) {
      for (auto i = std::begin(tokens) + 9, e = std::end(tokens); i != e; ++i) {
        m_name += " ";
        m_name += *i;
      }
    }

    auto tok8 = tokens[8];
    std::transform(tok8.begin(), tok8.end(), tok8.begin(), ::toupper);
    if (tok8 == "DECCA") {
      try {
        tokens[9];
      } catch (std::invalid_argument e) {
        std::cerr << dat_row << std::endl
                  << "Needs Master|Red|Green|Purple after 'Decca'" << std::endl;
        throw e;
      }
      auto decca_type = tokens[9];
      std::transform(decca_type.begin(), decca_type.end(), decca_type.begin(),
                     ::toupper);
      if (decca_type == "MASTER")
        m_type = Type::decca_master;
      else if (decca_type == "RED")
        m_type = Type::decca_red;
      else if (decca_type == "GREEN")
        m_type = Type::decca_green;
      else if (decca_type == "PURPLE")
        m_type = Type::decca_purple;
    }
  }

  Type type() const { return m_type; }
  double lat_deg() const { return m_lat; }
  double lon_deg() const { return m_lon; }
  int elev_ft() const { return m_elev_ft; }
  float freq_khz() const { return m_freq_khz; }
  int service_range_nm() const { return m_service_range_nm; }
  float bearing_true_deg() const { return m_bearing_true_deg; }
  std::string ident() const { return m_ident; }
  std::string name() const { return m_name; }
  bool standalone() const { return m_standalone; }

private:
  Type m_type;
  double m_lat, m_lon; // decimal degrees
  int m_elev_ft;
  float m_freq_khz;
  int m_service_range_nm;
  float m_bearing_true_deg;
  std::string m_ident;
  std::string m_name;
  bool m_standalone;
};

std::vector<NavAid> ParseEarthNavDat(std::string filepath = "earth_nav.dat") {
  std::vector<NavAid> navaids;
  std::ifstream navdat(filepath);
  std::string line;
  if (navdat.is_open()) {
    // ignore status and notice lines at top of file
    getline(navdat, line);
    getline(navdat, line);
    while (getline(navdat, line) && line != "99") {
      try {
        if (line.size() > 0)
          navaids.push_back(line);
      } catch (std::exception e) {
        std::cerr << e.what() << std::endl;
      }
    }
  }
  navdat.close();
  return navaids;
}
