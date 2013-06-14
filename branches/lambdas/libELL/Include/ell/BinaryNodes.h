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
    /// Alternative, left first match
    template <typename Token, typename Left, typename Right>
    struct Alt : public BinaryNode<Token, Alt<Token, Left, Right>, Left, Right>
    {
        typedef BinaryNode<Token, Alt<Token, Left, Right>, Left, Right> Base;
        using Base::right;
        using Base::left;

        Alt(const Left & left, const Right & right)
          : Base(left, right)
        { }

        std::string get_kind() const { return "alternative"; }

        using Base::match;

        template <typename V>
        bool match(Parser<Token> * parser, Storage<V> & s) const
        {
            ELL_BEGIN_PARSE
            res = left.match(parser, s) || right.match(parser, s);
            ELL_END_PARSE
        }
    };

    /// Longest alternative
    template <typename Token, typename Left, typename Right>
    struct LAl : public BinaryNode<Token, LAl<Token, Left, Right>, Left, Right>
    {
        typedef BinaryNode<Token, LAl<Token, Left, Right>, Left, Right> Base;
        using Base::right;
        using Base::left;

        LAl(const Left & left, const Right & right)
          : Base(left, right)
        { }
         
        std::string get_kind() const { return "longest"; }

        using Base::match;

        template <typename V>
        bool match(Parser<Token> * parser, Storage<V> & s) const
        {
            ELL_BEGIN_PARSE
            typename Parser<Token>::Context sav_pos(parser);

            size_t left_s = 0;
            size_t right_s = 0;
            // Parse a first time to find length
            {
              SafeModify<> no_actions(parser->flags.action, false);

              if (left.match(parser, s))
                  left_s = parser->measure(sav_pos);
              sav_pos.restore(parser);

              if (right.match(parser, s))
                  right_s = parser->measure(sav_pos);
              sav_pos.restore(parser);
            }

            if ((left_s != 0) | (right_s != 0))
            {
                // Reparse the right one with actions
                if (left_s >= right_s)
                    res = left.match(parser, s);
                else
                    res = right.match(parser, s);
            }
            ELL_END_PARSE
        }
    };

    /// Combination (equivalent to a >> b | b >> a)
    template <typename Token, typename Left, typename Right>
    struct Asc : public BinaryNode<Token, Asc<Token, Left, Right>, Left, Right>
    {
        typedef BinaryNode<Token, Asc<Token, Left, Right>, Left, Right> Base;
        using Base::right;
        using Base::left;

        Asc(const Left & left, const Right & right)
          : Base(left, right)
        { }

        std::string get_kind() const { return "combination"; }

        using Base::match;
        template <typename T>
        bool match(Parser<Token> * parser, Storage<T> & s) const
        {
            ELL_BEGIN_PARSE
            s.clear();
            typename Storage<T>::Unit se;
            if (left.match(parser, se))
            {
                s.enqueue(se);
                if (right.match(parser, se))
                {
                    s.enqueue(se);
                    res = true;
                }
            }
            else if (right.match(parser, se))
            {
                s.enqueue(se);
                if (left.match(parser, se))
                {
                    s.enqueue(se);
                    res = true;
                }
            }
            ELL_END_PARSE
        }
    };

    /// Aggregation (sequence)
    template <typename Token, typename Left, typename Right>
    struct Agg : public BinaryNode<Token, Agg<Token, Left, Right>, Left, Right>
    {
        typedef BinaryNode<Token, Agg<Token, Left, Right>, Left, Right> Base;
        using Base::right;
        using Base::left;

        Agg(const Left & left, const Right & right)
          : Base(left, right)
        { }

        std::string get_kind() const { return "aggregation"; }

        using Base::match;

        template <typename V>
        bool match(Parser<Token> * parser, Storage<V> & s) const
        {
            ELL_BEGIN_PARSE
            typename Parser<Token>::Context sav_pos(parser);

            typename Storage<V>::Unit s1;
            if (left.match(parser, s1))
            {
                parser->skip();
                typename Storage<V>::Unit s2;
                if (right.match(parser, s2))
                {
                    s.enqueue(s1);
                    s.enqueue(s2);
                    res=true;
                }
                else
                {
                    if (! parser->flags.look_ahead)
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
    struct Dif : public BinaryNode<Token, Dif<Token, Left, Right>, Left, Right>
    {
        typedef BinaryNode<Token, Dif<Token, Left, Right>, Left, Right> Base;
        using Base::right;
        using Base::left;

        Dif(const Left & left, const Right & right)
          : Base(left, right)
        { }

        std::string get_kind() const { return "exclusion"; }

        using Base::match;

        template <typename V>
        bool match(Parser<Token> * parser, Storage<V> & s) const
        {
            ELL_BEGIN_PARSE
            typename Parser<Token>::Context sav_pos(parser);
            if (right.parse(parser))
                sav_pos.restore(parser);
            else
                res = left.match(parser, s);
            ELL_END_PARSE
        }
    };

    template <typename Token, typename Left, typename Right>
    struct Lst : public BinaryNode<Token, Lst<Token, Left, Right>, Left, Right>
    {
        typedef BinaryNode<Token, Lst<Token, Left, Right>, Left, Right> Base;
        using Base::right;
        using Base::left;

        Lst(const Left & left, const Right & right)
          : Base(left, right)
        { }

        std::string get_kind() const { return "list"; }

        using Base::match;

        template <typename T>
        bool match(Parser<Token> * parser, Storage<T> & s) const
        {
            ELL_BEGIN_PARSE
            typename Parser<Token>::Context sav_pos(parser);
            typename Storage<T>::Unit se;
            s.clear();

            while (left.match(parser, se))
            {
                s.enqueue(se);
                res = true;
                parser->skip();
                sav_pos = typename Parser<Token>::Context(parser);

                if (! right.parse(parser))
                    break;

                parser->skip();
            }

            sav_pos.restore(parser);
            ELL_END_PARSE
        }
    };

    /// Bound repetition, equivalent to `* (left - right) >> right`
    template <typename Token, typename Left, typename Right>
    struct BRp : public BinaryNode<Token, BRp<Token, Left, Right>, Left, Right>
    {
        typedef BinaryNode<Token, BRp<Token, Left, Right>, Left, Right> Base;
        using Base::right;
        using Base::left;

        BRp(const Left & left, const Right & right)
          : Base(left, right)
        { }

        std::string get_kind() const { return "bound-repetition"; }

        using Base::match;
        template <typename T>
        bool match(Parser<Token> * parser, Storage<T> & s) const
        {
            ELL_BEGIN_PARSE
            s.clear();
            typename Storage<T>::Unit se;
            while (1)
            {
                res = right.parse(parser);
                if (res || ! left.match(parser, se))
                    break;
                s.enqueue(se);
                parser->skip();
            }
            ELL_END_PARSE
        }
    };

    /// No suffix
    template <typename Token, typename Left, typename Right>
    struct NSx : public BinaryNode<Token, NSx<Token, Left, Right>, Left, Right>
    {
        typedef BinaryNode<Token, NSx<Token, Left, Right>, Left, Right> Base;
        using Base::right;
        using Base::left;

        NSx(const Left & left, const Right & right)
          : Base(left, right)
        { }

        std::string get_kind() const { return "no-suffix"; }

        using Base::match;
        template <typename T>
        bool match(Parser<Token> * parser, Storage<T> & s) const
        {
            ELL_BEGIN_PARSE
            SafeModify<> m1(parser->flags.look_ahead, true);
            typename Parser<Token>::Context sav_pos(parser);
            res = left.match(parser, s);
            if (res && right.parse(parser))
            {
                sav_pos.restore(parser);
                res = false;
            }
            ELL_END_PARSE
        }
    };

    /// Skipper enabling directive, like the skip() directive, but taking the skipper to use as a second parameter.
    template <typename Token, typename Left, typename Right>
    struct SSk : public BinaryNode<Token, SSk<Token, Left, Right>, Left, Right>
    {
        typedef BinaryNode<Token, SSk<Token, Left, Right>, Left, Right> Base;
        using Base::right;
        using Base::left;

        SSk(const Left & left, const Right & right)
          : Base(left, right)
        { }

        std::string get_kind() const { return "with-skipper"; }

        using Base::match;
        template <typename T>
        bool match(Parser<Token> * parser, Storage<T> & s) const
        {
            ELL_BEGIN_PARSE
            typename Parser<Token>::Context sav_pos(parser);
            {
                SafeModify<const Node<Token> *> m1(parser->skipper, &right);
                SafeModify<> m2(parser->flags.skip, true);

                parser->skip();
                res = left.match(parser, s);
            }
            if (res)
            {
                parser->skip(); // Advance with restored skipper
            }
            else
            {
                sav_pos.restore(parser);
            }
            ELL_END_PARSE
        }
    };
}

#endif // INCLUDED_ELL_BINARY_NODES_H
