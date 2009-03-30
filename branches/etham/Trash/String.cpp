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

#include "KoaLang/CommonHeaders.h"
#include "KoaLang/System/String.h"

namespace KoaLang
{
    namespace System
    {
        char unprotect(const char * i)
        {
            switch (* i)
            {
                case '0': return '\0';
                case 'a': return '\a';
                case 'b': return '\b';
                case 't': return '\t';
                case 'n': return '\n';
                case 'v': return '\v';
                case 'f': return '\f';
                case 'r': return '\r';
                default : return * i;
            }
        }

        std::string protect(char _c)
        {
            unsigned char c=_c;
#           define SYSCHAR ((const char *) -1)
            struct
            {
                int from;
                const char * to;
            } pairs[]={{'\0', "0"}, {1, SYSCHAR}, {7, "abtnvfr"}, {14, SYSCHAR},
                       {32, NULL}, {34, "\""}, {35, NULL}, {39, "\'"}, {40, NULL}, {92, "\\"},
                       {93, NULL}, {127, SYSCHAR}, {129, SYSCHAR}, {256, SYSCHAR}};
            int i=0;
            while (true)
            {
                if (c >= pairs[i].from && c < pairs[i + 1].from)
                {
                    if (pairs[i].to == NULL) return std::string(1, c);
                    else if (pairs[i].to == SYSCHAR)
                    {
                        std::ostringstream oss;
                        oss << "\\<" << (int) c << '>';
                        return oss.str();
                    }
                    else return '\\' + std::string(1, pairs[i].to[c - pairs[i].from]);
                }
                ++i;
            }
#             undef SYSCHAR
        }

        std::string protect(const char * begin, const char * end)
        {
            std::string out;

            for (const char * c = begin; c < end; c++)
            {
                out += protect(* c);
            }

            return out;
        }

        std::string unprotect(const char * begin, const char * end)
        {
            std::string out;

            for (const char * i = begin; i < end; i++)
            {
                if (* i == '\\')
                {
                    out += unprotect(++i);
                }
                else
                    out += * i;
            }

            return out;
        }

        std::string string_printf(const char *format, ...)
        {
            std::string s;
            va_list args;

            va_start(args, format);
            s=string_vprintf(format, args);
            va_end(args);

            return s;
        }

        std::string string_vprintf(const char *format, va_list args)
        {
            va_list copy, copy2;

            va_copy(copy, args);
            int size = vsnprintf(NULL, 0, format, copy);
            va_end(copy);

            va_copy(copy2, args);
            std::string str(size + 1, '\0');
            vsprintf((char *) str.data(), format, copy2);
            va_end(copy2);

            return str;
        }
    } // System namespace
} // KoaLang namespace
