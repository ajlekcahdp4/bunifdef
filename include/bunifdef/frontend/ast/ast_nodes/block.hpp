#pragma once

#include "i_ast_node.hpp"

#include <cassert>
#include <string>
#include <variant>
#include <vector>

namespace bunifdef::frontend::ast {
class directive;

class block : private std::vector<i_ast_node *>, public i_ast_node {
public:
  block() = default;
  block(const std::vector<i_ast_node *> &vec, location l) : vector{vec}, i_ast_node{l} {}

  void append_line(i_ast_node *line) {
    const bool empty = vector::empty();
    vector::push_back(line);
    auto loc = line->loc();
    m_loc = empty ? loc : (m_loc + loc);
  }

  void accept(i_visitor &v) override { v.apply(*this); }

  using vector::back;
  using vector::begin;
  using vector::cbegin;
  using vector::cend;
  using vector::crbegin;
  using vector::crend;
  using vector::end;
  using vector::front;
  using vector::size;
};
} // namespace bunifdef::frontend::ast
