%skeleton "lalr1.cc"
%require "3.5"

%defines

%define api.token.raw
%define api.parser.class
{ parser }

%define api.token.constructor
%define api.value.type variant

%define api.namespace 
{ bunifdef::frontend }

%define parse.error verbose
%define parse.lac full

%glr-parser

%code requires {
#include "bunifdef/frontend/ast/ast_container.hpp"
#include "bunifdef/frontend/ast/ast_nodes.hpp"

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <cstdlib>
#include <memory>
#include <stdexcept>

namespace bunifdef::frontend {
  class scanner;
  class parser_driver;
}

}

%code top {

#include "bunifdef/frontend/scanner.hpp"
#include "bunifdef_parser.hpp"
#include "bunifdef/frontend/driver.hpp"

#include <iostream>
#include <string>
#include <sstream>

static bunifdef::frontend::parser::symbol_type yylex(bunifdef::frontend::scanner &p_scanner, bunifdef::frontend::parser_driver &) {
  return p_scanner.get_next_token();
}

}

%lex-param 
{ bunifdef::frontend::scanner &scanner }

%lex-param 
{ bunifdef::frontend::parser_driver &driver }

%parse-param 
{ bunifdef::frontend::scanner &scanner }

%parse-param 
{ bunifdef::frontend::parser_driver &driver }

%define parse.trace
%define api.token.prefix {TOKEN_}

%locations
%define api.location.file "location.hpp"

/* Directives */
%token <std::pair<std::string, std::string>> IF       "#if"
%token ELSE     "#else"
%token ENDIF    "#endif"
%token EOF 0    "end of file"

/* Terminals */
%token <std::string> LINE "line"

%type <std::vector<ast::i_ast_node *>>
  block

%type <ast::directive *>
  directive

%type <ast::line *>
  line


%start text

%%

text:  
  block  { auto ptr = driver.make_ast_node<ast::block>($1, @1); driver.m_ast.set_root_ptr(ptr); }

block:
  block line { $$ = std::move($1); $$.push_back($2); }
| block directive { $$ = std::move($1); $$.push_back($2); }
| %empty {}

line:
  LINE { $$ = driver.make_ast_node<ast::line>($1, @1); }

directive:
  IF block ENDIF     { 
    auto true_block = driver.make_ast_node<ast::block>(std::vector{$2}, @2);
    $$ = driver.make_ast_node<ast::directive>($1.first, $1.second, *true_block, @$);
  }
| IF block ELSE block ENDIF  { 
    auto true_block = driver.make_ast_node<ast::block>(std::vector{$2}, @2);
    auto else_block = driver.make_ast_node<ast::block>(std::vector{$4}, @4);
    $$ = driver.make_ast_node<ast::directive>($1.first, $1.second, *true_block, *else_block, @$);
  }

%%

void bunifdef::frontend::parser::error(const location &loc, const std::string &message) {
  /* When using custom error handling this only gets called when unexpected errors occur, like running out of memory or when an exception gets thrown. 
  Don't know what to do about parser::syntax_error exception for now */

  if (std::string_view{message} == "memory exhausted") {
    throw std::runtime_error{"Bison memory exhausted"};
  }

  driver.report_error(message, loc); 
}
