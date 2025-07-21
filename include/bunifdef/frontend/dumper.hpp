#pragma once

#include "ast/ast_nodes/i_ast_node.hpp"

#include <iostream>
#include <string>

namespace bunifdef::frontend::ast {

std::string ast_dump_str(i_ast_node *node);

inline void ast_dump(i_ast_node *node, std::ostream &os) {
  os << ast_dump_str(node);
}

} // namespace bunifdef::frontend::ast
