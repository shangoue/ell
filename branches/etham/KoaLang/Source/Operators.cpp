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

#include "koalang/Operators.h"

#include <math.h>

namespace koalang
{
#   define P1 params[0]->what()
#   define P2 params[1]->what()
#   define P3 params[2]->what()
#   define D(nb_args, id, desc)                              \
    const std::string Operator<id>::what(bool enclose)       \
    {                                                        \
        std::ostringstream oss;                              \
        oss << desc;                                         \
        if (enclose) return '(' + oss.str() + ')';           \
        return oss.str();                                    \
    }
    OPERATORS
#   undef D
#   undef P3
#   undef P2
#   undef P1

    Abstract * Operator<BREAK>::eval(Object * o)
    {
        -- o->loops;
        return new Nothing;
    }

    Abstract * Operator<RET>::eval(Abstract * pe1, Object * o)
    {
        o->loops = 0;
        return o->return_value = pe1;
    }

    Abstract * Operator<NEG>::eval(Abstract * pe1, Object * o)
    {
        if (! pe1->isNumber()) raise_error(number_expected_error + what());

        return new Number(- ((Number *) pe1)->value);
    }

    Abstract * Operator<NOT>::eval(Abstract * pe1, Object * o)
    {
        return new Number(! pe1->test());
    }

    Abstract * Operator<COUNT>::eval(Abstract * pe1, Object * o)
    {
        int r;

        if (pe1->isNothing())
        {
            r = 0;
        }
        else if (pe1->isList())
        {
            r = ((List *) pe1)->size();
        }
        else
            r = 1;

        return new Number(r);
    }

    Abstract * Operator<EQ>::eval(Abstract * pe1, Abstract * pe2, Object * o)
    {
        return new Number(equal(pe1, pe2));
    }

    Abstract * Operator<NE>::eval(Abstract * pe1, Abstract * pe2, Object * o)
    {
        return new Number(! equal(pe1, pe2));
    }

#   define GET_ORDER                                                      \
        bool r1 = pe1->greater_or_equal(pe2);                             \
        bool r2 = pe2->greater_or_equal(pe1);                             \
        if (! r1 && ! r2)                                                 \
        {                                                                 \
            raise_error(incompatible_types_error + what()); \
        }

    Abstract * Operator<LE>::eval(Abstract * pe1, Abstract * pe2, Object * o)
    {
        GET_ORDER
        return new Number(r2);
    }

    Abstract * Operator<GE>::eval(Abstract * pe1, Abstract * pe2, Object * o)
    {
        GET_ORDER
        return new Number(r1);
    }

    Abstract * Operator<LT>::eval(Abstract * pe1, Abstract * pe2, Object * o)
    {
        GET_ORDER
        return new Number(! r1);
    }

    Abstract * Operator<GT>::eval(Abstract * pe1, Abstract * pe2, Object * o)
    {
        GET_ORDER
        return new Number(! r2);
    }

    Abstract * Operator<MULT>::eval(Abstract * pe1, Abstract * pe2, Object * o)
    {
        if (! pe2->isNumber())
        {
            raise_error(number_expected_error + pe2->what());
        }

        double n=((Number *) pe2)->value;

        if (pe1->isNumber()) return new Number(((Number *) pe1)->value * n);

        if (pe1->isIndexable())
        {
            int N = (int) n;

            if (N <= 0) return new Nothing;

            Indexable * id = (Indexable *) pe1;
            Indexable * ni = id->get_slice(0, id->size());
            for (int i = 1; i < N; i++)
            {
                ni = ni->set_slice(ni->size(), ni->size(), id);
            }

            return ni;
        }

        raise_error(incompatible_types_error + what());
    }

    Abstract * Operator<DIV>::eval(Abstract * pe1, Abstract * pe2, Object * o)
    {
        if (! pe2->isNumber())
        {
            raise_error(number_expected_error + pe2->what());
        }

        double n=((Number *) pe2)->value;

        if (pe1->isNumber()) return new Number(((Number *) pe1)->value / n);

        if (pe1->isIndexable())
        {
            int N = (int) n;

            if (N <= 0) return new Nothing;

            Indexable * id = (Indexable *) pe1;
            List * l = new List;

            double last, d = 0;

            for (int i = 0; i < N; i++)
            {
                last = d;
                d = i * id->size();
                d = d / N;
                l->set_element(i, id->get_slice((int) last, (int) d));
            }

            return l;
        }

        raise_error(number_expected_error + what());
    }

    Abstract * Operator<MODULO>::eval(Abstract * pe1, Abstract * pe2, Object * o)
    {
        if (pe1->isNumber() && pe2->isNumber())
        {
            Number * n1=(Number *) pe1;
            Number * n2=(Number *) pe2;
            return new Number(fmod(n1->value, n2->value));
        }

        raise_error(number_expected_error + what());
    }

    Abstract * Operator<ADD>::eval(Abstract * pe1, Abstract * pe2, Object * o)
    {
        if (pe1->isNumber() && pe2->isNumber())
        {
            return new Number(((Number *) pe1)->value + ((Number *) pe2)->value);
        }

        if (pe1->isIndexable())
        {
            Indexable * i1=(Indexable *) pe1;

            if (pe2->isIndexable())
            {
                Indexable * i2=(Indexable *) pe2;

                return i1->set_slice(i1->size(), i1->size(), i2);
            }
            else
            {
                return i1->set_element(i1->size(), pe2);
            }
        }
        else if (pe2->isIndexable())
        {
            Indexable * i2=(Indexable *) pe2;

            return i2->set_element(i2->size(), pe1);
        }

        raise_error(incompatible_types_error + what());
    }

    Abstract * Operator<SUB>::eval(Abstract * pe1, Abstract * pe2, Object * o)
    {
        if (pe1->isNumber() && pe2->isNumber())
        {
            return new Number(((Number *) pe1)->value - ((Number *) pe2)->value);
        }

        if (pe1->isIndexable())
        {
            Indexable * id = (Indexable *) pe1;

            for (int i = 0; i < id->size(); i++)
            {
                if (equal(pe2, id->get_element(i)))
                {
                    return id->set_slice(i, i + 1, new Nothing);
                }
            }

            raise_error(not_found_error + what());
        }

        raise_error(incompatible_types_error + what());
    }

#   define D(OP, CPPOP)                                         \
    Abstract * Operator<OP>::eval(Abstract * pe1,               \
                                  Abstract * pe2,               \
                                  Object * o)                   \
    {                                                           \
        return new Number(pe1->test() CPPOP pe2->test());       \
    }
    D(AND, &&)
    D(OR, ||)
    D(XOR, ^)
#   undef D

    Abstract * Operator<DEF>::eval(Object * o)
    {
        Abstract * pe3=params[2]->eval(o);
        Function * f = new Function(params[1], pe3);
        o->assign_var((Variable *) params[0], f);
        return f;
    }

    Abstract * Operator<SCOPE>::eval(Object * o)
    {
        Object * object=new Object(o);
        params[0]->execute(object);
        return object;
    }

    Abstract * Operator<PARENT>::eval(Object * o)
    {
        if (! o->parent)
            raise_error(at_root_scope_error + what());

        return params[0]->eval(o->parent);
    }

    Abstract * Operator<MEMBER>::eval(Object * o)
    {
        Abstract * pe1=params[0]->eval(o);
        if (! pe1->isObject())
            raise_error(object_expected_error + what());

        return params[1]->eval((Object *) pe1);
    }

    Abstract * Operator<ASSIGN>::eval(Object * o)
    {
        return params[0]->assign(params[1]->eval(o), o);
    }

    Abstract * Operator<WHILE>::eval(Object * o)
    {
        Abstract * r = NULL;
        int lc=o->loops++;

        while (o->loops > 0 && params[0]->eval(o)->test())
        {
            r = params[1]->eval(o);
        }

        o->loops=lc;

        return r ? r : new Nothing;
    }

    Abstract * Operator<DOWHILE>::eval(Object * o)
    {
        Abstract * r;
        int lc=o->loops++;

        do
        {
            r = params[0]->eval(o);
        }
        while (o->loops > 0 && params[1]->eval(o)->test());

        o->loops=lc;

        return r;
    }

    Abstract * Operator<IF>::eval(Object * o)
    {
        if (params[0]->eval(o)->test())
            return params[1]->eval(o);
        else
            return params[2]->eval(o);
    }

    Abstract * Operator<FOREACH>::eval(Object * o)
    {
        Abstract * r = NULL;
        Abstract * pe2=params[1]->eval(o);

        if (! pe2->isIterable())
            raise_error(not_iterable_error + pe2->what());

        Iterable * it=(Iterable *) pe2;
        Iterable::Iterator * i = it->iterator();

        int lc=o->loops++;

        Object transparent(o);

        while (1)
        {
            Abstract * elem = i->next();

            if (elem->isNothing()) break;

            transparent.is_transparent=false;
            params[0]->assign(elem, & transparent);
            transparent.is_transparent=true;

            r = params[2]->eval(& transparent);
        }

        o->loops=lc;

        return r ? r : new Nothing;
    }

    Abstract * Operator<SELECT>::eval(Object * o)
    {
        return params[0]->select(params[1]->eval(o), o);
    }

    Abstract * Operator<SELECT>::assign(Abstract * value, Object * o)
    {
         Abstract * pe1=params[0]->eval(o);
         Abstract * pe2=params[1]->eval(o);

         if (pe1->isIndexable())
         {
             if (! pe2->isNumber())
                 raise_error(number_expected_error + pe2->what());

             int n = (int) ((Number *) pe2)->value;

             return ((Indexable *) pe1)->set_element(n, value);
         }

         if (pe1->isFunction())
             raise_error(assign_error + what());

         raise_error(not_indexable_error + pe1->what());
         return params[0]->assign(value, o);
    }

#   define SLICE_OP(op)                                                          \
        Abstract * pe1=params[0]->eval(o);                                       \
        Abstract * pe2=params[1]->eval(o);                                       \
        Abstract * pe3=params[2]->eval(o);                                       \
                                                                                 \
        if (pe1->isIndexable())                                                  \
        {                                                                        \
            Indexable * i = (Indexable *) pe1;                                   \
                                                                                 \
            if (! pe2->isNumber())                                               \
            {                                                                    \
                raise_error(number_expected_error + pe2->what());  \
            }                                                                    \
            if (! pe3->isNumber())                                               \
            {                                                                    \
                raise_error(number_expected_error + pe3->what());  \
            }                                                                    \
                                                                                 \
            int s = (int) ((Number *) pe2)->value;                               \
            int e = (int) ((Number *) pe3)->value;                               \
                                                                                 \
            op;                                                                  \
        }                                                                        \
        else raise_error(not_indexable_error + pe1->what());

    Abstract * Operator<SLICE>::eval(Object * o)
    {
        SLICE_OP(return i->get_slice(s, e))
    }

    Abstract * Operator<SLICE>::assign(Abstract * value, Object * o)
    {
        SLICE_OP(return i->set_slice(s, e, value))
    }
}
