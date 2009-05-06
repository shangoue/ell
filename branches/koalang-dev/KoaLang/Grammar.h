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

#ifndef __KOALANG_GRAMMAR__
#define __KOALANG_GRAMMAR__

#include "Lexer.h"

namespace koalang
{
    struct Grammar : public ell::Grammar<Lex>
    {
        Grammar();

        ell::Character<Lex> op(const char * op)
        {
            return ell::Character<Lex>(Lex(op, Lex::OP));
        }

        ell::Character<Lex> number, string, identifier, newline;

        ell::Rule<Lex> top, statement, define, assignation, expression,
                       logical, order, sum, product, unary, selection,
                       variable, parameters, atome, skipper;
    };
}

#endif // __KOALANG_GRAMMAR__
