#include "bunifdef/frontend/expr_expander.hpp"
#include "bunifdef/frontend/ast/ast_nodes.hpp"
#include "bunifdef/frontend/driver.hpp"
#include "bunifdef/frontend/error.hpp"

#include <boost/filesystem.hpp>

#include <fstream>
#include <utility>

namespace bunifdef::frontend {

class expander : public ast::i_visitor {
  ast::ast_container &m_ast;

public:
  expander(ast::ast_container &ast) : m_ast(ast) {}
  void apply(ast::directive &ref) override;
  void apply(ast::block &ref) override {
    for (auto *line : ref) {
      line->accept(*this);
    }
  }
  void apply(ast::line &) override {}
  void apply(ast::error_node &) override {}
  void apply(ast::constant_expression &) override {}
  void apply(ast::variable_expression &) override {}
  void apply(ast::unary_expression &) override {}
  void apply(ast::binary_expression &) override { std::unreachable(); }
};

static void write_to_file(const std::string &path, std::string_view content) {
  std::fstream tmp_file(path, std::ostream::out);
  if (!tmp_file.good())
    throw internal_error(fmt::format("Cannot open file \"{}\" for temporary value", path));
  tmp_file << content;
  tmp_file.flush();
}

void expander::apply(ast::directive &ref) {
  namespace fs = boost::filesystem;
  auto cond = ref.cond_str();
  auto temp_path = fs::temp_directory_path() / fs::unique_path("%%%%-%%%%-%%%%-%%%%.txt");
  auto path_str = temp_path.native();
  write_to_file(path_str, cond);
  frontend_driver drv{path_str, m_ast, &ref};
  drv.parse();
  ref.true_block()->accept(*this);
  if (ref.else_block()) ref.else_block()->accept(*this);
  fs::remove_all(path_str.c_str());
}

void expand_directive_expressions(ast::ast_container &ast) {
  expander exp(ast);
  ast.get_root_ptr()->accept(exp);
}
} // namespace bunifdef::frontend
