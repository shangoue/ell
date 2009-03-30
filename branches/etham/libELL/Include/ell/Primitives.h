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

#ifndef INCLUDED_PARSER_PRIMITIVES_H
#define INCLUDED_PARSER_PRIMITIVES_H

#include <ell/Node.h>
#include <ell/Parser.h>

namespace ell
{
    template <typename Token, typename ConcreteNode>
    struct TokenPrimitiveBase : public ConcreteNodeBase<Token, ConcreteNode>
    {
        using ConcreteNodeBase<Token, ConcreteNode>::parse;

        bool parse(Parser<Token> * parser, Token & output) const
        {
            const Token & t = parser->get();

            bool match = ((ConcreteNode *) this)->parse(parser);

            if (match)
                output = t;

            return match;
        }
    };

    template <typename Token>
    struct Epsilon : public TokenPrimitiveBase<Token, Epsilon<Token> >
    {
        using TokenPrimitiveBase<Token, Epsilon<Token> >::parse;
        bool parse(Parser<Token> * parser) const
        {
            BEGIN_PARSE
            match = true;
            END_PARSE
        }

        void describe(std::ostream & os) const
        {
            os << "eps";
        }
    };

    template <typename Token>
    struct Any : public TokenPrimitiveBase<Token, Any<Token> >
    {
        using TokenPrimitiveBase<Token, Any<Token> >::parse;
        bool parse(Parser<Token> * parser) const
        {
            BEGIN_PARSE
            if (bool(parser->get()))
            {
                match = true;
                parser->next();
            }
            END_PARSE
        }

        void describe(std::ostream & os) const
        {
            os << ".";
        }
    };

    template <typename Token>
    struct EndOfStream : public TokenPrimitiveBase<Token, EndOfStream<Token> >
    {
        using TokenPrimitiveBase<Token, EndOfStream<Token> >::parse;
        bool parse(Parser<Token> * parser) const
        {
            BEGIN_PARSE
            match = not bool(parser->get());
            END_PARSE
        }

        void describe(std::ostream & os) const
        {
            os << "<EOS>";
        }
    };

    template <typename Token>
    struct Charset : public TokenPrimitiveBase<Token, Charset<Token> >
    {
        Charset(const std::string & s)
            : set(s)
        { }

        using TokenPrimitiveBase<Token, Charset<Token> >::parse;
        bool parse(Parser<Token> * parser) const
        {
            BEGIN_PARSE
            const char * p = & set[0];
            const wchar_t c = parser->get();
            while (* p)
            {
                const wchar_t first = * p;
                if (c == first)
                {
                    match = true;
                    parser->next();
                    break;
                }
                ++p;
                if (* p == '-' && * (p + 1))
                {
                    const wchar_t second = * (p + 1);

                    if (c >= first && c <= second)
                    {
                        parser->next();
                        match = true;
                        break;
                    }

                    p += 2;
                }
            }
            END_PARSE
        }

        void describe(std::ostream & os) const
        {
            os << '[' << protect<char>(set) << ']';
        }

        std::string set;
    };

    template <typename Token>
    struct Character : public TokenPrimitiveBase<Token, Character<Token> >
    {
        Character(const Token _c)
            : c(_c)
        { }

        using TokenPrimitiveBase<Token, Character<Token> >::parse;
        bool parse(Parser<Token> * parser) const
        {
            BEGIN_PARSE
            if (parser->get() == c)
            {
                parser->next();
                match = true;
            }
            END_PARSE
        }

        void describe(std::ostream & os) const
        {
            os << '\'' << protect_char(c) << '\'';
        }

        const Token c;
    };

    template <typename Token, const Token C1, const Token C2>
    struct Range : public TokenPrimitiveBase<Token, Range<Token, C1, C2> >
    {
        using TokenPrimitiveBase<Token, Range<Token, C1, C2> >::parse;
        bool parse(Parser<Token> * parser) const
        {
            BEGIN_PARSE
            const Token c = parser->get();
            const int d1 = c - C1;
            const int d2 = c - C2;
            if (d1 >= 0 && d2 <= 0)
            {
                parser->next();
                match = true;
            }
            END_PARSE
        }

        void describe(std::ostream & os) const
        {
            os << '[' << protect_char(C1) << '-' << protect_char(C2) << ']';
        }
    };

    template <typename Token>
    struct Error : public ConcreteNodeBase<Token, Error<Token> >
    {
        Error(const std::string & s)
            : str(s)
        { }

        using ConcreteNodeBase<Token, Error<Token> >::parse;
        bool parse(Parser<Token> * parser) const
        {
            BEGIN_PARSE
            parser->raise_error(str, parser->line_number);
            END_PARSE
        }

        void describe(std::ostream & os) const
        {
            os << "error(\"" << protect<Token>(str) << "\")";
        }

    private:
        std::string str;
    };

    template <typename Token>
    struct IgnoreCaseString : public ConcreteNodeBase<Token, IgnoreCaseString<Token> >
    {
        IgnoreCaseString(const std::basic_string<Token> & s)
            : str(s)
        { }

        using ConcreteNodeBase<Token, IgnoreCaseString<Token> >::parse;
        bool parse(Parser<Token> * parser) const
        {
            BEGIN_PARSE
            typename Parser<Token>::Context sav_pos = parser->save_pos();
            const Token * p = & str[0];
            match = true;
            while (* p)
            {
                const wchar_t c = * p;
                const wchar_t gc = parser->get();

                if (c == gc
                    || (c >= 'A' && c <= 'Z' && c + 32 == gc)
                    || (c >= 'a' && c <= 'z' && c - 32 == gc))
                {
                    parser->next();
                    ++p;
                }
                else
                {
                    match = false;
                    parser->restore_pos(sav_pos);
                    break;
                }
            }
            END_PARSE
        }

        void describe(std::ostream & os) const
        {
            os << "icase(\"" << protect<Token>(str) << "\")";
        }

        std::basic_string<Token> str;
    };

    /// Exact matching string parser
    /// Can compare strings of different types if == operator is defined between them
    /// The ostream << operator must be defined for Token
    template <typename Token>
    struct String : public ConcreteNodeBase<Token, String<Token> >
    {
        String(const std::basic_string<Token> & s)
            : str(s)
        { }

        using ConcreteNodeBase<Token, String<Token> >::parse;
        bool parse(Parser<Token> * parser) const
        {
            BEGIN_PARSE
            typename Parser<Token>::Context sav_pos = parser->save_pos();
            const Token * p = str.c_str();
            match = true;
            while (* p)
            {
                if (! (* p == parser->get()))
                {
                    match = false;
                    parser->restore_pos(sav_pos);
                    break;
                }
                parser->next();
                ++p;
            }
            END_PARSE
        }

        void describe(std::ostream & os) const
        {
            os << '\"' << protect<Token>(str) << '\"';
        }

        std::basic_string<Token> str;
    };

    /// Only for byte strings...
    struct UTF8NonASCII : public ConcreteNodeBase<char, UTF8NonASCII>
    {
        using ConcreteNodeBase<char, UTF8NonASCII>::parse;
        bool parse(Parser<char> * parser) const
        {
            BEGIN_PARSE
            Parser<char>::Context sav_pos = parser->save_pos();

            unsigned char c = parser->get();
            if (c < 0xC0 || c > 0xFD)
                match = false;
            else
            {
                match = true;
                parser->next();
                c = parser->get();
                int n = 1;

                if (c >= 0xE0)
                    n++;
                if (c >= 0xF0)
                    n++;
                if (c >= 0xF8)
                    n++;
                if (c >= 0xFC)
                    n++;

                for (int i = 0; i < n; ++i)
                {
                    c = parser->get();
                    parser->next();
                    if (c < 0x80 || c > 0xBF)
                    {
                        match = false;
                        parser->restore_pos(sav_pos);
                        break;
                    }
                }
            }
            END_PARSE
        }

        void describe(std::ostream & os) const
        {
            os << "utf8nonascii";
        }
    };
}

#endif // INCLUDED_PARSER_PRIMITIVES_H
