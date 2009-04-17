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

#ifndef INCLUDED_ELL_BINARY_NODES_H
#define INCLUDED_ELL_BINARY_NODES_H

#include <ell/BinaryNode.h>
#include <ell/Parser.h>

namespace ell
{
    template <typename Token, typename Left, typename Right>
    struct Alternative : public BinaryNode<Token, Alternative<Token, Left, Right>, Left, Right>
    {
        typedef BinaryNode<Token, Alternative<Token, Left, Right>, Left, Right> Base;
        using Base::right;
        using Base::left;

        Alternative(const Left & left, const Right & right)
          : Base(left, right, "or")
        { }

        using Base::parse;

        template <typename V>
        bool parse(Parser<Token> * parser, Storage<V> & s) const
        {
            ELL_BEGIN_PARSE
            match = left.parse(parser, s) or right.parse(parser, s);
            ELL_END_PARSE
        }
    };

    template <typename Token, typename Left, typename Right>
    struct Association : public BinaryNode<Token, Association<Token, Left, Right>, Left, Right>
    {
        typedef BinaryNode<Token, Association<Token, Left, Right>, Left, Right> Base;
        using Base::right;
        using Base::left;

        Association(const Left & left, const Right & right)
          : Base(left, right, "followed or preceded by")
        { }

        using Base::parse;
        template <typename T>
        bool parse(Parser<Token> * parser, Storage<T> & s) const
        {
            ELL_BEGIN_PARSE
            s.clear();
            typename Storage<T>::Unit se;
            if (left.parse(parser, se))
            {
                s.enqueue(se);
                if (right.parse(parser, se))
                {
                    s.enqueue(se);
                    match = true;
                }
            }
            else if (right.parse(parser, se))
            {
                s.enqueue(se);
                if (left.parse(parser, se))
                {
                    s.enqueue(se);
                    match = true;
                }
            }
            ELL_END_PARSE
        }
    };

    template <typename Token, typename Left, typename Right>
    struct Aggregation : public BinaryNode<Token, Aggregation<Token, Left, Right>, Left, Right>
    {
        typedef BinaryNode<Token, Aggregation<Token, Left, Right>, Left, Right> Base;
        using Base::right;
        using Base::left;

        Aggregation(const Left & left, const Right & right)
          : Base(left, right, 0)
        { }

        using Base::parse;

        template <typename V>
        bool parse(Parser<Token> * parser, Storage<V> & s) const
        {
            ELL_BEGIN_PARSE
            typename Parser<Token>::Context sav_pos(parser);

            typename Storage<V>::Unit s1;
            if (left.parse(parser, s1))
            {
                parser->skip();
                typename Storage<V>::Unit s2;
                if (right.parse(parser, s2))
                {
                    s.enqueue(s1);
                    s.enqueue(s2);
                    match=true;
                }
                else
                {
                    if (not parser->flags.look_ahead)
                        parser->mismatch(right);

                    sav_pos.restore(parser);
                }
            }
            else
                sav_pos.restore(parser);
            ELL_END_PARSE
        }
    };

    template <typename Token, typename Left, typename Right>
    struct Difference : public BinaryNode<Token, Difference<Token, Left, Right>, Left, Right>
    {
        typedef BinaryNode<Token, Difference<Token, Left, Right>, Left, Right> Base;
        using Base::right;
        using Base::left;

        Difference(const Left & left, const Right & right)
          : Base(left, right, "but not")
        { }

        using Base::parse;

        template <typename V>
        bool parse(Parser<Token> * parser, Storage<V> & s) const
        {
            ELL_BEGIN_PARSE
            typename Parser<Token>::Context sav_pos(parser);
            if (right.parse(parser))
                sav_pos.restore(parser);
            else
                match = left.parse(parser, s);
            ELL_END_PARSE
        }
    };

    template <typename Token, typename Left, typename Right>
    struct List : public BinaryNode<Token, List<Token, Left, Right>, Left, Right>
    {
        typedef BinaryNode<Token, List<Token, Left, Right>, Left, Right> Base;
        using Base::right;
        using Base::left;

        List(const Left & left, const Right & right)
          : Base(left, right, "separated by")
        { }

        using Base::parse;

        template <typename T>
        bool parse(Parser<Token> * parser, Storage<T> & s) const
        {
            ELL_BEGIN_PARSE
            typename Parser<Token>::Context sav_pos(parser);
            typename Storage<T>::Unit se;
            s.clear();

            while (left.parse(parser, se))
            {
                s.enqueue(se);
                match = true;
                parser->skip();
                sav_pos = typename Parser<Token>::Context(parser);

                if (! right.parse(parser))
                    break;

                match = false;
                parser->skip();
            }

            sav_pos.restore(parser);
            ELL_END_PARSE
        }
    };

    template <typename Token, typename Left, typename Right>
    struct BoundRepetition : public BinaryNode<Token, BoundRepetition<Token, Left, Right>, Left, Right>
    {
        typedef BinaryNode<Token, BoundRepetition<Token, Left, Right>, Left, Right> Base;
        using Base::right;
        using Base::left;

        BoundRepetition(const Left & left, const Right & right)
          : Base(left, right, "until")
        { }

        using Base::parse;
        template <typename T>
        bool parse(Parser<Token> * parser, Storage<T> & s) const
        {
            ELL_BEGIN_PARSE
            s.clear();
            typename Storage<T>::Unit se;
            while (1)
            {
                match = right.parse(parser);
                if (match or not left.parse(parser, se))
                    break;
                s.enqueue(se);
                parser->skip();
            }
            ELL_END_PARSE
        }
    };

    template <typename Token, typename Left, typename Right>
    struct NoSuffix : public BinaryNode<Token, NoSuffix<Token, Left, Right>, Left, Right>
    {
        typedef BinaryNode<Token, NoSuffix<Token, Left, Right>, Left, Right> Base;
        using Base::right;
        using Base::left;

        NoSuffix(const Left & left, const Right & right)
          : Base(left, right, "not followed by")
        { }

        using Base::parse;
        template <typename T>
        bool parse(Parser<Token> * parser, Storage<T> & s) const
        {
            ELL_BEGIN_PARSE
            SafeModify<> m1(parser->flags.look_ahead, true);
            typename Parser<Token>::Context sav_pos(parser);
            match = left.parse(parser, s);
            if (match and right.parse(parser))
            {
                sav_pos.restore(parser);
                match = false;
            }
            ELL_END_PARSE
        }
    };
}

#endif // INCLUDED_ELL_BINARY_NODES_H
