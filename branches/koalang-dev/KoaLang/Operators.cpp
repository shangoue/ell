#include "Operators.h"
#include "Parser.h"

namespace koalang
{
    Object * Eval::eval(Map * context)
    {
        String * s = left->eval(context)->to<String>();
        List * l = Parser().parse(s->value, "TODO: add location info within objects");
        return l->eval(right->eval(context)->to<Map>());
    }
}
