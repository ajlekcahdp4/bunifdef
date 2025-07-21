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
. { 
  auto loc = update_loc();
  std::stringstream ss;
  ss << "Unknown character '" << yytext << "'";
  auto msg = ss.str();
  m_driver.report_error(msg, loc); throw parser::syntax_error{loc, msg}; 
}
<<EOF>> { return bunifdef::frontend::parser::make_EOF(update_loc()); }

%%
