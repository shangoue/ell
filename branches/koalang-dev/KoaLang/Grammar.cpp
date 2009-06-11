#include "Grammar.h"
#include "Parser.h"
#include "Operators.h"

namespace koalang
{
    Grammar::Grammar()
      : number(Lex(Lex::NUM)),
        string(Lex(Lex::STR)),
        identifier(Lex(Lex::IDENT)),
        newline(Lex(Lex::NL))
    {
#       define P & Parser

        program = no_look_ahead(* statement >> end);

        statement = skip(define | assignation);

        //TODO: add anonymous function handling within expr : f = (<a><b> -> a + b)
        define = look_ahead( parameters
                             >> identifier [P::push<Variable>]
                             >> parameters >> op(":") ) >> statement [P::push_define];

        parameters = eps [P::push<List>] >> ! (op("<") >> * identifier [P::append<Variable>] >> op(">"));

        assignation = expression >> ! (op("=") >> expression) [P::push_binary<Assign>];

        expression = no_skip(look_ahead(logical >> ! (logical [P::push_bunch] >> * logical [P::append])));

        logical = order >> * ( (op("&&") >> order) [P::push_binary<And>]
                             | (op("||") >> order) [P::push_binary<Or>]
                             | (op("^") >> order) [P::push_binary<Xor>] );

        order = sum >> * ( (op("==") >> sum) [P::push_binary<Eq>]
                         | (op("\\=") >> sum) [P::push_binary<NotEq>]
                         | (op("<=") >> sum) [P::push_binary<LE>]
                         | (op(">=") >> sum) [P::push_binary<GE>]
                         | (op("<")  >> sum) [P::push_binary<LT>]
                         | (op(">")  >> sum) [P::push_binary<GT>] );

        sum = product >> * ( (op("+") >> product) [P::push_binary<Add>]
                           | (op("-") >> product) [P::push_binary<Sub>]);

        product = unary >> * ( (op("*") >> unary) [P::push_binary<Mult>]
                             | (op("/") >> unary) [P::push_binary<Div>]
                             | (op("%") >> unary) [P::push_binary<Mod>]);

        unary = op("-") >> unary
              | op("\\") >> unary
              | op("!") >> unary
              | op("?") >> unary
              | op("#") >> unary
              | op("@") >> unary
              | selection;

        selection = atome >> * (op("[") >> expression >> op("]"));

        atome = op("(") [P::push<List>] >> * statement [P::append] >> op(")")
              | op("{") [P::push<Block>] >> * statement [P::append] >> op("}")
              | number [P::push<Real>]
              | string [P::push<String>]
              | variable
              | op("@@") [P::push<Scope>];

        variable = identifier [P::push<Variable>] >> * (op(".") >> identifier)
                 | op("..") >> identifier;

        skipper = op(",") | newline;
#       undef I

        ELL_NAME_RULE(program)
        ELL_NAME_RULE(statement)
        ELL_NAME_RULE(define)
        ELL_NAME_RULE(parameters)
        ELL_NAME_RULE(assignation)
        ELL_NAME_RULE(expression)
        ELL_NAME_RULE(atome)
        ELL_NAME_RULE(variable)

        logical.name = order.name = sum.name = product.name = unary.name = selection.name = "expression";

    }
}
