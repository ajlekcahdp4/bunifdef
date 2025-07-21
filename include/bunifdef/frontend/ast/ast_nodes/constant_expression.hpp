#pragma once

#include "i_ast_node.hpp"

namespace bunifdef::frontend::ast {

class constant_expression
    : public i_expression { // Inherit from i_expression but dont pass trivial builtin type
                            // by shared ptr. Should fix in the future
  int m_val;

  EZVIS_VISITABLE();

public:
  constant_expression(int p_val, location l)
      : i_expression{l, types::type_builtin::type_int}, m_val{p_val} {}
  int value() const { return m_val; }
};

} // namespace bunifdef::frontend::ast
