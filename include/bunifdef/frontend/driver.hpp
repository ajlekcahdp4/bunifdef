#pragma once

#include "bunifdef_parser.hpp"

#include "bunifdef/frontend/ast/ast_container.hpp"
#include "bunifdef/frontend/error.hpp"
#include "bunifdef/frontend/scanner.hpp"
#include "bunifdef/frontend/source.hpp"

#include <cassert>
#include <filesystem>
#include <iostream>
#include <memory>
#include <sstream>
#include <string_view>
#include <utility>

namespace bunifdef::frontend {
class parser_driver {
private:
  scanner m_scanner;
  parser m_parser;

  ast::directive *m_parent = nullptr;
  std::optional<error_kind> m_current_error;
  ast::ast_container &m_ast;
  std::vector<error_report> &m_error_queue;

  friend class parser;
  friend class scanner;

private:
  void report_error(std::string message, location l) { m_current_error = {std::move(message), l}; }

  void queue_error(std::string message, location l) {
    m_error_queue.push_back({
        error_kind{message, l}
    });
  }

  error_kind take_error() {
    auto error = m_current_error.value();
    m_current_error = std::nullopt;
    return error;
  }

public:
  parser_driver(
      std::string *filename, ast::ast_container &ast, std::vector<error_report> &queue,
      ast::directive *parent = nullptr
  )
      : m_scanner{*this, filename}, m_parser{m_scanner, *this}, m_parent(parent), m_ast(ast),
        m_error_queue(queue) {}

  bool parse() { return m_parser.parse(); }
  void switch_input_stream(std::istream *is) { m_scanner.switch_streams(is, nullptr); }

  template <typename t_node_type, typename... t_args> auto *make_ast_node(t_args &&...args) {
    return &m_ast.make_node<t_node_type>(std::forward<t_args>(args)...);
  }

  void set_ast_root_ptr(ast::i_ast_node *ptr) { // nullptr is possible
    m_ast.set_root_ptr(ptr);
  }

  ast::directive *parent() { return m_parent; }
  ast::ast_container &ast() & { return m_ast; }
  ast::i_ast_node *get_ast_root_ptr() & { return m_ast.get_root_ptr(); }
};

class frontend_driver {
private:
  source_input &m_source;
  error_reporter m_reporter;

  std::unique_ptr<std::istringstream> m_iss;
  std::unique_ptr<parser_driver> m_parsing_driver;

public:
  frontend_driver(
      source_input &src, ast::ast_container &ast, std::vector<error_report> &queue,
      ast::directive *parent = nullptr
  )
      : m_source{src}, m_reporter{m_source},
        m_iss{std::make_unique<std::istringstream>(m_source.iss())},
        m_parsing_driver{std::make_unique<parser_driver>(m_source.filename(), ast, queue, parent)} {
    m_parsing_driver->switch_input_stream(m_iss.get());
  }
  std::string_view get_filename() const { return m_source.get_filename(); }

  ast::ast_container &ast() const & { return m_parsing_driver->ast(); }

  void parse() { m_parsing_driver->parse(); }

  bool analyze() {
    auto &ast = m_parsing_driver->ast();
    if (!ast.get_root_ptr()) return true;
    return false;
  }
};

} // namespace bunifdef::frontend
