#ifndef __KOALANG_OPERATORS_H__
#define __KOALANG_OPERATORS_H__

#include "Types.h"

namespace koalang
{
    struct Operator : public Object
    {
        std::string name;

        void describe(std::ostream & os) const { os << name; }
    };

    struct Scope : public Operator
    {
        Scope(const std::string & n) { Operator::name = n; }

        Object * concrete_eval(Map * context) { return context; }
    };

    struct UnaryOperator : public Operator
    {
        void describe(std::ostream & os) const
        {
            os << name << ' ' << * target;
        }

        Object * target;
    };

    struct BinaryOperator : public Operator
    {
        // TODO: manage priority when unparsing
        void describe(std::ostream & os) const
        {
            os << '(' << * left << ' ' << name << ' ' << * right << ')';
        }

        Object * left, * right;
    };

    struct Assign : public BinaryOperator
    {
        Object * concrete_eval(Map * context)
        {
            assign(left, right, context);
            return new List;
        }

        static void assign(Object * left, Object * right, Map * context)
        {
            if (left->is<Variable>())
            {
                context->value[left->to<Variable>()->value] = right->eval(context);
            }
            else if (left->is<List>())
            {
                List * left_list = (List *) left;
                List * right_list = right->to<List>();

                ObjectList::iterator l = left_list->value.begin(),
                                     r = right_list->value.begin();
                while (l != left_list->value.end())
                {
                    if (r == right_list->value.end())
                        throw std::runtime_error("Left list too small for assignment");
                    assign(* l, * r, context);
                }

                if (r != right_list->value.end())
                    throw std::runtime_error("Left list too big for assignment");
            }
            else
                throw std::runtime_error("Not assignable");
        }
    };

#   define op(NAME, OP)                                              \
    struct NAME : public BinaryOperator                              \
    {                                                                \
        Object * concrete_eval(Map * context)                        \
        {                                                            \
            return new Real(left->eval(context)->to<Real>()->value   \
                        OP right->eval(context)->to<Real>()->value); \
        }                                                            \
    };
    op(Add,   +)
    op(Eq,    ==)
    op(NotEq, !=)

    op(Sub,   -)
    op(Mult,  *)
    op(Div,   /)
    op(LE,    <=)
    op(GE,    >=)
    op(LT,    <)
    op(GT,    <)
#   undef op

#   define op(NAME, OP)                                                    \
    struct NAME : public BinaryOperator                                    \
    {                                                                      \
        Object * concrete_eval(Map * context)                              \
        {                                                                  \
            return new Real(long(left->eval(context)->to<Real>()->value)   \
                        OP long(right->eval(context)->to<Real>()->value)); \
        }                                                                  \
    };
    op(And,   &)
    op(Or,    |)
    op(Xor,   ^)
    op(Mod,   %)
#   undef op
}

#endif //__KOALANG_OPERATORS_H__
