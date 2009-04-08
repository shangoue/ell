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

#include <ell/Grammar.h>

namespace koalang
{
    struct Lex
    {
        Lex(double n)
          : type(NUM),
            n(n)
        { }

        Lex(const char * begin, const char * end, Type type)
          : type(type),
            s(begin, end)
        { }

        enum Type { NUM, STR, IDENT, OP } type;

        union
        {
            double n;
            std::string s;
        };
    };

    struct Lexer : public ell::Grammar<char>, public ell::Parser<char>
    {
        Lexer()
        {
            top = * lexeme( keyword
                          | ident
                          | real [L::push_number]
                          | string
                          | operator);

            ident_char = alnum | utf8nonascii;

            keyword = (( str("break") | str("def")
                       | str("do") | str("else")
                       | str("for") | str("if")
                       | str("in") | str("return")
                       | str("while")) >> eps - ident_char) [L::push<OP>];

            ident = (+ ident_char - digit) [L::push<IDENT>];

            string = ch('\"') [L::push_string] >> string_content * ch('\"')
                   | ch('\'') [L::push_string] >> string_content * ch('\'');

            string_content = ch('\\') >> any [L::push_escaped_code]
                           | any [L::push_char];

            operator = ( repeat<1,2>(ch('.'))
                       | chset("-!#*/%+<>&|^=") >> ! ch('=')
                       | chset("[](){}:")) [L::push<OP>];

            skipper = blank
                    | ch('\'') >> any * (ch('\n') | eos)
                    | block_comment;

            block_comment = str("/'") >> (block_comment | any) * str("'/");
        }

        void push_number(double n) { output.push_back(Lex(n)); }

        template <const int T>
        void push(const char * begin) { output.push_back(Lex(begin, position, T)); }

        void push_string() { output.push_back(Lex(position, position, STR)); }

        void push_char(char c) { output.back().s += c; }

        void push_escaped_code(char c)
        {
            char r = c;
            switch (c)
            {
            case 'a': r = '\a'; break;
            case 'b': r = '\b'; break;
            case 't': r = '\t'; break;
            case 'n': r = '\n'; break;
            case 'v': r = '\v'; break;
            case 'f': r = '\f'; break;
            case 'r': r = '\r'; break;
            case '0': r = '\0'; break;
            }
            push_char(r);
        }

        std::vector<Lex> output;
    };

    struct Grammar : public ell::Grammar<Lex>
    {
        Grammar();

        ell::Rule<char> top, ident_char, 
    };
}
