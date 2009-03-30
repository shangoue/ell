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

#ifndef INCLUDED_PARSER_UNARY_NODE_H
#define INCLUDED_PARSER_UNARY_NODE_H

#include <ell/Rule.h>

namespace ell
{
    template <typename Token, typename ConcreteNode, typename UnaryFlavour>
    struct UnaryNodeBase : public ConcreteNodeBase<Token, ConcreteNode>
    {
        void describe(std::ostream & os) const
        {
#           if PARSER_DEBUG == 1
            if (Node<Token>::must_be_dumped())
                os << Node<Token>::name << '(' << ((UnaryFlavour *) this)->target << ')';
            else
#           endif
                os << ((UnaryFlavour *) this)->target;
        }
    };

    template <typename Token, typename ConcreteNode, typename Child>
    struct UnaryNode : public UnaryNodeBase<Token, ConcreteNode, UnaryNode<Token, ConcreteNode, Child> >
    {
        UnaryNode(const Child & child, const char * n = 0)
          : target(child)
        {
            Node<Token>::name = n;
        }

        Child target;
    };

    template <typename Token, typename ConcreteNode>
    struct UnaryNode<Token, ConcreteNode, Rule<Token> >
      : public UnaryNodeBase<Token, ConcreteNode, UnaryNode<Token, ConcreteNode, Rule<Token> > >
    {
        UnaryNode(const Rule<Token> & child, const char * n = 0)
          : target(child)
        {
            Node<Token>::name = n;
        }

        const Rule<Token> & target;
    };
}

#endif // INCLUDED_PARSER_UNARY_NODE_H
