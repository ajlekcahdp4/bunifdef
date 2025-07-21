#include "bunifdef/frontend/dumper.hpp"
#include "bunifdef/frontend/ast/ast_nodes.hpp"
#include "bunifdef/frontend/ast/ast_nodes/directive.hpp"

#include <fmt/core.h>
#include <fmt/std.h>

#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

namespace bunifdef::frontend::ast {

class ast_dumper : public i_visitor {
public:
  std::vector<i_ast_node *> m_queue;
  std::stringstream m_ss;

private:
  void print_declare_node(const i_ast_node &ref, std::string_view label) {
    m_ss << fmt::format("\tnode_{} [label = \"{}\"];\n", fmt::ptr(&ref), label);
  }

  void
  print_bind_node(const i_ast_node &parent, const i_ast_node &child, std::string_view label = "") {
    m_ss << fmt::format(
        "\tnode_{} -> node_{} [label = \"{}\"]\n", fmt::ptr(&parent), fmt::ptr(&child), label
    );
  }

public:
  ast_dumper() = default;

  void apply(directive &ref);
  void apply(block &ref);
  void apply(line &ref);
  void apply(error_node &ref);

private:
  void add_next(i_ast_node &node) { m_queue.push_back(&node); }

  i_ast_node *take_next() {
    if (m_queue.empty()) {
      return nullptr;
    }

    auto *ptr = m_queue.back();
    m_queue.pop_back();
    return ptr;
  }

public:
  std::string ast_dump(i_ast_node *root) {
    m_ss.clear();
    m_queue.clear();

    if (root) {
      add_next(*root);
    }

    m_ss << fmt::format("digraph abstract_syntax_tree_{} {{\n", fmt::ptr(this));
    while (auto *ptr = take_next()) {
      ptr->accept(*this);
    }
    m_ss << "}\n";

    return m_ss.str();
  }
};

void ast_dumper::apply(error_node &ref) {
  print_declare_node(ref, "<error>");
}

void ast_dumper::apply(directive &ref) {
  print_declare_node(ref, fmt::format("{}: <{}>", if_kind_to_string(ref.kind()), ref.cond_str()));

  print_bind_node(ref, *ref.true_block(), "<then>");

  add_next(*ref.true_block());

  if (ref.else_block()) {
    print_bind_node(ref, *ref.else_block(), "<else>");
    add_next(*ref.else_block());
  }
}

void ast_dumper::apply(block &ref) {
  print_declare_node(ref, "<block>");
  for (const auto &v : ref) {
    print_bind_node(ref, *v);
    add_next(*v);
  }
}

void ast_dumper::apply(line &ref) {
  print_declare_node(ref, fmt::format("<line: {}>", ref.content()));
}

std::string ast_dump_str(i_ast_node *node) {
  ast_dumper dumper;
  return dumper.ast_dump(node);
}

} // namespace bunifdef::frontend::ast
