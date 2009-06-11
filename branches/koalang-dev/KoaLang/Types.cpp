#include "Types.h"
#include "Operators.h"

namespace koalang
{
    Object * Object::eval(Map * context)
    {
        context->backtrace->push_back(this);
        Object * r = concrete_eval(context);
        context->backtrace->pop_back();
        return r;
    }

    static void in_place_eval(ObjectList & right, Map * context)
    {
        ObjectList left;

        while (not right.empty())
        {
            Object * o = right.back();
            right.pop_back();

            if (not o->is<Variable>())
            {
                left.push_back(o->eval(context));
                continue;
            }

            Variable * v = (Variable *) o;
            o = context->look_up(v->value);
            if (not o->is<Function>())
            {
                left.push_back(o);
                continue;
            }

            Function * f = (Function *) o;
            if (left.size() < f->left.size())
                throw std::runtime_error("Lacking of a left argument for function " + v->value);

            if (not f->right.empty())
            {
                in_place_eval(right, context);
                if (right.size() < f->right.size())
                    throw std::runtime_error("Lacking of a right argument for function " + v->value);
            }

            Map * locals = new Map(context);
            for (unsigned i = f->left.size() - 1; i >= 0; ++i)
            {
                Assign::assign(f->left[i], left.back(), locals);
                left.pop_back();
            }
            for (unsigned i = 0; i < f->right.size(); ++i)
            {
                Assign::assign(f->right[i], right[i], locals);
            }
            right.erase(right.begin(), right.begin() + f->right.size());

            left.push_back(f->eval(locals));
        }

        right = left;
    }

    Object * List::concrete_eval(Map * context)
    {
        ObjectList l = value;
        in_place_eval(l, context);

        if (l.size() == 1)
            return l.back();

        List * result = new List;
        result->value = l;
        return result;
    }
}
