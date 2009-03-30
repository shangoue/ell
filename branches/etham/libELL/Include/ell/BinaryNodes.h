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
#   define D(CLASS, NAME)                                                            \
    template <typename Token, typename Left, typename Right>                         \
    struct CLASS : public BinaryNode<Token, CLASS<Token, Left, Right>, Left, Right>  \
    {                                                                                \
        typedef BinaryNode<Token, CLASS<Token, Left, Right>, Left, Right> Base;      \
        using Base::right;                                                           \
        using Base::left;                                                            \
                                                                                     \
        CLASS(const Left & left, const Right & right)                                \
            : Base(left, right)                                                      \
        {                                                                            \
            Node<Token>::name = #NAME;                                               \
        }                                                                            \
                                                                                     \
        using Base::parse;                                                           \
        bool parse(Parser<Token> * parser) const;                                    \
    };                                                                               \

    D(Alternative,           or)
    D(Difference,            bereft of)
    D(Aggregation,           followed by)
    D(List,                  separated by)
    D(BoundRepetition,       until)
    D(DisorderedAggregation, and)
    D(NoSuffix,              not followed by)
#   undef D

    template <typename Token, typename Left, typename Right>
    bool Alternative<Token, Left, Right>::parse(Parser<Token> * parser) const
    {
        BEGIN_PARSE
        match = left.parse(parser) or right.parse(parser);
        END_PARSE
    }

    template <typename Token, typename Left, typename Right>
    bool Difference<Token, Left, Right>::parse(Parser<Token> * parser) const
    {
        BEGIN_PARSE
        typename Parser<Token>::Context sav_pos = parser->save_pos();
        if (right.parse(parser))
            parser->restore_pos(sav_pos);
        else
            match = left.parse(parser);
        END_PARSE
    }

    template <typename Token, typename Left, typename Right>
    bool Aggregation<Token, Left, Right>::parse(Parser<Token> * parser) const
    {
        BEGIN_PARSE
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
        END_PARSE
    }

    template <typename Token, typename Left, typename Right>
    bool List<Token, Left, Right>::parse(Parser<Token> * parser) const
    {
        BEGIN_PARSE
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
        END_PARSE
    }

    template <typename Token, typename Left, typename Right>
    bool BoundRepetition<Token, Left, Right>::parse(Parser<Token> * parser) const
    {
        BEGIN_PARSE
        while (1)
        {
            match = right.parse(parser);
            if (match or not left.parse(parser))
                break;
            parser->skip();
        }
        END_PARSE
    }

    template <typename Token, typename Left, typename Right>
    bool DisorderedAggregation<Token, Left, Right>::parse(Parser<Token> * parser) const
    {
        BEGIN_PARSE
        if (left.parse(parser))
        {
            match = true;
            right.parse(parser);
        }
        else if (right.parse(parser))
        {
            match = true;
            left.parse(parser);
        }
        END_PARSE
    }

    template <typename Token, typename Left, typename Right>
    bool NoSuffix<Token, Left, Right>::parse(Parser<Token> * parser) const
    {
        BEGIN_PARSE
        SafeModify<> m1(parser->flags.no_step_back, false);
        SafeModify<const Node<Token> *> m2(parser->skipper, 0);
        typename Parser<Token>::Context sav_pos = parser->save_pos();
        match = left.parse(parser);
        if (match and right.parse(parser))
        {
            parser->restore_pos(sav_pos);
            match = false;
        }
        END_PARSE
    }
}

#endif // INCLUDED_PARSER_BINARY_NODES_H
