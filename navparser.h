/** XPNavAidParser
 *
 * (c) Jack Deeth 2016
 *
 * Released under Creative Commons Attribution (CC-BY)
 *
 * This is a set of utilities to parse X-Plane's navigational data, for use in
 * creating bespoke charts.
 */

#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <sstream>
#include <vector>

#include "parseutil.h"

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
    other,
    invalid
  };

  NavAid(std::string dat_row) { parse(dat_row); }

  void parse(std::string dat_row) {
    m_type = Type::invalid;
    m_freq_khz = 0;
    m_service_range_nm = 0;
    m_bearing_true_deg = 0;
    m_standalone = true;

    tokens tkn = tokenise(dat_row);

    if (tkn.size() < 9) {
      throw(std::invalid_argument("Not enough fields"));
    }

    switch (stoi(tkn[0])) {
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

    m_latlon = {stof(tkn[1]), stof(tkn[2])};
    m_elev_ft = stoi(tkn[3]);

    if (m_type == Type::outer_marker || m_type == Type::middle_marker ||
        m_type == Type::inner_marker) {
      // do nothing
    } else if (m_type == Type::ndb) {
      m_freq_khz = stoi(tkn[4]);
    } else {
      m_freq_khz = stoi(tkn[4]) * 10;
    }

    // todo: parse range and bearing

    m_ident = tkn[7];

    m_name = detokenise(std::begin(tkn) + 8, std::end(tkn));

    if (to_upper(tkn[8]) == "DECCA") {
      try {
        tkn[9];
      } catch (std::invalid_argument e) {
        std::cerr << dat_row << std::endl
                  << "Needs Master|Red|Green|Purple after 'Decca'" << std::endl;
        throw e;
      }
      auto decca_type = to_upper(tkn[9]);
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

  NavAid() : m_type{Type::invalid} {}

  Type type() const { return m_type; }
  vec2 latlon() const { return m_latlon; }
  double lat_deg() const { return m_latlon.first; }
  double lon_deg() const { return m_latlon.second; }
  int elev_ft() const { return m_elev_ft; }
  float freq_khz() const { return m_freq_khz; }
  int service_range_nm() const { return m_service_range_nm; }
  float bearing_true_deg() const { return m_bearing_true_deg; }
  std::string ident() const { return m_ident; }
  std::string name() const { return m_name; }
  bool standalone() const { return m_standalone; }

  // I think this is not approved by Scott Meyer's "Effective C++"...
  bool isValid() const { return m_type != Type::invalid; }

private:
  vec2 m_latlon;
  Type m_type;
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

struct DeccaChain {
  std::string name;
  std::string channel;
  NavAid master;
  NavAid red;
  NavAid green;
  NavAid purple;
};

std::vector<DeccaChain> BuildDeccaChains(std::vector<NavAid> navaids) {
  std::vector<DeccaChain> chains;
  for (auto n : navaids) {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
    switch (n.type()) {
    case NavAid::Type::decca_master:
      chains.push_back({});
      chains.back().master = n;
      { // keep tokens out of scope of other cases
        tokens tkn = tokenise(n.name());
        chains.back().channel = tkn[2];
        chains.back().name = detokenise(std::begin(tkn) + 3, std::end(tkn));
      }
      break;
    case NavAid::Type::decca_red:
      chains.back().red = n;
      break;
    case NavAid::Type::decca_green:
      chains.back().green = n;
      break;
    case NavAid::Type::decca_purple:
      chains.back().purple = n;
      break;
    }
#pragma GCC diagnostic pop

  }
  return chains;
}
