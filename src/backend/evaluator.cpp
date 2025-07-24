#include "bunifdef/frontend/ast/ast_nodes.hpp"
#include "bunifdef/frontend/ast/ast_nodes/binary_expression.hpp"
#include "bunifdef/frontend/ast/ast_nodes/directive.hpp"
#include "bunifdef/frontend/ast/ast_nodes/unary_expression.hpp"
#include "bunifdef/frontend/error.hpp"

#include "bunifdef/backend/evaluator.hpp"

#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

namespace bunifdef::backend {
namespace ast = bunifdef::frontend::ast;

int perform_action(ast::binary_operation op, int lhs, int rhs) {
  switch (op) {
  case ast::binary_operation::E_BIN_OP_ADD: return lhs + rhs;
  case ast::binary_operation::E_BIN_OP_SUB: return lhs - rhs;
  case ast::binary_operation::E_BIN_OP_MUL: return lhs * rhs;
  case ast::binary_operation::E_BIN_OP_DIV: return lhs / rhs;
  case ast::binary_operation::E_BIN_OP_MOD: return lhs % rhs;
  case ast::binary_operation::E_BIN_OP_EQ: return lhs == rhs;
  case ast::binary_operation::E_BIN_OP_NE: return lhs != rhs;
  case ast::binary_operation::E_BIN_OP_GT: return lhs > rhs;
  case ast::binary_operation::E_BIN_OP_LS: return lhs < rhs;
  case ast::binary_operation::E_BIN_OP_GE: return lhs >= rhs;
  case ast::binary_operation::E_BIN_OP_LE: return lhs <= rhs;
  case ast::binary_operation::E_BIN_OP_AND: return lhs && rhs;
  case ast::binary_operation::E_BIN_OP_OR: return lhs || rhs;
  }
  std::unreachable();
}

class evaluator : public ast::i_visitor {
  const std::unordered_map<std::string, int> &m_known_defs;
  std::vector<int> m_values;
  ast::if_kind m_kind;

public:
  evaluator(const std::unordered_map<std::string, int> &defs, ast::if_kind kind)
      : m_known_defs(defs), m_kind(kind) {}

  void apply(ast::directive &) override {
    throw bunifdef::frontend::internal_error("evaluator run on non-expression node");
  }
  void apply(ast::block &) override {
    throw bunifdef::frontend::internal_error("evaluator run on non-expression node");
  }
  void apply(ast::line &) override {
    throw bunifdef::frontend::internal_error("evaluator run on non-expression node");
  }
  void apply(ast::error_node &) override {
    throw bunifdef::frontend::internal_error("evaluator run on non-expression node");
  }
  void apply(ast::unary_expression &ref) override {
    if (m_kind != ast::if_kind::E_IF)
      throw std::runtime_error("Unary expresssions are accepted only in #if statements");
    ref.expr().accept(*this);
    switch (ref.op_type()) {
    case ast::unary_operation::E_UN_OP_NEG: {
      auto val = m_values.back();
      m_values.pop_back();
      m_values.push_back(-val);
      break;
    }
    case ast::unary_operation::E_UN_OP_POS: break;
    case ast::unary_operation::E_UN_OP_NOT: {
      auto val = m_values.back();
      m_values.pop_back();
      m_values.push_back(!val);
      break;
    }
    }
  }
  void apply(ast::binary_expression &ref) override {
    if (m_kind != ast::if_kind::E_IF)
      throw std::runtime_error("Binary expresssions are accepted only in #if statements");
    ref.left().accept(*this);
    ref.right().accept(*this);
    auto lhs = m_values.back();
    m_values.pop_back();
    auto rhs = m_values.back();
    m_values.pop_back();
    auto res = perform_action(ref.op_type(), lhs, rhs);
    m_values.push_back(res);
  }
  void apply(ast::variable_expression &ref) override {
    auto found = m_known_defs.find(std::string(ref.name()));
    switch (m_kind) {
    case ast::if_kind::E_IF: {
      if (found == m_known_defs.end())
        throw std::runtime_error(fmt::format("Undefined name: {}", ref.name()));
      m_values.push_back(found->second);
      break;
    }
    case ast::if_kind::E_IFDEF: {
      m_values.push_back(found != m_known_defs.end());
      break;
    }
    case ast::if_kind::E_IFNDEF: {
      m_values.push_back(found == m_known_defs.end());
      break;
    }
    default:
      std::unreachable();
    }
  }
  void apply(ast::constant_expression &ref) override {
    if (m_kind != ast::if_kind::E_IF)
      throw std::runtime_error("Constant expresssions are accepted only in #if statements");
    m_values.push_back(ref.value());
  }

  int value() const {
    assert(m_values.size() == 1);
    return m_values[0];
  }
};

class text_processor : public ast::i_visitor {
  const std::unordered_map<std::string, int> &m_known_defs;
  std::ostream &m_os;

public:
  text_processor(std::ostream &os, const std::unordered_map<std::string, int> &defs)
      : m_known_defs{defs}, m_os(os) {}

  void apply(ast::directive &ref) override {
      evaluator evtr(m_known_defs, ref.kind());
      assert(ref.cond());
      ref.cond()->accept(evtr);
      if (evtr.value()) ref.true_block()->accept(*this);
      else if (ref.else_block()) ref.else_block()->accept(*this);
  }
  void apply(ast::block &ref) override {
    for (auto *line : ref) {
      line->accept(*this);
    }
  }
  void apply(ast::line &ref) override { m_os << ref.content() << '\n'; }
  void apply(ast::error_node &) override {}
  void apply(ast::unary_expression &) override {}
  void apply(ast::binary_expression &) override {}
  void apply(ast::variable_expression &) override {}
  void apply(ast::constant_expression &) override {}
};

void process_text(
    bunifdef::frontend::ast::ast_container &ast, std::ostream &os,
    const std::unordered_map<std::string, int> &defs
) {
  text_processor tproc{os, defs};
  ast.get_root_ptr()->accept(tproc);
}

} // namespace bunfidef::backend
