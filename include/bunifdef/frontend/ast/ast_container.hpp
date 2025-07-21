#pragma once

#include "ast_nodes.hpp"

#include <cassert>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace bunifdef::frontend::ast {

using i_ast_node_uptr = std::unique_ptr<i_ast_node>;

class ast_container final {
private:
  std::vector<i_ast_node_uptr> m_nodes;
  i_ast_node *m_root = nullptr;

  template <typename T, typename... Ts> T &emplace_back(Ts &&...args) {
    auto uptr = std::make_unique<T>(std::forward<Ts>(args)...);
    auto ptr = uptr.get();
    m_nodes.push_back(std::move(uptr));
    return *ptr;
  }

public:
  ast_container() = default;

  ast_container(ast_container &&other) = default;
  ast_container &operator=(ast_container &&other) = default;
  ~ast_container() = default;

  void set_root_ptr(i_ast_node *ptr) { m_root = ptr; }

  i_ast_node *get_root_ptr() const & { return m_root; }

  template <typename t_node_type, typename... t_args>
  t_node_type &make_node(t_args &&...args)
    requires std::is_base_of_v<i_ast_node, t_node_type>
  {
    return emplace_back<t_node_type>(std::forward<t_args>(args)...);
  }
};

} // namespace bunifdef::frontend::ast
