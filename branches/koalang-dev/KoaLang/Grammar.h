#ifndef __KOALANG_GRAMMAR__
#define __KOALANG_GRAMMAR__

#include "Lexer.h"

namespace koalang
{
    struct Grammar : public ell::Grammar<Lex>
    {
        Grammar();

        ell::Ch<Lex> op(const char * op)
        {
            return ell::Ch<Lex>(Lex(op, Lex::OP));
        }

        ell::Ch<Lex> number, string, identifier, newline;

        ell::Rule<Lex> program, statement, define, assignation, expression,
                       logical, order, sum, product, unary, selection,
                       variable, parameters, atome, skipper;
    };
}

#endif // __KOALANG_GRAMMAR__
