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

#ifndef INCLUDED_ELL_NUMERICS_H
#define INCLUDED_ELL_NUMERICS_H

#include <ell/Node.h>
#include <ell/Parser.h>

namespace ell
{
    namespace
    {
        inline void describe_integer_parser(int Radix, int MinDigits, int MaxDigits, std::ostream & os)
        {
            if (MinDigits != 1 || MaxDigits != 200)
                os << '(';
            if (Radix == 8)
                os << "octal ";
            else if (Radix == 16)
                os << "hexadecimal ";
            else if (Radix != 10)
                os << Radix << "-based ";
            os << "int";
            if (MinDigits != 1 || MaxDigits != 200)
                os << " with " << MinDigits << " < digits < " << MaxDigits << ')';
        }

        //@{
        /// Extract optional sign prefix
        template <typename Token, typename T>
        struct GetSign
        {
            int operator () (Parser<Token> * parser)
            {
                switch (parser->get())
                {
                case '-': parser->next(); return -1;
                case '+': parser->next(); return 1;
                default: return 1;
                }
            }
        };

        template <typename Token>
        struct GetSign<Token, unsigned long>
        {
            int operator () (Parser<Token> *)
            {
                return 1;
            }
        };
        //@}

        //@{
        /// Check radix: return 0 <= n < R, or a negative value if error
        template <typename Token, const int R>
        struct GetDigit
        {
            int operator () (Parser<Token> * parser)
            {
                int d = parser->get() - '0';
                if (d >= R)
                    return -1;
                return d;
            }
        };

        template <typename Token>
        struct GetDigit<Token, 16>
        {
            int operator () (Parser<Token> * parser)
            {
                int c = parser->get();

                if (c < '0') return -1;
                if (c <= '9') return (c - '0');
                if (c < 'A') return -1;
                if (c <= 'F') return (10 + c - 'A');
                if (c < 'a') return -1;
                if (c <= 'f') return (10 + c - 'a');

                return -1;
            }
        };
        //@}
    } // Anonymous namespace

    /// Generic integer parser
    template <typename Token, typename Sign = unsigned long, const int Radix = 10, const int MinDigits = 1, const int MaxDigits = 200>
    struct Integer : public ConcreteNodeBase<Token, Integer<Token, Sign, Radix, MinDigits, MaxDigits> >
    {
        using ConcreteNodeBase<Token, Integer<Token, Sign, Radix, MinDigits, MaxDigits> >::parse;

        template <typename V>
        bool parse(Parser<Token> * parser, Storage<V> & s) const
        {
            ELL_BEGIN_PARSE
            typename Parser<Token>::Context sav_pos = parser->save_pos();

            int digit_nb = 0;
            Sign value = 0;
            int sign = GetSign<Token, Sign>(parser);
            int d = 0;

            // Check mandatory digits
            while (digit_nb < MinDigits)
            {
                d = GetDigit<Token, Radix>(parser);

                if (d < 0)
                {
                    parser->restore_pos(sav_pos);
                    break;
                }

                value = value * Radix + d;
                ++digit_nb;
                parser->next();
            }

            if (d >= 0)
            {
                // Check optional digits
                while (1)
                {
                    d = GetDigit<Token, Radix>(parser);

                    if (d < 0)
                        break;
                    if (digit_nb >= MaxDigits)
                        break;

                    value = value * Radix + d;
                    ++digit_nb;
                    parser->next();
                }

                if (d >= 0)
                {
                    parser->restore_pos(sav_pos); // Too much digits
                }
                else
                {
                    match = true;
                    s.assign(value * sign);
                }
            }
            ELL_END_PARSE
        }

        void describe(std::ostream & os) const
        {
            describe_integer_parser(Radix, MinDigits, MaxDigits, os);
        }
    };

    /// Partial template specialization for numbers of any digits
    template <typename Token, typename Sign, const int Radix>
    struct Integer<Token, Sign, Radix, 1, 200>
      : public ConcreteNodeBase<Token, Integer<Token, Sign, Radix, 1, 200> >
    {
        using ConcreteNodeBase<Token, Integer<Token, Sign, Radix, 1, 200> >::parse;
        using ConcreteNodeBase<Token, Integer<Token, Sign, Radix, 1, 200> >::operator [ ];

        template <typename V>
        bool parse(Parser<Token> * parser, Storage<V> & se) const
        {
            ELL_BEGIN_PARSE
            typename Parser<Token>::Context sav_pos(parser);

            Storage<Sign> s;
            s.value = 0;
            int sign = GetSign<Token, Sign>()(parser);
            int d = GetDigit<Token, Radix>()(parser);

            if (d >= 0)
            {
                s.value = s.value * Radix + d;
                parser->next();
                match = true;

                while (1)
                {
                    d = GetDigit<Token, Radix>()(parser);
                    if (d < 0)
                        break;
                    s.value = s.value * Radix + d;
                    parser->next();
                }

                s.value = s.value * sign;
                assign(se, s);
            }
            else
            {
                sav_pos.restore(parser);
            }
            ELL_END_PARSE
        }

        void describe(std::ostream & os) const
        {
            describe_integer_parser(Radix, 1, 200, os);
        }
    };

    template <typename Token>
    struct Real : public ConcreteNodeBase<Token, Real<Token> >
    {
        using ConcreteNodeBase<Token, Real<Token> >::parse;
        using ConcreteNodeBase<Token, Real<Token> >::operator [ ];

        template <typename V>
        bool parse(Parser<Token> * parser, Storage<V> & s) const
        {
            // No wide string real number parsing implemented
            ELL_BEGIN_PARSE
            char * endptr;
            Storage<double> sd;
            sd.value = strtod(parser->position, & endptr);
            assign(s, sd);

            if (endptr > parser->position)
            {
                parser->position = endptr;
                match = true;
            }
            ELL_END_PARSE
        }

        void describe(std::ostream & os) const
        {
            os << "real";
        }
    };
}

#endif // INCLUDED_ELL_NUMERICS_H
