#include "Types.h"
#include "Operators.h"

namespace koalang
{
    // List evaluation is a bit complicated because we must resolve any function call, which
    // keeping the order of evaluation of elements.
    Object * List::eval(Map * context)
    {
        List * result = new List;
        unsigned begin = 0;

        for (unsigned i = 0; i < value.size(); ++i)
        {
            Object * v = value[i];
            if (v->is<Variable>())
            {
                v = context->look_up(((Variable *) value[i])->value);

                if (v->is<Function>())
                {
                    Function * f = (Function *) v;
               		Map * locals = new Map(context);

                    for (unsigned left = 0; left < f->left.size(); ++left)
                    {
                        if (i - 1 - left < 0)
                            throw std::runtime_error("Lacks of a left operand");

                        Assign::assign(f->left[left], value[i - 1 - left], locals);
                    }

                    for (unsigned right = 0; right < f->right.size(); ++right)
                    {
                        if (i + 1 + right >= value.size())
                            throw std::runtime_error("Lacks of a right operand");

                        Assign::assign(f->right[right], value[i + 1 + right], locals);
                    }

                    for (unsigned j = begin; j < i - f->left.size(); ++j)
                        result->value.push_back(value[j]->eval(context));

                    result->value.push_back(f->eval(locals));

                    i = i + f->right.size();
                    begin = i + 1;
                }
            }
        }

        for (unsigned j = begin; j < value.size(); ++j)
            result->value.push_back(value[j]->eval(context));

        return result;
    }
}
