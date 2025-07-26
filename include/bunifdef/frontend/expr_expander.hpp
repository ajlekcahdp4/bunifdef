#pragma once

#include <bunifdef/frontend/ast/ast_container.hpp>
#include <bunifdef/frontend/error.hpp>

namespace bunifdef::frontend {

void expand_directive_expressions(ast::ast_container &ast, std::vector<error_report> &queue);
}
