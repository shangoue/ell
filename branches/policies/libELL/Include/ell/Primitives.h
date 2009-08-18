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

#ifndef INCLUDED_ELL_PRIMITIVES_H
#define INCLUDED_ELL_PRIMITIVES_H

#include <ell/Node.h>
#include <ell/Parser.h>

namespace ell
{
    template <typename Token, typename ConcreteNode>
    struct TokenPrimitiveBase : public ConcreteNodeBase<Token, ConcreteNode>
    {
        using ConcreteNodeBase<Token, ConcreteNode>::parse;

        template <typename V>
        bool parse(Parser<Token> * parser, Storage<V> & s) const
        {
            Storage<Token> si;
            si.value = parser->get();
            assign(s, si);
            return ((ConcreteNode *) this)->parse(parser);
        }
    };

    template <typename Token>
    struct Eps : public TokenPrimitiveBase<Token, Eps<Token> >
    {
        using TokenPrimitiveBase<Token, Eps<Token> >::parse;
        bool parse(Parser<Token> * parser, Storage<void> &) const
        {
            ELL_BEGIN_PARSE
            match = true;
            ELL_END_PARSE
        }

        void describe(std::ostream & os) const { os << "epsilon"; }
    };

    template <typename Token>
    struct Nop : public ConcreteNodeBase<Token, Nop<Token> >
    {
        using ConcreteNodeBase<Token, Nop<Token> >::parse;
        bool parse(Parser<Token> * parser, Storage<void> &) const
        {
            ELL_BEGIN_PARSE
            ELL_END_PARSE
        }

        void describe(std::ostream & os) const { os << "nop"; }
    };

    template <typename Token>
    struct Any : public TokenPrimitiveBase<Token, Any<Token> >
    {
        using TokenPrimitiveBase<Token, Any<Token> >::parse;

        bool parse(Parser<Token> * parser, Storage<void> &) const
        {
            ELL_BEGIN_PARSE
            if (not parser->end())
            {
                match = true;
                parser->next();
            }
            ELL_END_PARSE
        }

        void describe(std::ostream & os) const
        {
            os << "any";
        }
    };

    template <typename Token>
    struct EoS : public TokenPrimitiveBase<Token, EoS<Token> >
    {
        using TokenPrimitiveBase<Token, EoS<Token> >::parse;

        bool parse(Parser<Token> * parser, Storage<void> &) const
        {
            ELL_BEGIN_PARSE
            match = parser->end();
            ELL_END_PARSE
        }

        void describe(std::ostream & os) const
        {
            os << "end";
        }
    };

    template <typename Token>
    struct ChS : public TokenPrimitiveBase<Token, ChS<Token> >
    {
        ChS(const std::string & s)
            : set(s)
        { }

        using TokenPrimitiveBase<Token, ChS<Token> >::parse;

        bool parse(Parser<Token> * parser, Storage<void> &) const
        {
            ELL_BEGIN_PARSE
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
            ELL_END_PARSE
        }

        void describe(std::ostream & os) const
        {
            os << '[' << protect(set) << ']';
        }

        std::string set;
    };

    template <typename Token>
    struct Ch : public TokenPrimitiveBase<Token, Ch<Token> >
    {
        Ch(const Token _c)
          : c(_c)
        { }

        using TokenPrimitiveBase<Token, Ch<Token> >::parse;

        bool parse(Parser<Token> * parser, Storage<void> &) const
        {
            ELL_BEGIN_PARSE
            if (parser->get() == c)
            {
                parser->next();
                match = true;
            }
            ELL_END_PARSE
        }

        void describe(std::ostream & os) const { os << c; }

        const Token c;
    };

	template <>
    inline void Ch<char>::describe(std::ostream & os) const { os << protect_char(c); }

    template <>
    inline void Ch<wchar_t>::describe(std::ostream & os) const { os << protect_char(c); }

    template <typename Token, const Token C1, const Token C2>
    struct Rg : public TokenPrimitiveBase<Token, Rg<Token, C1, C2> >
    {
        using TokenPrimitiveBase<Token, Rg<Token, C1, C2> >::parse;

        bool parse(Parser<Token> * parser, Storage<void> &) const
        {
            ELL_BEGIN_PARSE
            const Token c = parser->get();
            const int d1 = c - C1;
            const int d2 = c - C2;
            if (d1 >= 0 && d2 <= 0)
            {
                parser->next();
                match = true;
            }
            ELL_END_PARSE
        }

        void describe(std::ostream & os) const
        {
            os << '[' << protect_char(C1) << '-' << protect_char(C2) << ']';
        }
    };

    template <typename Token>
    struct Err : public ConcreteNodeBase<Token, Err<Token> >
    {
        Err(const std::string & s)
            : str(s)
        { }

        using ConcreteNodeBase<Token, Err<Token> >::parse;

        bool parse(Parser<Token> * parser, Storage<void> &) const
        {
            ELL_BEGIN_PARSE
            parser->raise_error(str);
            ELL_END_PARSE
        }

        void describe(std::ostream & os) const
        {
            os << "error(\"" << protect(str) << "\")";
        }

    private:
        std::string str;
    };

    template <typename Token>
    struct IStr : public ConcreteNodeBase<Token, IStr<Token> >
    {
        IStr(const std::basic_string<Token> & s)
          : str(s)
        { }

        using ConcreteNodeBase<Token, IStr<Token> >::parse;

        bool parse(Parser<Token> * parser, Storage<void> &) const
        {
            ELL_BEGIN_PARSE
            typename Parser<Token>::Context sav_pos(parser);
            const Token * p = & str[0];
            match = true;
            while (* p)
            {
                const wchar_t c = * p;
                const wchar_t gc = parser->get();

                if (c == gc
                    or (c >= 'A' and c <= 'Z' and c + 32 == gc)
                    or (c >= 'a' and c <= 'z' and c - 32 == gc))
                {
                    parser->next();
                    ++p;
                }
                else
                {
                    match = false;
                    sav_pos.restore(parser);
                    break;
                }
            }
            ELL_END_PARSE
        }

        void describe(std::ostream & os) const
        {
            os << "icase(\"" << protect(str) << "\")";
        }

        std::basic_string<Token> str;
    };

    /// Exact matching string parser
    /// Can compare strings of different types if == operator is defined between them
    /// The ostream << operator must be defined for Token
    template <typename Token>
    struct Str : public ConcreteNodeBase<Token, Str<Token> >
    {
        Str(const std::basic_string<Token> & s)
          : str(s)
        { }

        using ConcreteNodeBase<Token, Str<Token> >::parse;

        bool parse(Parser<Token> * parser, Storage<void> &) const
        {
            ELL_BEGIN_PARSE
            typename Parser<Token>::Context sav_pos(parser);
            const Token * p = str.c_str();
            match = true;
            while (* p)
            {
                if (! (* p == parser->get()))
                {
                    match = false;
                    sav_pos.restore(parser);
                    break;
                }
                parser->next();
                ++p;
            }
            ELL_END_PARSE
        }

        void describe(std::ostream & os) const
        {
            os << '\"' << protect(str) << '\"';
        }

        std::basic_string<Token> str;
    };

    /// Only for byte strings...
    struct UTF8NonASCII : public ConcreteNodeBase<char, UTF8NonASCII>
    {
        using ConcreteNodeBase<char, UTF8NonASCII>::parse;

        bool parse(Parser<char> * parser, Storage<void> &) const
        {
            ELL_BEGIN_PARSE
            Parser<char>::Context sav_pos(parser);

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
                        sav_pos.restore(parser);
                        break;
                    }
                }
            }
            ELL_END_PARSE
        }

        void describe(std::ostream & os) const
        {
            os << "utf8nonascii";
        }
    };
}

#endif // INCLUDED_ELL_PRIMITIVES_H
