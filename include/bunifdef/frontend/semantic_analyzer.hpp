#pragma once

#include "bunifdef/frontend/ast/ast_container.hpp"
#include "bunifdef/frontend/ast/ast_nodes.hpp"
#include "bunifdef/frontend/ast/ast_nodes/directive.hpp"
#include "bunifdef/frontend/ast/ast_nodes/variable_expression.hpp"
#include "bunifdef/frontend/error.hpp"

namespace bunifdef::frontend {

class semantic_analyzer : public ast::i_visitor {
  std::vector<error_report> &m_error_queue;
  void report_error(std::string msg, location loc) {
    m_error_queue.push_back({
        error_kind{msg, loc}
    });
  }

  void report_error(error_report report) { m_error_queue.push_back(std::move(report)); }

public:
  semantic_analyzer(std::vector<error_report> &error_queue) : m_error_queue(error_queue) {}
  void apply(ast::directive &ref) override {
    if (ref.kind() == ast::if_kind::E_IF) return;
    auto *var = dynamic_cast<ast::variable_expression *>(ref.cond());
    if (!var)
      report_error(
          fmt::format(
              "ifdef and ifndef directives expected only identifiers as condition. Got \"{}\"",
              ref.cond_str()
          ),
          ref.cond()->loc()
      );
  }
  void apply(ast::block &ref) override {
    for (auto *line : ref) {
      line->accept(*this);
    }
  }
  void apply(ast::line &) override {}
  void apply(ast::error_node &ref) override { report_error(ref.error_msg(), ref.loc()); }
  void apply(ast::constant_expression &) override {}
  void apply(ast::variable_expression &) override {}
  void apply(ast::unary_expression &) override {}
  void apply(ast::binary_expression &) override { std::unreachable(); }

  void analyze(ast::ast_container &ast) { ast.get_root_ptr()->accept(*this); }
};

}; // namespace bunifdef::frontend
