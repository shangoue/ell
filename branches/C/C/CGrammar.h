#ifndef INCLUDED_ELL_C_GRAMMAR_H
#define INCLUDED_ELL_C_GRAMMAR_H

#include <ell/Grammar.h>

#include "CLexer.h"

namespace ell
{
    struct CGrammar : public Grammar<Lex>
    {
        CGrammar();

        Rule<Lex> root
#       define D(rule) , rule
#       include "CRules.def"
#       undef D
        ;

#       define D(ID, NAME) Ch<Lex> ID;
#       include "CTokens.def"
    };
}

#endif // INCLUDED_ELL_C_GRAMMAR_H
