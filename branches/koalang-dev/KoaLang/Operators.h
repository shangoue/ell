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

#ifndef __KOALANG_OPERATORS_H__
#define __KOALANG_OPERATORS_H__

#include "Types.h"

namespace koalang
{
    struct Operator : public Object
    {
        std::string name;
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
        Object * eval(Map * context)
        {
            if (left->is<Variable>())
            {
                return context->value[left->to<Variable>().value] = right->eval(context);
            }
    };

    struct Call : public Assign
    {
        void describe(std::ostream & os) const
        {
            os << '(' << * left << ' ' << name << ' ' << * right << ')';
        }

        Object * eval(Map * context)
        {
            Map parameters(context);
            Assign a;
            a.left = function->left;
            a.right = left;
            a.eval(& parameters);
            a.left = function->right;
            a.right = right;
            a.eval(& parameters);
            return function->body->eval(parameters);
        }

        List * left, * right;
        Function * function;
    };


    struct Add : public BinaryOperator
    {
        Object * eval(Map * context)
        {
            return new Real(left->eval(context)->to<Real>() + right->eval(context)->to<Real>());
        }
    };

    struct Print : public UnaryOperator
    {
        Object * eval(Map * context)
        {
            std::cout << * (target->eval(context)) << std::endl;
            return new List;
        }
    };
}

#endif //__KOALANG_OPERATORS_H__
