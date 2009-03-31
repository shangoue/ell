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

#ifndef INCLUDED_PARSER_BINARY_NODES_H
#define INCLUDED_PARSER_BINARY_NODES_H

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
        bool parse(Parser<Token> * parser) const
        {
            ELL_BEGIN_PARSE
            match = left.parse(parser) or right.parse(parser);
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
          : Base(left, right, "bereft of")
        { }

        using Base::parse;
        bool parse(Parser<Token> * parser) const
        {
            ELL_BEGIN_PARSE
            typename Parser<Token>::Context sav_pos = parser->save_pos();
            if (right.parse(parser))
                parser->restore_pos(sav_pos);
            else
                match = left.parse(parser);
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
          : Base(left, right, "followed by")
        { }

        using Base::parse;
        bool parse(Parser<Token> * parser) const
        {
            ELL_BEGIN_PARSE
            typename Parser<Token>::Context sav_pos = parser->save_pos();

            if (left.parse(parser))
            {
                parser->skip();
                if (right.parse(parser))
                    match=true;
                else
                {
                    if (not parser->flags.step_back)
                        parser->raise_error(right);

                    parser->restore_pos(sav_pos);
                }
            }
            else
                parser->restore_pos(sav_pos);
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
        bool parse(Parser<Token> * parser, std::vector<T> & list) const
        {
            ELL_BEGIN_PARSE
            typename Parser<Token>::Context sav_pos = parser->save_pos();
            T element;
            list.clear();

            while (left.parse(parser, element))
            {
                match = true;
                list.push_back(element);
                parser->skip();
                sav_pos = parser->save_pos();

                if (! right.parse(parser))
                    break;
                parser->skip();
            }

            parser->restore_pos(sav_pos);
            ELL_END_PARSE
        }

        bool parse(Parser<Token> * parser) const
        {
            ELL_BEGIN_PARSE
            typename Parser<Token>::Context sav_pos = parser->save_pos();

            while (left.parse(parser))
            {
                match = true;
                parser->skip();
                sav_pos = parser->save_pos();

                if (! right.parse(parser))
                    break;
                parser->skip();
            }

            parser->restore_pos(sav_pos);
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
        bool parse(Parser<Token> * parser) const
        {
            ELL_BEGIN_PARSE
            while (1)
            {
                match = right.parse(parser);
                if (match or not left.parse(parser))
                    break;
                parser->skip();
            }
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
        bool parse(Parser<Token> * parser) const
        {
            ELL_BEGIN_PARSE
            if (left.parse(parser))
                match = right.parse(parser);
            else if (right.parse(parser))
                match = left.parse(parser);
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
        bool parse(Parser<Token> * parser) const
        {
            ELL_BEGIN_PARSE
            SafeModify<> m1(parser->flags.step_back, true);
            typename Parser<Token>::Context sav_pos = parser->save_pos();
            match = left.parse(parser);
            if (match and right.parse(parser))
            {
                parser->restore_pos(sav_pos);
                match = false;
            }
            ELL_END_PARSE
        }
    };
}

#endif // INCLUDED_PARSER_BINARY_NODES_H
