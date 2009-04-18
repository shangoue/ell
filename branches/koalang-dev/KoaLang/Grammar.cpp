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
#include "Interpreter.h"
#include "Operators.h"

namespace koalang
{
    Grammar::Grammar()
      : number(Lex(Lex::NUM)),
        string(Lex(Lex::STR)),
        identifier(Lex(Lex::IDENT)),
        newline(Lex(Lex::NL))
    {
#       define I & Interpreter
        top = no_look_ahead(* statement >> end);

        statement = skip( op("if") >> expression
                        | op("else") >> statement
                        | op("for") >> expression
                        | op("in") >> expression
                        | op("while") >> expression
                        | op("do")
                        | op("break")
                        | op("return") >> expression
                        | (op("print") >> expression) [I::unary<Print>]
                        | assignation );

        assignation = expression >> ! ( op(":") >> expression [I::define]
                                      | (op("=") >> expression) [I::binary<Assign>] );

        // TODO: use no_skipper
        expression = no_skip( scoped [I::is_defined] >> * logical
                            | + logical >> ! (scoped [I::is_defined] >> * logical) );

        logical = order >> * ( op("and") >> order
                             | op("or") >> order
                             | op("xor") >> order );

        order = sum >> * ( op("==") >> sum
                         | op("!=") >> sum
                         | op("<=") >> sum
                         | op(">=") >> sum
                         | op("<") >> sum
                         | op(">") >> sum );

        sum = product >> * ( (op("+") >> product) [I::binary<Add>]
                           | op("-") >> product);

        product = unary >> * ( op("*") >> unary
                             | op("/") >> unary
                             | op("%") >> unary );

        unary = op("-") >> unary
              | op("!") >> unary
              | op("#") >> unary
              | op("@") >> unary
              | selection;

        selection = scoped >> * (op("[") >> expression >> op("]"));

        scoped = atome >> * ( op("..") >> atome
                            | op(".") >> atome );

        formal_parameters = op("<") >> * identifier >> op(">");

        atome = op("(") >> * statement >> op(")")
              | op("{") >> * statement >> op("}")
              | formal_parameters >> identifier >> ! formal_parameters
              | op("`") >> * identifier >> op("`")
              | number [I::push<Real>]
              | string [I::push<String>]
              | identifier [I::push<Variable>] >> ! formal_parameters
              | op("@@");
#       undef I

        top.set_name(0);
        ELL_NAME_RULE(statement)
        ELL_NAME_RULE(assignation)
        ELL_NAME_RULE(expression)
        ELL_NAME_RULE(logical)
        ELL_NAME_RULE(order)
        ELL_NAME_RULE(sum)
        ELL_NAME_RULE(product)
        ELL_NAME_RULE(unary)
        ELL_NAME_RULE(selection)
        ELL_NAME_RULE(scoped)
        ELL_NAME_RULE(atome)
    }
}
