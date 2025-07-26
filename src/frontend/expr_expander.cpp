#include "bunifdef/frontend/expr_expander.hpp"
#include "bunifdef/frontend/ast/ast_nodes.hpp"
#include "bunifdef/frontend/ast/ast_nodes/directive.hpp"
#include "bunifdef/frontend/driver.hpp"
#include "bunifdef/frontend/error.hpp"
#include "bunifdef/frontend/source.hpp"

#include <boost/filesystem.hpp>

#include <fstream>
#include <ranges>
#include <utility>

namespace ranges = std::ranges;
namespace bunifdef::frontend {

class expander : public ast::i_visitor {
  ast::ast_container &m_ast;
  std::vector<error_report> &m_error_queue;

public:
  expander(ast::ast_container &ast, std::vector<error_report> &queue)
      : m_ast(ast), m_error_queue(queue) {}
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
  source_input source(path_str, /* append_newline */ false);
  auto fix_location = [&ref](auto loc) {
    auto orig = ref.loc();
    // +1 for '#' character and +1 for tracking space
    auto directive_len = ast::if_kind_to_string(ref.kind()).size() + 2;
    auto beg = position(orig.begin.filename, orig.begin.line, orig.begin.column + directive_len);
    auto end = position(
        orig.begin.filename, orig.begin.line, orig.begin.column + directive_len + loc.end.column
    );
    location res;
    res.begin = beg;
    res.end = end;
    return res;
  };
  std::vector<error_report> local_queue;
  frontend_driver drv{source, m_ast, local_queue, &ref};
  drv.parse();
  ranges::transform(local_queue, std::back_inserter(m_error_queue), [&](auto &report) {
    auto kerr = error_kind{
        report.m_primary_error.m_error_message, fix_location(report.m_primary_error.m_loc)
    };
    error_report res{kerr};
    ranges::transform(report.m_attachments, std::back_inserter(res.m_attachments), [&](auto &att) {
      return error_attachment{att.m_info_message, fix_location(att.m_loc)};
    });
    return res;
  });
  ref.true_block()->accept(*this);
  if (ref.else_block()) ref.else_block()->accept(*this);
  fs::remove_all(path_str.c_str());
}

void expand_directive_expressions(ast::ast_container &ast, std::vector<error_report> &queue) {
  expander exp(ast, queue);
  ast.get_root_ptr()->accept(exp);
}
} // namespace bunifdef::frontend
