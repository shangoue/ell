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
    struct Repeat;

    template <typename Token, typename Child, typename ConcreteParser, typename Var, typename Value>
    struct Action;

    template <typename Token, typename Left, typename Right>
    struct Alternative;

    template <typename Token, typename Left, typename Right>
    struct Difference;

    template <typename Token, typename Left, typename Right>
    struct Aggregation;

    template <typename Token, typename Left, typename Right>
    struct List;

    template <typename Token, typename Left, typename Right>
    struct BoundRepetition;

    template <typename Token, typename Left, typename Right>
    struct Association;

    template <typename Token, typename Left, typename Right>
    struct NoSuffix;

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
            sr.describe(os);
            return os;
        }

#       if ELL_DEBUG == 1
        virtual bool must_be_dumped() const { return ELL_DUMP_NODES; }
#       endif

        void get_value() { }

        virtual void describe(std::ostream & os) const = 0;

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
        Action<Token, ConcreteNode, ConcreteParser, V ConcreteParser::*, V> operator [ ] (V ConcreteParser::*var) const;

        template <typename ConcreteParser, typename R>
        Action<Token, ConcreteNode, ConcreteParser, R (ConcreteParser::*)(), void> operator [ ] (R (ConcreteParser::*meth)()) const;

        template <typename ConcreteParser, typename R, typename V>
        Action<Token, ConcreteNode, ConcreteParser, R (ConcreteParser::*)(V), V> operator [ ] (R (ConcreteParser::*meth)(V)) const;

        template <typename ConcreteParser, typename R, typename V>
        Action<Token, ConcreteNode, ConcreteParser, R (ConcreteParser::*)(const V &), V> operator [ ] (R (ConcreteParser::*meth)(const V &)) const;
        //@}

        //@{
        /// Grammar unary Operator
        Repeat<Token, ConcreteNode, 0, 1> operator ! () const;

        Repeat<Token, ConcreteNode, 1, -1> operator + () const;

        Repeat<Token, ConcreteNode, 0, -1> operator * () const;
        //@}

        //@{
        /// Grammar binary Operator
        template <typename Right>
        Alternative<Token, ConcreteNode, Right> operator | (const Right & r) const;

        template <typename Right>
        Difference<Token, ConcreteNode, Right> operator - (const Right & r) const;

        template <typename Right>
        Aggregation<Token, ConcreteNode, Right> operator >> (const Right & r) const;

        template <typename Right>
        List<Token, ConcreteNode, Right> operator % (const Right & r) const;

        template <typename Right>
        BoundRepetition<Token, ConcreteNode, Right> operator * (const Right & r) const;

        template <typename Right>
        Association<Token, ConcreteNode, Right> operator & (const Right & r) const;
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
    Action<Token, CN, CP, V CP::*, V> ConcreteNodeBase<Token, CN>::operator [ ](V CP::*var) const
    {
        return Action<Token, CN, CP, V CP::*, V>(* (CN *) this, var);
    }

    template <typename Token, typename CN>
    template <typename CP, typename R>
    Action<Token, CN, CP, R (CP::*)(), void> ConcreteNodeBase<Token, CN>::operator [ ](R (CP::*meth)()) const
    {
        return Action<Token, CN, CP, R (CP::*)(), void>(* (CN *) this, meth);
    }

    template <typename Token, typename CN>
    template <typename CP, typename R, typename V>
    Action<Token, CN, CP, R (CP::*)(V), V> ConcreteNodeBase<Token, CN>::operator [ ](R (CP::*meth)(V)) const
    {
        return Action<Token, CN, CP, R (CP::*)(V), V>(* (CN *) this, meth);
    }

    template <typename Token, typename CN>
    template <typename CP, typename R, typename V>
    Action<Token, CN, CP, R (CP::*)(const V &), V> ConcreteNodeBase<Token, CN>::operator [ ](R (CP::*meth)(const V &)) const
    {
        return Action<Token, CN, CP, R (CP::*)(const V &), V>(* (CN *) this, meth);
    }

    template <typename Token, typename CN>
    Repeat<Token, CN, 0, 1> ConcreteNodeBase<Token, CN>::operator ! () const
    {
        return Repeat<Token, CN, 0, 1>(* (CN *) this);
    }

    template <typename Token, typename CN>
    Repeat<Token, CN, 1, -1> ConcreteNodeBase<Token, CN>::operator + () const
    {
        return Repeat<Token, CN, 1, -1>(* (CN *) this);
    }

    template <typename Token, typename CN>
    Repeat<Token, CN, 0, -1> ConcreteNodeBase<Token, CN>::operator * () const
    {
        return Repeat<Token, CN, 0, -1>(* (CN *) this);
    }

    template <typename Token, typename CN>
    template <typename Right>
    Alternative<Token, CN, Right> ConcreteNodeBase<Token, CN>::operator | (const Right & r) const
    {
        return Alternative<Token, CN, Right>(* (CN *) this, r);
    }

    template <typename Token, typename CN>
    template <typename Right>
    Difference<Token, CN, Right> ConcreteNodeBase<Token, CN>::operator - (const Right & r) const
    {
        return Difference<Token, CN, Right>(* (CN *) this, r);
    }

    template <typename Token, typename CN>
    template <typename Right>
    Aggregation<Token, CN, Right> ConcreteNodeBase<Token, CN>::operator >> (const Right & r) const
    {
        return Aggregation<Token, CN, Right>(* (CN *) this, r);
    }

    template <typename Token, typename CN>
    template <typename Right>
    List<Token, CN, Right> ConcreteNodeBase<Token, CN>::operator % (const Right & r) const
    {
        return List<Token, CN, Right>(* (CN *) this, r);
    }

    template <typename Token, typename CN>
    template <typename Right>
    BoundRepetition<Token, CN, Right> ConcreteNodeBase<Token, CN>::operator * (const Right & r) const
    {
        return BoundRepetition<Token, CN, Right>(* (CN *) this, r);
    }

    template <typename Token, typename CN>
    template <typename Right>
    Association<Token, CN, Right> ConcreteNodeBase<Token, CN>::operator & (const Right & r) const
    {
        return Association<Token, CN, Right>(* (CN *) this, r);
    }
}

#endif // INCLUDED_ELL_NODE_H
