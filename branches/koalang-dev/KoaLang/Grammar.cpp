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
                        | (op("print") >> expression) [I::push_unary<Print>]
                        | define
                        | assignation );

        //TODO: add anonymous function handling within expr : f = (<a><b> -> a + b)
        define = look_ahead( parameters
                             >> identifier [I::push<Variable>]
                             >> parameters >> op(":") ) >> statement [I::push_define];

        parameters = eps [I::push<List>] >> ! (op("<") >> * identifier [I::append<Variable>] >> op(">"));

        assignation = expression >> ! (op("=") >> expression) [I::push_binary<Assign>];

        expression = no_skip(logical >> ! (logical [I::push_bunch] >> * logical [I::append]));
        
        logical = order >> * ( (op("and") >> order) [I::push_binary<And>]
                             | (op("or") >> order) [I::push_binary<Or>]
                             | (op("xor") >> order) [I::push_binary<Xor>] );

        order = sum >> * ( (op("==") >> sum) [I::push_binary<Eq>]
                         | (op("!=") >> sum) [I::push_binary<NotEq>]
                         | (op("<=") >> sum) [I::push_binary<LE>]
                         | (op(">=") >> sum) [I::push_binary<GE>]
                         | (op("<")  >> sum) [I::push_binary<LT>]
                         | (op(">")  >> sum) [I::push_binary<GT>] );

        sum = product >> * ( (op("+") >> product) [I::push_binary<Add>]
                           | (op("-") >> product) [I::push_binary<Sub>]);

        product = unary >> * ( (op("*") >> unary) [I::push_binary<Mult>]
                             | (op("/") >> unary) [I::push_binary<Div>]
                             | (op("%") >> unary) [I::push_binary<Mod>]);

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
