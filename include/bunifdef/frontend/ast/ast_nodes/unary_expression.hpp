#pragma once

#include "i_ast_node.hpp"
#include "location.hpp"

#include <cassert>
#include <exception>
#include <utility>

namespace bunifdef::frontend::ast {

enum class unary_operation {
  E_UN_OP_NEG,
  E_UN_OP_POS,
  E_UN_OP_NOT,
};

constexpr std::string_view unary_operation_to_string(unary_operation op) {
  using unary_op = unary_operation;

  switch (op) {
  case unary_op::E_UN_OP_NEG: return "-";
  case unary_op::E_UN_OP_POS: return "+";
  case unary_op::E_UN_OP_NOT: return "!";
  }

  std::unreachable();
}

class unary_expression : public i_expression {
private:
  unary_operation m_operation_type;
  i_expression *m_expr;

public:
  unary_expression(unary_operation op_type, i_expression &p_expr, location l)
      : i_expression{l}, m_operation_type{op_type}, m_expr{&p_expr} {}

  unary_operation op_type() const { return m_operation_type; }
  i_expression &expr() const { return *m_expr; }
  void accept(i_visitor &v) override { v.apply(*this); }
};

} // namespace bunifdef::frontend::ast
