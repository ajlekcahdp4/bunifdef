#pragma once

#include "bunifdef/frontend/ast/ast_nodes/i_ast_node.hpp"

namespace bunifdef::frontend::ast {

class constant_expression : public ast::i_expression {
  int m_val;

public:
  constant_expression(int p_val, location l) : i_expression{l}, m_val{p_val} {}
  int value() const { return m_val; }
  void accept(i_visitor &v) override { v.apply(*this); }
};

} // namespace bunifdef::frontend::ast
