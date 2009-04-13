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

#ifndef __KOALANG_INTERPRETER__
#define __KOALANG_INTERPRETER__

#include "Grammar.h"
#include "Types.h"

namespace koalang
{
    struct Interpreter : public ell::Parser<Lex>
    {
        Interpreter()
          : ell::Parser<Lex>(& grammar.top)
        {
            stack = new List;
        }

        void parse(const char * buffer, const char * file = "<stdin>")
        {
            lexer.parse(buffer);
            ell::Parser<Lex>::parse(file, lexer.lexemes);
        }

        Lexer lexer;
        Grammar grammar;

        template <typename Object>
        void push(const Lex & lex)
        {
            stack->children.push_back(new Object(lex));
        }

        List * stack;
    };
}
#endif // __KOALANG_INTERPRETER__
