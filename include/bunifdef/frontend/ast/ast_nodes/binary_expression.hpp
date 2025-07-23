#pragma once

#include "i_ast_node.hpp"

#include <cassert>
#include <stdexcept>
#include <utility>

namespace bunifdef::frontend::ast {

enum class binary_operation {
  E_BIN_OP_ADD,
  E_BIN_OP_SUB,
  E_BIN_OP_MUL,
  E_BIN_OP_DIV,
  E_BIN_OP_MOD,
  E_BIN_OP_EQ,
  E_BIN_OP_NE,
  E_BIN_OP_GT,
  E_BIN_OP_LS,
  E_BIN_OP_GE,
  E_BIN_OP_LE,
  E_BIN_OP_AND,
  E_BIN_OP_OR,
};

constexpr std::string_view binary_operation_to_string(binary_operation op) {
  using bin_op = binary_operation;
  switch (op) {
  case bin_op::E_BIN_OP_ADD: return "+";
  case bin_op::E_BIN_OP_SUB: return "-";
  case bin_op::E_BIN_OP_MUL: return "*";
  case bin_op::E_BIN_OP_DIV: return "/";
  case bin_op::E_BIN_OP_MOD: return "%";
  case bin_op::E_BIN_OP_EQ: return "==";
  case bin_op::E_BIN_OP_NE: return "!=";
  case bin_op::E_BIN_OP_GT: return ">";
  case bin_op::E_BIN_OP_LS: return "<";
  case bin_op::E_BIN_OP_GE: return ">=";
  case bin_op::E_BIN_OP_LE: return "<=";
  case bin_op::E_BIN_OP_AND: return "&&";
  case bin_op::E_BIN_OP_OR: return "||";
  }
  std::unreachable();
}

class binary_expression : public i_expression {
  binary_operation m_operation_type;
  i_expression *m_left, *m_right;

public:
  binary_expression(binary_operation op_type, i_expression &left, i_expression &right, location l)
      : i_expression{l}, m_operation_type{op_type}, m_left{&left}, m_right{&right} {}

  i_expression &left() const { return *m_left; }
  i_expression &right() const { return *m_right; }

  binary_operation op_type() const { return m_operation_type; }
  void accept(i_visitor &v) override { v.apply(*this); }
};

} // namespace bunifdef::frontend::ast
