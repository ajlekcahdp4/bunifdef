#pragma once

#include "i_ast_node.hpp"

#include <string>
#include <string_view>

namespace bunifdef::frontend::ast {

class variable_expression : public i_expression {
  std::string m_name;

public:
  variable_expression(std::string p_name, location l) : i_expression{l}, m_name{p_name} {}

  void accept(i_visitor &v) override { v.apply(*this); }
  std::string_view name() const & { return m_name; }
};

} // namespace bunifdef::frontend::ast
