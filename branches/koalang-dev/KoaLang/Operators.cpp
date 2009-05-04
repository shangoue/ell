#include "Operators.h"
#include "Interpreter.h"

namespace koalang
{
    Object * Eval::eval(Map * context)
    {
        String * s = left->eval(context)->to<String>();
        Interpreter i;
        List * l = i.parse(s->value, "TODO: add location info within objects");
        return l->eval(right->eval(context)->to<Map>());
    }
}
