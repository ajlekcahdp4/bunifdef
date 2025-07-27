#pragma once

#include <fmt/core.h>

#include <boost/regex.hpp>

#include <iostream>
#include <span>
#include <string>
#include <unordered_map>

namespace bunifdef::frontend {
struct def_info final {
  int val;
  bool defined;
};

inline std::unordered_map<std::string, def_info>
parse_defs(std::span<std::string> defs, std::span<std::string> undefs) {
  std::unordered_map<std::string, def_info> map;
  for (const std::string &def : defs) {
    boost::cmatch what;
    auto matched = boost::regex_match(
        def.c_str(), what, boost::regex("([a-zA-Z][a-zA-Z0-9]*)(=[a-zA-Z0-9]+)?")
    );
    if (!matched)
      throw std::runtime_error(fmt::format(
          "Definition should be an identifier optionally followed by =<value>. Got \"{}\"", def
      ));
    auto name = what[1];
    std::string val_str = what[2].matched ? std::string(what[2]).substr(1) : "0";
    int val = 0;
    try {
      std::size_t num = 0;
      val = std::stoi(val_str, &num);
      if (num != val_str.size()) throw std::runtime_error("invalid value");
    } catch (...) {
      throw std::runtime_error(fmt::format(
          "Invalid value for definition \"{}\". Expected integer, got \"{}\"", name.str(), val_str
      ));
    }
    map[name] = {val, true};
  }
  for (const std::string &def : undefs) {
    auto matched = boost::regex_match(def.c_str(), boost::regex("[a-zA-Z][a-zA-Z0-9]*"));
    if (!matched)
      throw std::runtime_error(
          fmt::format("Definition to undefine should consist of only identifier. Got \"{}\"", def)
      );
    map[def] = {0, false};
  }
  return map;
}

} // namespace bunifdef::frontend
