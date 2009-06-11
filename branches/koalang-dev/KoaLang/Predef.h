#ifndef __KOALANG_PREDEF_H__
#define __KOALANG_PREDEF_H__

#include "Types.h"

namespace koalang
{
    struct Predef : public Function
    {
        Predef()
        {
            right.push_back(new Variable("target"));
        }

        void describe(std::ostream & os) const
        {
            os << "( <" << right << "> -> ... )";
        }
    };

    struct Print : public Predef
    {
        Object * concrete_eval(Map * context)
        {
            std::cout << * (context->look_up("target")->eval(context)) << std::endl;
            return new List;
        }
    };

    struct Input : public Predef
    {
        Object * concrete_eval(Map * context)
        {
            std::cout << context->look_up("target")->eval(context)->to<String>()->value;
            std::string line;
            std::getline(std::cin, line);
            return new String(line);
        }
    };

    struct Eval : public Predef
    {
        Object * concrete_eval(Map * context)
        {
            String * s = context->look_up("target")->eval(context)->to<String>();
            List * l = Parser().parse(s->value, "TODO: add location info within objects");
            return l->eval(context);
        }
    };
}

#endif // __KOALANG_PREDEF_H__
