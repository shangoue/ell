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

#ifndef INCLUDED_PARSER_RULE_H
#define INCLUDED_PARSER_RULE_H

#include <ell/Node.h>

namespace ell
{
    /// A rule allows to introduce recursive definitions in a grammar as
    /// it breaks the expression templates chain.
    template <typename Token>
    struct Rule : public ConcreteNodeBase<Token, Rule<Token> >
    {
        using Node<Token>::name;
        using ConcreteNodeBase<Token, Rule<Token> >::parse;

        // Default name to anonymous to avoid infinite recursion in describe
        // when no names has been given
        Rule()
          : top(0)
        {
            Node<Token>::name = "<anonymous>";
        }

        virtual ~Rule() { clean(); }

        void clean()
        {
            if (top and not dynamic_cast<const Rule *>(top))
                delete top;
        }

        template <typename N>
        void operator = (const N & n)
        {
            clean();
            if (dynamic_cast<const Rule *>(& n))
                top = & n;
            else
                top = new N(n);
        }

        void set_name(const char * n) { name = n; }

        bool parse(Parser<Token> * parser) const
        {
            ELL_BEGIN_PARSE
            match = top->parse(parser);
            ELL_END_PARSE
        }

        void describe(std::ostream & os) const
        {
            if (name)
                os << name;
            else
                os << * top;
        }

#       if ELL_DEBUG == 1
        bool must_be_dumped() const { return true; }
#       endif

    private:
        const Node<Token> * top;
    };
}

#endif // INCLUDED_PARSER_RULE_H
