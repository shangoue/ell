#include "CGrammar.h"
#include "CParser.h"

namespace ell
{
    CGrammar::CGrammar()
      : root()
#       define D(ID, NAME) , ID(Lex(TOK_##ID))
#       include "CTokens.def"
    {
#       define P &CParser
#       include "CGrammar.def"
#       undef P

#       define D(rule) rule.set_name(#rule);
#       include "CRules.def"
#       undef D

        root.set_transparent();
    }
}
