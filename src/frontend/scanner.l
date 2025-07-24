%{

#include "bunifdef_parser.hpp"
#include "bunifdef/frontend/scanner.hpp"
#include "bunifdef/frontend/driver.hpp"

#include <boost/regex.hpp>

#include <iostream>
#include <string>
#include <sstream>

%}

%option noyywrap nounput noinput nodefault 
%option c++
%option prefix="bunifdef_"
%option yyclass="scanner"
%option nounistd

number      (0|[1-9][0-9]*)
identifier  [a-zA-Z_][a-zA-Z_0-9]*
nl [\n|\r\n]
if ({nl}#(if|ifdef|ifndef)[ ].+)
else ({nl}#else)
endif ({nl}#endif)

%%
"("             { return parser::make_LPAREN(update_loc()); }
")"             { return parser::make_RPAREN(update_loc()); }

"=="            { return parser::make_COMP_EQ(update_loc()); }
"!="            { return parser::make_COMP_NE(update_loc()); }
">"             { return parser::make_COMP_GT(update_loc()); }
"<"             { return parser::make_COMP_LS(update_loc()); }
">="            { return parser::make_COMP_GE(update_loc()); }
"<="            { return parser::make_COMP_LE(update_loc()); }

"&&"            { return parser::make_LOGICAL_AND(update_loc()); }
"||"            { return parser::make_LOGICAL_OR(update_loc()); }

"!"             { return parser::make_BANG(update_loc()); }

"+"             { return parser::make_PLUS(update_loc()); }
"-"             { return parser::make_MINUS(update_loc()); }
"*"             { return parser::make_MULTIPLY(update_loc()); }
"/"             { return parser::make_DIVIDE(update_loc()); }
"%"             { return parser::make_MODULUS(update_loc()); }

{identifier}    {  auto newloc = update_loc(); return parser::make_IDENTIFIER(yytext, newloc); }
{number}        { ; auto newloc = update_loc(); return parser::make_INTEGER_CONSTANT(std::atoi(yytext), newloc); }


{if}            {
    std::string text = yytext;
    boost::cmatch what;
    auto matched = boost::regex_match(yytext, what, boost::regex("\r?\n#(if|ifdef|ifndef) .+"));
    assert(matched);
    auto type_str = what[1];
    auto condition = boost::regex_replace(text,
                            boost::regex("\r?\n#(if|ifdef|ifndef)[ ]+"),
                            "", boost::match_default | boost::format_sed);
    return parser::make_IF({type_str, condition}, update_loc());
  }

{else}          { return parser::make_ELSE(update_loc()); }
{endif}         { return parser::make_ENDIF(update_loc()); }
{nl}.*          { return parser::make_LINE(yytext + 1, update_loc()); }
[ \t]+          { m_pos.columns(yyleng); }
. { 
  auto loc = update_loc();
  std::stringstream ss;
  ss << "Unknown character '" << yytext << "'";
  auto msg = ss.str();
  m_driver.report_error(msg, loc); throw parser::syntax_error{loc, msg}; 
}
<<EOF>> { return bunifdef::frontend::parser::make_EOF(update_loc()); }

%%
