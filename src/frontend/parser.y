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

/* Signle letter tokens */
%token LPAREN     "("
%token RPAREN     ")"

%token COMP_EQ  "=="
%token COMP_NE  "!="
%token COMP_GT  ">"
%token COMP_LS  "<"
%token COMP_GE  ">="
%token COMP_LE  "<="
%token EOF 0    "end of file"

%token BANG     "!"

%token PLUS       "+"
%token MINUS      "-"
%token MULTIPLY   "*"
%token DIVIDE     "/"
%token MODULUS    "%"

%token LOGICAL_AND  "&&"
%token LOGICAL_OR   "||"

/* Terminals */
%token <int> INTEGER_CONSTANT "constant"
%token <std::string> IDENTIFIER "identifier"

/* Rules that model the AST */
%type <ast::i_expression *>
  primary_expression
  multiplicative_expression
  unary_expression
  additive_expression
  comparison_expression
  equality_expression
  logical_expression
  expression


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
| expression  {
    if (!driver.parent())
      throw internal_error("Expession can be parsed only if parent directive was specified.");
  driver.parent()->set_cond($1);
}

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

primary_expression:
  INTEGER_CONSTANT            { $$ = driver.make_ast_node<ast::constant_expression>($1, @1); }
| IDENTIFIER                  { $$ = driver.make_ast_node<ast::variable_expression>($1, @1); }
| LPAREN expression RPAREN    { $$ = $2; }
| LPAREN error RPAREN         { auto e = driver.take_error(); $$ = driver.make_ast_node<ast::error_node>(e.m_error_message, e.m_loc); yyerrok; }

unary_expression: 
  PLUS unary_expression         { $$ = driver.make_ast_node<ast::unary_expression>(ast::unary_operation::E_UN_OP_POS, *$2, @$); }
| MINUS unary_expression        { $$ = driver.make_ast_node<ast::unary_expression>(ast::unary_operation::E_UN_OP_NEG, *$2, @$); }
| BANG unary_expression         { $$ = driver.make_ast_node<ast::unary_expression>(ast::unary_operation::E_UN_OP_NOT, *$2, @$); }
| primary_expression            { $$ = $1; }

multiplicative_expression:  
  multiplicative_expression MULTIPLY unary_expression   { $$ = driver.make_ast_node<ast::binary_expression>(ast::binary_operation::E_BIN_OP_MUL, *$1, *$3, @$); }
| multiplicative_expression DIVIDE unary_expression     { $$ = driver.make_ast_node<ast::binary_expression>(ast::binary_operation::E_BIN_OP_DIV, *$1, *$3, @$); }
| multiplicative_expression MODULUS unary_expression    { $$ = driver.make_ast_node<ast::binary_expression>(ast::binary_operation::E_BIN_OP_MOD, *$1, *$3, @$); }
| unary_expression                                      { $$ = $1; }

additive_expression:  
  additive_expression PLUS multiplicative_expression    { $$ = driver.make_ast_node<ast::binary_expression>(ast::binary_operation::E_BIN_OP_ADD, *$1, *$3, @$); }
| additive_expression MINUS multiplicative_expression   { $$ = driver.make_ast_node<ast::binary_expression>(ast::binary_operation::E_BIN_OP_SUB, *$1, *$3, @$); }
| multiplicative_expression                             { $$ = $1; }

comparison_expression:  
  comparison_expression COMP_GT additive_expression     { $$ = driver.make_ast_node<ast::binary_expression>(ast::binary_operation::E_BIN_OP_GT, *$1, *$3, @$); }
| comparison_expression COMP_LS additive_expression     { $$ = driver.make_ast_node<ast::binary_expression>(ast::binary_operation::E_BIN_OP_LS, *$1, *$3, @$); }
| comparison_expression COMP_GE additive_expression     { $$ = driver.make_ast_node<ast::binary_expression>(ast::binary_operation::E_BIN_OP_GE, *$1, *$3, @$); }
| comparison_expression COMP_LE additive_expression     { $$ = driver.make_ast_node<ast::binary_expression>(ast::binary_operation::E_BIN_OP_LE, *$1, *$3, @$); }
| additive_expression                                   { $$ = $1; }

equality_expression:  
  equality_expression COMP_EQ comparison_expression   { $$ = driver.make_ast_node<ast::binary_expression>(ast::binary_operation::E_BIN_OP_EQ, *$1, *$3, @$); }
| equality_expression COMP_NE comparison_expression   { $$ = driver.make_ast_node<ast::binary_expression>(ast::binary_operation::E_BIN_OP_NE, *$1, *$3, @$); }
| comparison_expression                               { $$ = $1; }

logical_expression:         
  logical_expression LOGICAL_AND equality_expression    { $$ = driver.make_ast_node<ast::binary_expression>(ast::binary_operation::E_BIN_OP_AND, *$1, *$3, @$); }
| logical_expression LOGICAL_OR equality_expression     { $$ = driver.make_ast_node<ast::binary_expression>(ast::binary_operation::E_BIN_OP_OR, *$1, *$3, @$); }
| equality_expression                                   { $$ = $1; }

expression:
  logical_expression    { $$ = $1; }


%%

void bunifdef::frontend::parser::error(const location &loc, const std::string &message) {
  /* When using custom error handling this only gets called when unexpected errors occur, like running out of memory or when an exception gets thrown. 
  Don't know what to do about parser::syntax_error exception for now */

  if (std::string_view{message} == "memory exhausted") {
    throw std::runtime_error{"Bison memory exhausted"};
  }

  driver.report_error(message, loc); 
}
