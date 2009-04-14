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
          : ell::Parser<Lex>(& grammar.top, & grammar.newline)
        {
            root_scope = new Map;
            stack = new Block;
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
            stack->value.push_back(new Object(lex));
        }

        template <typename Object>
        void binary(const Lex * name)
        {
            Object * op = new Object;
            op->right = pop();
            op->left = pop();
            op->name = name->s;
            push(op);
        }

        void define()
        {
            //TODO
        }

        bool is_defined(const Lex * name)
        {
            // TODO
            pop();
            return false;
        }

        Object * push(Object * o) { stack->value.push_back(o); return o; }
        Object * pop() { Object * o = stack->value.back(); stack->value.pop_back(); return o; }

        Lex op;
        Map * root_scope;
        Block * stack;
    };
}
#endif // __KOALANG_INTERPRETER__
