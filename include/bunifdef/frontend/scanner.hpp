#pragma once

#if !defined(yyFlexLexerOnce)
#undef yyFlexLexer
#define yyFlexLexer bunifdef_FlexLexer
#include <FlexLexer.h>
#endif

#undef YY_DECL
#define YY_DECL                                                                                    \
  bunifdef::frontend::parser::symbol_type bunifdef::frontend::scanner::get_next_token()

#include "bunifdef_parser.hpp"
#include "location.hpp"

namespace bunifdef::frontend {
class parser_driver;

class scanner final : public yyFlexLexer {
private:
  parser_driver &m_driver;
  position m_pos;

private:
  auto symbol_length() const { return yyleng; }

public:
  scanner(parser_driver &driver, std::string *filename = nullptr)
      : m_driver{driver}, m_pos{filename} {}
  bunifdef::frontend::parser::symbol_type get_next_token();

  location update_loc() {
    auto old_pos = m_pos;
    auto new_pos = (m_pos += symbol_length());
    return location{old_pos, new_pos};
  }
};

} // namespace bunifdef::frontend
