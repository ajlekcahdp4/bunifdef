#pragma once

#include "i_ast_node.hpp"

namespace bunifdef::frontend::ast {

class print_statement : public i_statement {
  i_expression *m_expr;

  EZVIS_VISITABLE();

public:
  print_statement(i_expression &p_expr, location l) : i_statement{l}, m_expr{&p_expr} {}
  i_expression &expr() const { return *m_expr; }
};

} // namespace bunifdef::frontend::ast
