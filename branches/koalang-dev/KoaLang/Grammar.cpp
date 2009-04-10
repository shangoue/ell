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
      : number(Lex(Lex::NUM)),
        string(Lex(Lex::STR)),
        identifier(Lex(Lex::IDENT))
    {
#       define I & Interpreter

        top = no_step_back(* statement) >> ( end
                                           | error(unexp_char_error) );

        statement = op("(") >> + statement >> op(")")
                  | op('{') >> + statement >> op('}')
                  | op("def") >> identifier >> (op("<") >> * identifier >> op(">") >> op(":") >> statement
                  | op("if") >> expression >> statement >> ! (op("else") >> statement)
                  | op("for") >> identifier >> op("in") >> expression >> statement
                  | op("while") >> expression >> statement
                  | op("do") >> statement >> op("while") >> expression
                  | op("break")
                  | op("return") >> expression
                  | assignation;

        assignation = expression >> op("=") >> statement;

        expression = order >> * ( op("&") >> order
                                | op("|") >> order
                                | op("^") >> order );

        order = sum >> * ( op("==") >> sum
                         | op("!=") >> sum
                         | op("<=") >> sum
                         | op(">=") >> sum
                         | op("<") >> sum
                         | op(">") >> sum );

        sum = product >> * ( op("+") >> product
                           | op("-") >> product );

        product = unary >> * ( op("*") >> unary
                             | op("/") >> unary
                             | op("%") >> unary );

        unary = op("-") >> unary
              | op("!") >> unary
              | op("#") >> unary
              | selection;

        selection = call >> * (op("[") >> expression >> op("]"));

        call = scoped >> * (op("<") >> * expression >> op(">"));

        scoped = atome >> * ( op("..") >> atome
                            | op(".") >> atome );

        atome = op("(") >> + statement >> op(")")
              | op('{') >> + statement >> op('}')
              | number
              | string
              | identifier;

#       undef I

        ELL_NAME_RULE(top)
        ELL_NAME_RULE(statement)
        ELL_NAME_RULE(assignation)
        ELL_NAME_RULE(expression)
        ELL_NAME_RULE(order)
        ELL_NAME_RULE(sum)
        ELL_NAME_RULE(product)
        ELL_NAME_RULE(unary)
        ELL_NAME_RULE(selection)
        ELL_NAME_RULE(call)
        ELL_NAME_RULE(scoped)
        ELL_NAME_RULE(atome)
    }
}
