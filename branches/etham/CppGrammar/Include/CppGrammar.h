// This file is part of Ell library.
//
// Ell library is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Ell library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Ell library.  If not, see <http://www.gnu.org/licenses/>.

#ifndef INCLUDE_PARSER_PRIVATE_HEADER
#ifndef INCLUDED_PARSER_CPPGRAMMAR_H
#define INCLUDED_PARSER_CPPGRAMMAR_H

#include "Rule.h"

struct Token
{
    enum Kind { UNKNOWN,
                HEADER_NAME,
                IDENTIFIER,
                REAL,
                INTEGER,
                BOOL,
                CHARACTER,
                STRING,
                OPERATOR } kind;

    Token(Kind kind, const char * begin, const char * end)
      : kind(kind), begin(begin), end(end)
    { }

    const char * begin;
    const char * end;
};

struct CppLexerGrammar
{
    typedef char token_t;

    CppLexerGrammar();

    CppSkipper skipper;
    Rule<token_t> token, header_name, identifier, character_literal, string_literal, punctuation, integer_literal, c_char;
};

struct CppLexer : public Parser
{
    CppLexer(const char * buffer)
      : Parser(& grammar.token, & grammar.skipper, buffer)
    { }

    std::basic_string<Token> tokens;

    template <const Kind kind>
    void push_token(const char * begin, const char * end)
    {
        tokens += Token(kind, begin, end);
    }

    static CppLexerGrammar grammar;
};


struct CppGrammar
{
    typedef Token token_t;

    CppGrammar();

    Rule<token_t> class_name,
                  identifier,
                  template_name,
                  template_argument_list,
                  conditional_expression,
                  assignment_expression;

};

#endif // INCLUDED_PARSER_CPPGRAMMAR_H
#endif // INCLUDE_PARSER_PRIVATE_HEADER
