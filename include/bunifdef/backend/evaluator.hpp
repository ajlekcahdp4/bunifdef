#pragma once

#include "bunifdef/frontend/ast/ast_container.hpp"
#include "bunifdef/frontend/defs.hpp"

#include <ostream>
#include <string>
#include <unordered_map>

namespace bunifdef::backend {
void process_text(
    bunifdef::frontend::ast::ast_container &ast, std::ostream &os,
    const std::unordered_map<std::string, frontend::def_info> &defs, bool selective
);
}
