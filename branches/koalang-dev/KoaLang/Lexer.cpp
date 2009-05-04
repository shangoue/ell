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

#include "Lexer.h"

namespace koalang
{
    Lexer::Lexer()
      : ell::Parser<char>(& top, & blank)
    {
        top = * lexeme( keyword
                      | ident [& Lexer::push<Lex::IDENT>]
                      | string
                      | op
                      | real [& Lexer::push_number]
                      | + ch('\n') [& Lexer::push<Lex::NL>]);

        keyword = (( str("break") | str("print")
                   | str("input") | str("eval")
                   | str("do")    | str("else")
                   | str("for")   | str("if")
                   | str("in")    | str("return")
                   | str("xor")   | str("or")
                   | str("and")   | str("while") ) >> eps - alnum) [& Lexer::push<Lex::OP>];

        string = ch('"') [& Lexer::push_string] >> string_char * ch('"');

        string_char = ch('\\') >> ( ch('a') [& Lexer::push_char<'\a'>]
                                  | ch('b') [& Lexer::push_char<'\b'>]
                                  | ch('t') [& Lexer::push_char<'\t'>]
                                  | ch('n') [& Lexer::push_char<'\n'>]
                                  | ch('v') [& Lexer::push_char<'\v'>]
                                  | ch('f') [& Lexer::push_char<'\f'>]
                                  | ch('r') [& Lexer::push_char<'\r'>])
                                  |            integer<unsigned long,  8, 1, 3>() [& Lexer::push_char]
                                  | ch('x') >> integer<unsigned long, 16, 1, 2>() [& Lexer::push_char]
                                  | ch('u') >> integer<unsigned long, 16, 4, 4>() [& Lexer::push_char]
                                  | ch('U') >> integer<unsigned long, 16, 8, 8>() [& Lexer::push_char]
                    | any [& Lexer::push_char] - ch('\n');

        op = ( chset("\\<>=") >> ! ch('=')
             | chset(",?!#*/%+[({:])}-")
             | ch('.') >> ! ch('.')
             | ch('@') >> ! ch('@')
             ) [& Lexer::push<Lex::OP>];

        blank = chset(" \t\r") 
              | (ch('\'') >> any * ch('\n')) [& Lexer::push<Lex::NL>];

        top.set_name(0);
        string_char.set_name(0);

        op.set_name("operator");
        keyword.set_name("keyword");
        string.set_name("string");
    }
}
