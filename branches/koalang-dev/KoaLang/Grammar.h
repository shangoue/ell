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
        Lex(Type type = END)
          : type(type)
        { }

        Lex(double n)
          : type(NUM),
            n(n)
        { }

        Lex(const char * begin, const char * end, Type type)
          : type(type),
            s(begin, end)
        { }

        Lex(const char * s, Type type)
          : type(type),
            s(s)
        { }

        bool operator == (const Lex & other) const
        {
            if (type != other.type)
                return false;

            if (type == OP)
                return (s == other.s);
            else
                return true;
        }

        operator bool () { return type != END; }

        enum Type
        {
            END,
            NUM,
            STR,
            IDENT,
            OP
        } type;

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
                          | op );

            keyword = (( str("break") | str("def")
                       | str("do")    | str("else")
                       | str("for")   | str("if")
                       | str("in")    | str("return")
                       | str("while") ) >> eps - alnum) [L::push<OP>];

            string = ch('\"') [L::push_string] >> string_char * ch('\"');
                   | ch('\'') [L::push_string] >> string_char * ch('\'');

            string_char = ch('\\') >> (            integer(unsigned,  8, 1, 3) [L::push_char]
                                      | ch('x') >> integer(unsigned, 16, 1, 2) [L::push_char]
                                      | ch('u') >> integer(unsigned, 16, 4, 4) [L::push_char]
                                      | ch('U') >> integer(unsigned, 16, 8, 8) [L::push_char]
                                      | any [L::push_escaped_code] )
                        | any [L::push_char] - ch('\n');

            op = ( repeat<1,2>(ch('.'))
                 | chset("-!#*/%+<>&|^=") >> ! ch('=')
                 | chset("[](){}:")) [L::push<OP>];
        }

        ell::Rule<char> top, keyword, string, string_char, op;

        void push_number(double n)
        {
            output += Lex(n);
        }

        template <const int T>
        void push(const char * begin)
        {
            output += Lex(begin, position, T);
        }

        void push_string()
        {
            output += Lex(position, position, STR);
        }

        void push_char(char c)
        {
            output.end()->s += c;
        }

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
            }
            push_char(r);
        }

        std::basic_string<Lex> output;
    };

    struct Grammar : public ell::Grammar<Lex>
    {
        Grammar();

        Character<Lex> op(const char * op)
        {
            return Character<Lex>(Lex(op, Lex::OP));
        }

        Character<Lex> number, string, identifier;

        ell::Rule<Lex> top, statement, assignation, expression,
                       order, sum, product, unary, selection,
                       call, scoped, atome;
    };
}
