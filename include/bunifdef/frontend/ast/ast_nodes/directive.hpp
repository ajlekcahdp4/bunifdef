#pragma once

#include "i_ast_node.hpp"

#include <cassert>
#include <string_view>
#include <utility>

namespace bunifdef::frontend::ast {

enum class if_kind {
  E_IF,
  E_IFDEF,
  E_IFNDEF,
};

constexpr std::string_view if_kind_to_string(if_kind op) {
  switch (op) {
  case if_kind::E_IF: return "if";
  case if_kind::E_IFDEF: return "ifdef";
  case if_kind::E_IFNDEF: return "ifndef";
  }
  std::unreachable();
}

constexpr if_kind if_kind_from_string(std::string_view str) {
  if (str == "ifndef") return if_kind::E_IFNDEF;
  if (str == "ifdef") return if_kind::E_IFDEF;
  if (str == "if") return if_kind::E_IF;
  std::unreachable();
}

class directive : public i_ast_node {
  if_kind m_kind;
  std::string m_condition_str;
  block *m_true_block = nullptr;
  block *m_else_block = nullptr;

public:
  directive(std::string_view kind_str, const std::string &cond, block &true_block, location l)
      : i_ast_node{l}, m_kind{if_kind_from_string(kind_str)}, m_condition_str{cond},
        m_true_block{&true_block} {}

  directive(
      std::string_view kind_str, const std::string &cond, block &true_block, block &else_block,
      location l
  )
      : i_ast_node{l}, m_kind{if_kind_from_string(kind_str)}, m_condition_str{cond},
        m_true_block{&true_block}, m_else_block{&else_block} {}
  void accept(i_visitor &v) override { v.apply(*this); }
  std::string_view cond_str() const & { return m_condition_str; }
  if_kind kind() const { return m_kind; }
  block *true_block() const { return m_true_block; }
  block *else_block() const { return m_else_block; }
};

} // namespace bunifdef::frontend::ast
