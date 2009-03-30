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

#ifndef INCLUDED_PARSER_GRAMMAR_H
#define INCLUDED_PARSER_GRAMMAR_H

#include <ell/Node.h>

namespace ell
{
    template <typename Token>
    struct Grammar
    {
        Grammar()
          : alnum(chset("a-zA-Z0-9_")),
            blank(chset(" \t\n\r")),
            ident(lexeme(chset("a-zA-Z_") >> * alnum))
        { }

        virtual ~Grammar() { }

        EndOfStream<Token>                      eos;
        Epsilon<Token>                          eps;
        Any<Token>                              any;

        Range<Token, (Token) '0', (Token) '9'>  digit;
        Range<Token, (Token) 'A', (Token) 'Z'>  upper;
        Range<Token, (Token) 'a', (Token) 'z'>  lower;
        Charset<Token>                          alnum;

        Charset<Token>                          blank;

        Lexeme<Token, Aggregation<Token, Charset<Token>, Repeat<Token, Charset<Token>, 0, -1> > >
                                                ident;

        Range<Token, (Token) '!', (Token) '~'>  visible_ascii;
        UTF8NonASCII                            utf8nonascii;

        Integer<Token, signed long>             signed_decimal;

        template <typename Sign, const int Radix, const int MinDigits, const int MaxDigits>
        Integer<Token, Sign, Radix, MinDigits, MaxDigits>
                                                integer() const { return Integer<Token, Sign, Radix, MinDigits, MaxDigits>(); }

        Integer<Token, unsigned long>           dec;
        Integer<Token, unsigned long, 16>       hex;
        Integer<Token, unsigned long, 8>        oct;
        Integer<Token, unsigned long, 2>        bin;

        Real<Token>                             real;

        template <typename P>
        NoAction<Token, P>                      no_action(const P & p) const { return NoAction<Token, P>(p); }

        template <typename P>
        NoStepBack<Token, P>                    no_step_back(const P & p) const { return NoStepBack<Token, P>(p); }

        template <typename P>
        NoConsume<Token, P>                     no_consume(const P & p) const { return NoConsume<Token, P>(p); }

        template <typename P>
        Lexeme<Token, P>                        lexeme(const P & p) const { return Lexeme<Token, P>(p); }

        template <typename P, const int exact>
        Repeat<Token, P, exact, exact>          repeat(const P & p) const { return Repeat<Token, P, exact, exact>(p); }

        template <typename P, const int min, const int max>
        Repeat<Token, P, min, max>              repeat(const P & p) const { return Repeat<Token, P, min, max>(p); }

        template <typename P, typename CP>
        DynRepeat<Token, P, CP>                 repeat(const int CP::*exact, const P & p) const { return DynRepeat<Token, P, CP>(p, exact, exact); }

        template <typename P, typename CP>
        DynRepeat<Token, P, CP>                 repeat(const int CP::*min, const int CP::*max, const P & p) const { return DynRepeat<Token, P, CP>(p, min, max); }

        template <typename P, typename Suffix>
        NoSuffix<Token, P, Suffix>              no_suffix(const P & p, const Suffix & s) const { return NoSuffix<Token, P, Suffix>(p, s); }

        Error<Token>                            error(const std::string & msg) const { return Error<Token>(msg); }

        template <typename T>
        Character<Token>                        ch(const T & t) const { return Character<Token>((Token) t); }

        template <const Token C1, const Token C2>
        Range<Token, C1, C2>                    range() const { return Range<Token, (Token) C1, (Token) C2>(); }

        Charset<Token>                          chset(const std::string & set) const { return Charset<Token>(set); }

        String<Token>                           str(const std::basic_string<Token> & arg) const { return String<Token>(arg); }

        IgnoreCaseString<Token>                 istr(const std::basic_string<Token> & arg) const { return IgnoreCaseString<Token>(arg); }

        NoSuffix<Token, String<Token>, Charset<Token> >
                                                kw(const std::basic_string<Token> & s) const { return no_suffix(str(s), alnum); }

        NoSuffix<Token, IgnoreCaseString<Token>, Charset<Token> >
                                                ikw(const std::basic_string<Token> & s) const { return no_suffix(istr(s), alnum); }
    };
}

#endif // INCLUDED_PARSER_GRAMMAR_H
