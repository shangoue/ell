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

#ifndef __KOALANG_OPERATORS__
#define __KOALANG_OPERATORS__
#include "koalang/Types.h"
#include "koalang/Messages.h"
#include "koalang/Defs.h"
namespace koalang
{
    struct BaseOperator : public Abstract
    {
        bool test()
        {
            raise_error(unexpected_error);
        }

        bool greater_or_equal(Abstract * a)
        {
            raise_error(unexpected_error);
        }

        virtual Abstract * select(Abstract * p, Object * o)
        {
            return eval(o)->select(p, o);
        }

        virtual Abstract * assign(Abstract * v, Object * o)
        {
            return eval(o)->assign(v, o);
        }
    };

    struct BaseNullaryOperator : public BaseOperator
    {
        Abstract * params[0];
    };

    struct BaseUnaryOperator : public BaseOperator
    {
        Abstract * eval(Object * o)
        {
            return eval(params[0]->eval(o), o);
        }

        virtual Abstract * eval(Abstract *,
                                Object *) = 0;
        Abstract * params[1];
    };

    struct BaseBinaryOperator : public BaseOperator
    {
        Abstract * eval(Object * o)
        {
            return eval(params[0]->eval(o), params[1]->eval(o), o);
        }

        virtual Abstract * eval(Abstract *,
                                Abstract *,
                                Object *) = 0;
        Abstract * params[2];
    };

    template <const int Id>
    struct Operator
    { };

#   define D(nb_args, id, desc)                           \
    template <>                                           \
    struct Operator<id> : public BaseNullaryOperator      \
    {                                                     \
        const std::string what(bool enclose=true);        \
        Abstract * eval(Object *);                        \
    };
    NULLARY_OPS
#   undef D

#   define D(nb_args, id, desc)                           \
    template <>                                           \
    struct Operator<id> : public BaseUnaryOperator        \
    {                                                     \
        const std::string what(bool enclose=true);        \
        Abstract * eval(Abstract *,                       \
                        Object *);                        \
    };
    UNARY_OPS
#   undef D

#   define D(nb_args, id, desc)                           \
    template <>                                           \
    struct Operator<id> : public BaseBinaryOperator       \
    {                                                     \
        const std::string what(bool enclose=true);        \
        Abstract * eval(Abstract *,                       \
                        Abstract *,                       \
                        Object *);                        \
    };
    BINARY_OPS
#   undef D

#   define D(nb_args, id, desc)                           \
    template <>                                           \
    struct Operator<id> : public BaseOperator             \
    {                                                     \
        const std::string what(bool enclose=true);        \
        Abstract * eval(Object *);                        \
                                                          \
        Abstract * params[nb_args];                       \
    };
    SPECIAL_EVAL_OPS
#   undef D

#   define D(nb_args, id, desc)                           \
    template <>                                           \
    struct Operator<id> : public BaseOperator             \
    {                                                     \
        const std::string what(bool enclose=true);        \
        Abstract * eval(Object *);                        \
        Abstract * assign(Abstract *, Object *);          \
                                                          \
        Abstract * params[nb_args];                       \
    };
    ASSIGNABLE_OPS
#   undef D
}
#endif // __KOALANG_OPERATORS__
