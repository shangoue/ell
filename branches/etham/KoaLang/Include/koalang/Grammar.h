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

#ifndef __KOALANG_GRAMMAR_H__
#define __KOALANG_GRAMMAR_H__

#include <ell/Parser.h>

namespace koalang
{
    struct Grammar : public parser::Rule
    {
        Grammar();

        parser::Rule number, string, variable, any, atome;
        parser::Rule scoped, assignable, unary, product, expression;
        parser::Rule ordering, comparison, assignation, statement;
        parser::Rule block_comment, skipper, ident_char;
    };
}

#endif //__KOALANG_GRAMMAR_H__
