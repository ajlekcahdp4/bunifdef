#pragma once

#include "i_ast_node.hpp"

#include <memory>
#include <string>
#include <string_view>

namespace bunifdef::frontend::ast {

class line : public i_ast_node {
  std::string m_content;

public:
  line(std::string p_content, location l) : i_ast_node{l}, m_content{p_content} {}
  void accept(i_visitor &v) override { v.apply(*this); }
  std::string_view content() const & { return m_content; }
};

} // namespace bunifdef::frontend::ast
