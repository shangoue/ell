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

namespace koalang
{
    Lexer::Lexer()
      : ell::Parser<char>(& top, & blank)
    {
        top = * lexeme( keyword
                      | ident [& Lexer::push<Lex::IDENT>]
                      | real [& Lexer::push_number]
                      | string
                      | op
                      | + ch('\n') [& Lexer::newline]);

        keyword = (( str("break")
                   | str("do")    | str("else")
                   | str("for")   | str("if")
                   | str("in")    | str("return")
                   | str("xor")   | str("or")
                   | str("and")   | str("while") ) >> eps - alnum) [& Lexer::push<Lex::OP>];

        string = ch('\"') [& Lexer::push_string] >> string_char * ch('\"')
               | ch('\'') [& Lexer::push_string] >> string_char * ch('\'');

        string_char = ch('\\') >> ( ch('a') [& Lexer::push_char<'\a'>]
                                  | ch('b') [& Lexer::push_char<'\b'>]
                                  | ch('t') [& Lexer::push_char<'\t'>]
                                  | ch('n') [& Lexer::push_char<'\n'>]
                                  | ch('v') [& Lexer::push_char<'\v'>]
                                  | ch('f') [& Lexer::push_char<'\f'>]
                                  | ch('r') [& Lexer::push_char<'\r'>])
                                  |            integer<unsigned long,  8, 1, 3>() [& Lexer::push_char]
                                  | ch('x') >> integer<unsigned long, 16, 1, 2>() [& Lexer::push_char]
                                  | ch('u') >> integer<unsigned long, 16, 4, 4>() [& Lexer::push_char]
                                  | ch('U') >> integer<unsigned long, 16, 8, 8>() [& Lexer::push_char]
                    | any [& Lexer::push_char] - ch('\n');

        op = ( chset("-!#*/%+<>=") >> ! ch('=')
             | chset("[({") [& Lexer::open]
             | chset("])}") [& Lexer::close]
             | ch('.') >> ! ch('.')
             | ch('@') >> ! ch('@')
             | ch(':')
             ) [& Lexer::push<Lex::OP>];

        blank = chset(" \t\r");

        top.set_name(0);
        string_char.set_name(0);

        op.set_name("operator");
        keyword.set_name("keyword");
        string.set_name("string");
    }

    Grammar::Grammar()
      : number(Lex(Lex::NUM)),
        string(Lex(Lex::STR)),
        identifier(Lex(Lex::IDENT)),
        newline(Lex(Lex::NL))
    {
#       define I & Interpreter
        top = no_step_back(! newline >> * statement >> end);

        statement = ( op("if") >> expression
                    | op("else") >> statement
                    | op("for") >> expression
                    | op("in") >> expression
                    | op("while") >> expression
                    | op("do")
                    | op("break")
                    | op("return") >> expression
                    | scoped [is_operator] >> * expression
                    | expression >> ( op(":") >> expression [I::define]
                                    | op("=") >> expression [I::push<ASSIGN, 2>]
                                    | * expression >> scoped [is_operator] >> * expression )
                    ) >> ! newline;

        expression = order >> * ( op("and") >> order
                                | op("or") >> order
                                | op("xor") >> order );

        order = sum >> * ( op("==") >> sum
                         | op("!=") >> sum
                         | op("<=") >> sum
                         | op(">=") >> sum
                         | op("<") >> sum
                         | op(">") >> sum );

        sum = product >> * ( op("+") >> product [I::push<ADD, 2>]
                           | op("-") >> product [I::push<SUB, 2>]);

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

        atome = op("(") >> * statement >> op(")")
              | op("{") >> * statement >> op("}")
              | op("<") >> * identifier >> op(">")
              | number [I::push<Real>]
              | string [I::push<String>]
              | identifier [I::push<Variable>]
              | op("@@");
#       undef I

        top.set_name(0);

        ELL_NAME_RULE(statement)
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
