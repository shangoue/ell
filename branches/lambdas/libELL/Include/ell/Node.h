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

    template <typename Token, typename Child, typename Action>
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
    struct Node;

    /// Grammar node base class
    template <typename Token>
    struct Node
    {
        virtual ~Node() { }

        /// Function called recursively to perform the parsing.
        /// Return true if and only if it matches.
        /// Each node is responsible for restoring the parser state as untouched,
        /// unless it has matched.
        virtual bool parse(Parser<Token> *) const = 0;

        virtual std::string get_kind() const = 0;
        virtual const Node<Token> * get_child_at(int /*index*/) const { return 0; }
        virtual std::string get_value() const { return ""; }
    };

    template <typename Token, typename ConcreteNode>
    struct ConcreteNodeBase : public Node<Token>
    {
        bool parse(Parser<Token> * parser) const
        {
            const ConcreteNode * instance = ((const ConcreteNode *) this);
            Storage<void> s;
            return instance->match(parser, s);
        }

        bool match(Parser<Token> * parser, Storage<std::basic_string<Token> > & s) const
        {
            const Token * begin = parser->position;
            bool res = parse(parser);
            s.value.assign(begin, parser->position);
            return res;
        }

        bool match(Parser<Token> * parser, Storage<ell::basic_string<Token> > & s) const
        {
            s.value.position = parser->position;
            bool res = parse(parser);
            s.value.size_ = parser->position - s.value.position;
            return res;
        }

        bool match(Parser<Token> * parser, Storage<const Token *> & s) const
        {
            s.value = & * parser->position;
            return parse(parser);
        }

        bool match(Parser<Token> * parser, Storage<bool> & s) const
        {
            const Token * begin = parser->position;
            bool res = parse(parser);
            s.value = (match & (parser->position != begin));
            return res;
        }

        //@{
        /// Semantic actions
        template <typename Action>
        Act<Token, ConcreteNode, Action> operator [ ] (Action var) const;
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
#include <ell/Dump.h>

namespace ell
{
    template <typename Token, typename CN>
    template <typename Action>
    Act<Token, CN, Action> ConcreteNodeBase<Token, CN>::operator [ ](Action act) const
    {
        return Act<Token, CN, Action>(* (const CN *) this, act);
    }

    template <typename Token, typename CN>
    Rp<Token, CN, 0, 1> ConcreteNodeBase<Token, CN>::operator ! () const
    {
        return Rp<Token, CN, 0, 1>(* (const CN *) this);
    }

    template <typename Token, typename CN>
    Rp<Token, CN, 1, -1> ConcreteNodeBase<Token, CN>::operator + () const
    {
        return Rp<Token, CN, 1, -1>(* (const CN *) this);
    }

    template <typename Token, typename CN>
    Rp<Token, CN, 0, -1> ConcreteNodeBase<Token, CN>::operator * () const
    {
        return Rp<Token, CN, 0, -1>(* (const CN *) this);
    }

    template <typename Token, typename CN>
    template <typename Right>
    Alt<Token, CN, Right> ConcreteNodeBase<Token, CN>::operator | (const Right & r) const
    {
        return Alt<Token, CN, Right>(* (const CN *) this, r);
    }

    template <typename Token, typename CN>
    template <typename Right>
    LAl<Token, CN, Right> ConcreteNodeBase<Token, CN>::operator || (const Right & r) const
    {
        return LAl<Token, CN, Right>(* (const CN *) this, r);
    }

    template <typename Token, typename CN>
    template <typename Right>
    Dif<Token, CN, Right> ConcreteNodeBase<Token, CN>::operator - (const Right & r) const
    {
        return Dif<Token, CN, Right>(* (const CN *) this, r);
    }

    template <typename Token, typename CN>
    template <typename Right>
    Agg<Token, CN, Right> ConcreteNodeBase<Token, CN>::operator >> (const Right & r) const
    {
        return Agg<Token, CN, Right>(* (const CN *) this, r);
    }

    template <typename Token, typename CN>
    template <typename Right>
    Lst<Token, CN, Right> ConcreteNodeBase<Token, CN>::operator % (const Right & r) const
    {
        return Lst<Token, CN, Right>(* (const CN *) this, r);
    }

    template <typename Token, typename CN>
    template <typename Right>
    BRp<Token, CN, Right> ConcreteNodeBase<Token, CN>::operator * (const Right & r) const
    {
        return BRp<Token, CN, Right>(* (const CN *) this, r);
    }

    template <typename Token, typename CN>
    template <typename Right>
    Asc<Token, CN, Right> ConcreteNodeBase<Token, CN>::operator & (const Right & r) const
    {
        return Asc<Token, CN, Right>(* (const CN *) this, r);
    }
}

#endif // INCLUDED_ELL_NODE_H
