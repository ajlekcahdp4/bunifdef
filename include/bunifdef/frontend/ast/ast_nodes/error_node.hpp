#pragma once

#include "i_ast_node.hpp"
#include "location.hpp"

#include <string>

namespace bunifdef::frontend::ast {

class error_node : public i_ast_node {
private:
  std::string m_error_message;

public:
  error_node(std::string_view msg, location l) : i_ast_node{l}, m_error_message{msg} {};
  std::string error_msg() const { return m_error_message; }
  void accept(i_visitor &v) override { v.apply(*this); }
};

} // namespace bunifdef::frontend::ast
