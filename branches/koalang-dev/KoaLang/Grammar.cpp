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
                        | define
                        | assignation );

        define = look_ahead(! parameters >> identifier >> ! parameters >> op(":")) >> expression;

        parameters = op("<") >> * identifier >> op(">");

        assignation = expression >> ! (op("=") >> expression) [I::binary<Assign>];

        // TODO: use no_skipper
        expression = no_skip(call);
        
        call = variable [I::is_defined] >> * logical
             | logical >> ! look_ahead(* logical >> variable [I::is_defined] >> * logical);

        logical = order >> * ( (op("and") >> order) [I::binary<And>]
                             | (op("or") >> order) [I::binary<Or>]
                             | (op("xor") >> order) [I::binary<Xor>] );

        order = sum >> * ( (op("==") >> sum) [I::binary<Eq>]
                         | (op("!=") >> sum) [I::binary<NotEq>]
                         | (op("<=") >> sum) [I::binary<LE>]
                         | (op(">=") >> sum) [I::binary<GE>]
                         | (op("<")  >> sum) [I::binary<LT>]
                         | (op(">")  >> sum) [I::binary<GT>] );

        sum = product >> * ( (op("+") >> product) [I::binary<Add>]
                           | (op("-") >> product) [I::binary<Sub>]);

        product = unary >> * ( (op("*") >> unary) [I::binary<Mult>]
                             | (op("/") >> unary) [I::binary<Div>]
                             | (op("%") >> unary) [I::binary<Mod>]);

        unary = op("-") >> unary
              | op("!") >> unary
              | op("#") >> unary
              | op("@") >> unary
              | selection;

        selection = atome >> * (op("[") >> expression >> op("]"));

        atome = op("(") [I::push<List>] >> * statement [I::append] >> op(")")
              | op("{") [I::push<Block>] >> * statement [I::append] >> op("}")
              | number [I::push<Real>]
              | string [I::push<String>]
              | variable
              | op("@@") [I::push<Scope>];

        variable = identifier [I::push<Variable>] >> * (op(".") >> identifier)
                 | op("..") >> identifier;
#       undef I

        top.set_name(0);
        ELL_NAME_RULE(statement)
        ELL_NAME_RULE(define)
        ELL_NAME_RULE(parameters)
        ELL_NAME_RULE(assignation)
        ELL_NAME_RULE(expression)
        ELL_NAME_RULE(logical)
        ELL_NAME_RULE(order)
        ELL_NAME_RULE(sum)
        ELL_NAME_RULE(product)
        ELL_NAME_RULE(unary)
        ELL_NAME_RULE(selection)
        ELL_NAME_RULE(atome)
        ELL_NAME_RULE(variable)
        ELL_NAME_RULE(call)
    }
}
