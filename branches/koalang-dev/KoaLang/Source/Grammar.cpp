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

#include <koalang/Grammar.h>

namespace koalang
{
    namespace Language
    {
        Grammar::Grammar()
        {
            top=& (statement * eos_p | error_p(unexp_char_error));

            ident_char=alnum_p | utf8nonascii_p;

            number=real_p [& Interpreter::push_number];

            string=ch_p('"') [& Interpreter::push_string];

            variable = lexeme_d(+ ident_char - digit_p) [& Interpreter::push_variable];

            any=(statement >> ! (statement [& Interpreter::begin_list] >> * statement) [& Interpreter::end_list]) |
                eps_p [& Interpreter::push_empty];

            atome=number | string | variable |
                  ch_p('(') >> any >> ch_p(')') |
                  ch_p('{') >> any >> ch_p('}') [& Interpreter::push_op<1, SCOPE>];

            scoped=atome >> * (str_p("..") >> atome [& Interpreter::push_op<2, PARENT>] |
                               ch_p('.') >> atome [& Interpreter::push_op<2, MEMBER>]);

            assignable=scoped >> * (ch_p('[') >> scoped >> (str_p("..") >> scoped [& Interpreter::push_op<3, SLICE>] |
                                                            eps_p [& Interpreter::push_op<2, SELECT>]) >> ch_p(']'));

            unary=ch_p('-') >> unary [& Interpreter::push_op<1, NEG>] |
                  ch_p('!') >> unary [& Interpreter::push_op<1, NOT>] |
                  ch_p('#') >> unary [& Interpreter::push_op<1, COUNT>] |
                  assignable;

            product=unary >> * (ch_p('*') >> unary [& Interpreter::push_op<2, MULT>] |
                                ch_p('/') >> unary [& Interpreter::push_op<2, DIV>] |
                                ch_p('%') >> unary [& Interpreter::push_op<2, MODULO>]);

            expression=product >> * (ch_p('+') >> product [& Interpreter::push_op<2, ADD>] |
                                     ch_p('-') >> product [& Interpreter::push_op<2, SUB>]);

            ordering=expression >> * (str_p("==") >> expression [& Interpreter::push_op<2, EQ>] |
                                      str_p("!=") >> expression [& Interpreter::push_op<2, NE>] |
                                      str_p("<=") >> expression [& Interpreter::push_op<2, LE>] |
                                      str_p(">=") >> expression [& Interpreter::push_op<2, GE>] |
                                      ch_p('<') >> expression [& Interpreter::push_op<2, LT>] |
                                      ch_p('>') >> expression [& Interpreter::push_op<2, GT>]);

            comparison=ordering >> * (ch_p('&') >> ordering [& Interpreter::push_op<2, AND>] |
                                      ch_p('|') >> ordering [& Interpreter::push_op<2, OR>] |
                                      ch_p('^') >> ordering [& Interpreter::push_op<2, XOR>]);

            assignation=comparison >> ! (ch_p('=') >> comparison [& Interpreter::push_op<2, ASSIGN>]);

#           define kw_p(str) tk_p(str, ident_char)

            statement=kw_p("def") >> variable >> (ch_p('[') >> any >> ch_p(']') |
                                                  eps_p [& Interpreter::push_empty]) >> ch_p(':') >> statement [& Interpreter::push_op<3, DEF>] |
                      kw_p("if") >> comparison >> statement >> (kw_p("else") >> statement |
                                                                eps_p [& Interpreter::push_nothing]) [& Interpreter::push_op<3, IF>] |
                      kw_p("for") >> variable >> kw_p("in") >> assignation >> statement [& Interpreter::push_op<3, FOREACH>] |
                      kw_p("while") >> comparison >> statement [& Interpreter::push_op<2, WHILE>] |
                      kw_p("do") >> statement >> kw_p("while") >> comparison [& Interpreter::push_op<2, DOWHILE>] |
                      kw_p("break") [& Interpreter::push_op<0, BREAK>] |
                      kw_p("return") >> assignation [& Interpreter::push_op<1, RETURN>] |
                      assignation;

            block_comment=str_p("/'") >> ((block_comment | range_p(1, 255)) * str_p("'/"));

            skipper=chset_p(" \n\t\r") |
                    ch_p('\'') >> range_p(1, 255) * (ch_p('\n') | check_d(eos_p)) |
                    block_comment;

            KOALANG_PARSER_NAME_RULE(number)
            KOALANG_PARSER_NAME_RULE(string)
            KOALANG_PARSER_NAME_RULE(variable)
            KOALANG_PARSER_NAME_RULE(any)
            KOALANG_PARSER_NAME_RULE(atome)
            KOALANG_PARSER_NAME_RULE(scoped)
            KOALANG_PARSER_NAME_RULE(assignation)
            KOALANG_PARSER_NAME_RULE(unary)
            KOALANG_PARSER_NAME_RULE(product)
            KOALANG_PARSER_NAME_RULE(expression)
            KOALANG_PARSER_NAME_RULE(ordering)
            KOALANG_PARSER_NAME_RULE(comparison)
            KOALANG_PARSER_NAME_RULE(assignation)
            KOALANG_PARSER_NAME_RULE(statement)

            KOALANG_PARSER_TRANSPARENT_RULE(ident_char)
            KOALANG_PARSER_NAME_RULE(skipper)
            KOALANG_PARSER_TRANSPARENT_RULE(block_comment)
        }
    } // Language namespace
}
