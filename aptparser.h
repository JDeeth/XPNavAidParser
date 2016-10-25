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

#include <fstream>
#include <string>
#include <utility>
#include <vector>

#include "parseutil.h"

struct Runway {
  vec2 start;
  vec2 end;
};

struct Airport {
  std::string icao;
  std::string name;
  std::vector<Runway> runways;
  Airport(std::string icao, std::string name) : icao(icao), name(name) {}
};

std::vector<Airport> ParseAptDat(std::string filepath = "apt.dat") {
  std::vector<Airport> airports;

  std::ifstream aptdat(filepath);
  std::string line;
  if (aptdat.is_open()) {
    while (getline(aptdat, line) && line != "99") {
      if (line.size() > 0) {
        tokens tkn = tokenise(line);

        // trusting apt.dat to have data in the right sequence - which it is
        // required to be by the standard
        // lines beginning with 1 denote a new land airport.
        // lines beginning with 100 denote a runway.
        if (tkn[0] == "1") {
          // todo: handle airports with spaces in the names
          airports.push_back({tkn[4], tkn[5]});
        } else if (tkn[0] == "100") {
          vec2 start{stof(tkn[9]), stof(tkn[10])};
          vec2 end{stof(tkn[18]), stof(tkn[19])};
          airports.back().runways.push_back({start, end});
        }
      }
    }
  }

  return airports;
}
