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

#ifndef INCLUDED_ELL_NODE_H
#define INCLUDED_ELL_NODE_H

#include <ell/Utils.h>
#include <ell/Storage.h>

namespace ell
{
    template <typename Token>
    struct Parser;

    template <typename Token, typename Child, const int Min, const int Max>
    struct Rp;

    template <typename Token, typename Child, typename ConcreteParser, typename Var, typename Value>
    struct Act;

    template <typename Token, typename Left, typename Right>
    struct Alt;

    template <typename Token, typename Left, typename Right>
    struct Dif;

    template <typename Token, typename Left, typename Right>
    struct Agg;

    template <typename Token, typename Left, typename Right>
    struct Lst;

    template <typename Token, typename Left, typename Right>
    struct BRp;

    template <typename Token, typename Left, typename Right>
    struct Asc;

    template <typename Token, typename Left, typename Right>
    struct NSx;

    template <typename Token, typename Left, typename Right>
    struct LAl;

    template <typename Token>
    struct Node
    {
        Node(const std::string & name = "")
          : name(name)
        { }

        virtual ~Node() { }

        virtual bool parse(Parser<Token> *) const = 0;

        friend inline std::ostream & operator << (std::ostream & os, const Node & sr)
        {
            os << sr.describe(false);
            return os;
        }

#       if ELL_DEBUG == 1
        virtual bool must_be_dumped() const { return ELL_DUMP_NODES; }
#       endif

        void get_value() { }

        virtual std::string describe(bool need_parens) const = 0;

        const std::string & get_name() const { return name; }

		std::string name;
    };

    template <typename Token, typename ConcreteNode>
    struct ConcreteNodeBase : public Node<Token>
    {
        bool parse(Parser<Token> * parser) const
        {
            Storage<void> s;
            return ((ConcreteNode *) this)->parse(parser, s);
        }

        bool parse(Parser<Token> * parser, Storage<std::basic_string<Token> > & s) const
        {
            const Token * begin = parser->position;
            bool match = parse(parser);
            s.value.assign(begin, parser->position);
            return match;
        }

        bool parse(Parser<Token> * parser, Storage<const Token *> & s) const
        {
            s.value = & * parser->position;
            return parse(parser);
        }

        //@{
        /// Semantic actions
        template <typename ConcreteParser, typename V>
        Act<Token, ConcreteNode, ConcreteParser, V ConcreteParser::*, V> operator [ ] (V ConcreteParser::*var) const;

        template <typename ConcreteParser, typename R>
        Act<Token, ConcreteNode, ConcreteParser, R (ConcreteParser::*)(), void> operator [ ] (R (ConcreteParser::*meth)()) const;

        template <typename ConcreteParser, typename R, typename V>
        Act<Token, ConcreteNode, ConcreteParser, R (ConcreteParser::*)(V), V> operator [ ] (R (ConcreteParser::*meth)(V)) const;

        template <typename ConcreteParser, typename R, typename V>
        Act<Token, ConcreteNode, ConcreteParser, R (ConcreteParser::*)(const V &), V> operator [ ] (R (ConcreteParser::*meth)(const V &)) const;
        //@}

        //@{
        /// Grammar unary Operator
        Rp<Token, ConcreteNode, 0, 1> operator ! () const;

        Rp<Token, ConcreteNode, 1, -1> operator + () const;

        Rp<Token, ConcreteNode, 0, -1> operator * () const;
        //@}

        //@{
        /// Grammar binary Operator
        template <typename Right>
        Alt<Token, ConcreteNode, Right> operator | (const Right & r) const;

        template <typename Right>
        LAl<Token, ConcreteNode, Right> operator || (const Right & r) const;

        template <typename Right>
        Dif<Token, ConcreteNode, Right> operator - (const Right & r) const;

        template <typename Right>
        Agg<Token, ConcreteNode, Right> operator >> (const Right & r) const;

        template <typename Right>
        Lst<Token, ConcreteNode, Right> operator % (const Right & r) const;

        template <typename Right>
        BRp<Token, ConcreteNode, Right> operator * (const Right & r) const;

        template <typename Right>
        Asc<Token, ConcreteNode, Right> operator & (const Right & r) const;
        //@}
    };
}

#include <ell/UnaryNodes.h>
#include <ell/BinaryNodes.h>
#include <ell/Primitives.h>
#include <ell/Numerics.h>

namespace ell
{
    template <typename Token, typename CN>
    template <typename CP, typename V>
    Act<Token, CN, CP, V CP::*, V> ConcreteNodeBase<Token, CN>::operator [ ](V CP::*var) const
    {
        return Act<Token, CN, CP, V CP::*, V>(* (CN *) this, var);
    }

    template <typename Token, typename CN>
    template <typename CP, typename R>
    Act<Token, CN, CP, R (CP::*)(), void> ConcreteNodeBase<Token, CN>::operator [ ](R (CP::*meth)()) const
    {
        return Act<Token, CN, CP, R (CP::*)(), void>(* (CN *) this, meth);
    }

    template <typename Token, typename CN>
    template <typename CP, typename R, typename V>
    Act<Token, CN, CP, R (CP::*)(V), V> ConcreteNodeBase<Token, CN>::operator [ ](R (CP::*meth)(V)) const
    {
        return Act<Token, CN, CP, R (CP::*)(V), V>(* (CN *) this, meth);
    }

    template <typename Token, typename CN>
    template <typename CP, typename R, typename V>
    Act<Token, CN, CP, R (CP::*)(const V &), V> ConcreteNodeBase<Token, CN>::operator [ ](R (CP::*meth)(const V &)) const
    {
        return Act<Token, CN, CP, R (CP::*)(const V &), V>(* (CN *) this, meth);
    }

    template <typename Token, typename CN>
    Rp<Token, CN, 0, 1> ConcreteNodeBase<Token, CN>::operator ! () const
    {
        return Rp<Token, CN, 0, 1>(* (CN *) this);
    }

    template <typename Token, typename CN>
    Rp<Token, CN, 1, -1> ConcreteNodeBase<Token, CN>::operator + () const
    {
        return Rp<Token, CN, 1, -1>(* (CN *) this);
    }

    template <typename Token, typename CN>
    Rp<Token, CN, 0, -1> ConcreteNodeBase<Token, CN>::operator * () const
    {
        return Rp<Token, CN, 0, -1>(* (CN *) this);
    }

    template <typename Token, typename CN>
    template <typename Right>
    Alt<Token, CN, Right> ConcreteNodeBase<Token, CN>::operator | (const Right & r) const
    {
        return Alt<Token, CN, Right>(* (CN *) this, r);
    }

    template <typename Token, typename CN>
    template <typename Right>
    LAl<Token, CN, Right> ConcreteNodeBase<Token, CN>::operator || (const Right & r) const
    {
        return LAl<Token, CN, Right>(* (CN *) this, r);
    }

    template <typename Token, typename CN>
    template <typename Right>
    Dif<Token, CN, Right> ConcreteNodeBase<Token, CN>::operator - (const Right & r) const
    {
        return Dif<Token, CN, Right>(* (CN *) this, r);
    }

    template <typename Token, typename CN>
    template <typename Right>
    Agg<Token, CN, Right> ConcreteNodeBase<Token, CN>::operator >> (const Right & r) const
    {
        return Agg<Token, CN, Right>(* (CN *) this, r);
    }

    template <typename Token, typename CN>
    template <typename Right>
    Lst<Token, CN, Right> ConcreteNodeBase<Token, CN>::operator % (const Right & r) const
    {
        return Lst<Token, CN, Right>(* (CN *) this, r);
    }

    template <typename Token, typename CN>
    template <typename Right>
    BRp<Token, CN, Right> ConcreteNodeBase<Token, CN>::operator * (const Right & r) const
    {
        return BRp<Token, CN, Right>(* (CN *) this, r);
    }

    template <typename Token, typename CN>
    template <typename Right>
    Asc<Token, CN, Right> ConcreteNodeBase<Token, CN>::operator & (const Right & r) const
    {
        return Asc<Token, CN, Right>(* (CN *) this, r);
    }
}

#endif // INCLUDED_ELL_NODE_H