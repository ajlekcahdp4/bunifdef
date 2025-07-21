#pragma once

#include "location.hpp"

namespace bunifdef::frontend::ast {

class i_ast_node;
class directive;
class error_node;
class block;
class line;

class i_visitor {
public:
  virtual ~i_visitor() {};
  virtual void apply(directive &) = 0;
  virtual void apply(block &) = 0;
  virtual void apply(line &) = 0;
  virtual void apply(error_node &) = 0;
};

class i_ast_node {
protected:
  location m_loc;

protected:
  i_ast_node(location l = location{}) : m_loc{l} {}

public:
  location loc() const { return m_loc; }

  virtual void accept(i_visitor &v) = 0;

  virtual ~i_ast_node() {}
};

class i_expression : public i_ast_node {
public:
  i_expression(location l = location{}) : i_ast_node{l} {}
};

} // namespace bunifdef::frontend::ast
