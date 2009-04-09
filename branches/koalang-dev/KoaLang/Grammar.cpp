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

#include "Grammar.h"
#include "Messages.h"

namespace koalang
{
    Grammar::Grammar()
    {
#       define I & Interpreter

        top = no_step_back(* statement) >> ( eos
                                           | error(unexp_char_error) );

        statement = kw("def") >> variable >> (ch('[') >> any >> ch(']') |
                                              eps [I::push_empty]) >> ch(':') >> statement [I::push_op<3, DEF>] |
                    kw("if") >> comparison >> statement >> (kw("else") >> statement |
                                                            eps [I::push_empty]) [I::push_op<3, IF>] |
                    kw("for") >> variable >> kw("in") >> assignation >> statement [I::push_op<3, FOREACH>] |
                    kw("while") >> comparison >> statement [I::push_op<2, WHILE>] |
                    kw("do") >> statement >> kw("while") >> comparison [I::push_op<2, DOWHILE>] |
                    kw("break") [I::push_op<0, BREAK>] |
                    kw("return") >> assignation [I::push_op<1, RETURN>] |
                    assignation;

        variable = lexeme(+ ident_char - digit) [I::push_variable];

        string = ch('"') >> any[I::push_string];

        soft_unit = (statement >> ! (statement [I::begin_list] >> * statement) [I::end_list])
                  | eps [I::push_empty];

        atome = real [I::push_number];
              | string
              | variable
              | ch('(') >> any >> ch(')')
              | ch('{') >> any >> ch('}') [I::push_op<1, SCOPE>];

        scoped = atome >> * (str("..") >> atome [I::push_op<2, PARENT>] |
                           ch('.') >> atome [I::push_op<2, MEMBER>]);

        assignable = scoped >> * (ch('[') >> scoped >> (str("..") >> scoped [I::push_op<3, SLICE>] |
                                                        eps [I::push_op<2, SELECT>]) >> ch(']'));

        unary = ch('-') >> unary [I::push_op<1, NEG>] |
              ch('!') >> unary [I::push_op<1, NOT>] |
              ch('#') >> unary [I::push_op<1, COUNT>] |
              assignable;

        product = unary >> * (ch('*') >> unary [I::push_op<2, MULT>] |
                            ch('/') >> unary [I::push_op<2, DIV>] |
                            ch('%') >> unary [I::push_op<2, MODULO>]);

        expr = product >> * (ch('+') >> product [I::push_op<2, ADD>] |
                                 ch('-') >> product [I::push_op<2, SUB>]);

        ordering = expr >> * (str("==") >> expr [I::push_op<2, EQ>] |
                                  str("!=") >> expr [I::push_op<2, NE>] |
                                  str("<=") >> expr [I::push_op<2, LE>] |
                                  str(">=") >> expr [I::push_op<2, GE>] |
                                  ch('<') >> expr [I::push_op<2, LT>] |
                                  ch('>') >> expr [I::push_op<2, GT>]);

        comparison = ordering >> * (ch('&') >> ordering [I::push_op<2, AND>] |
                                  ch('|') >> ordering [I::push_op<2, OR>] |
                                  ch('^') >> ordering [I::push_op<2, XOR>]);

        assignation = comparison >> ! (ch('=') >> comparison [I::push_op<2, ASSIGN>]);
#       undef I

        ELL_NAME_RULE(number)
        ELL_NAME_RULE(string)
        ELL_NAME_RULE(variable)
        ELL_NAME_RULE(any)
        ELL_NAME_RULE(atome)
        ELL_NAME_RULE(scoped)
        ELL_NAME_RULE(assignation)
        ELL_NAME_RULE(unary)
        ELL_NAME_RULE(product)
        ELL_NAME_RULE(expr)
        ELL_NAME_RULE(ordering)
        ELL_NAME_RULE(comparison)
        ELL_NAME_RULE(assignation)
        ELL_NAME_RULE(statement)
    }
    } // Language namespace
}
