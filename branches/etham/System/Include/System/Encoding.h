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

#ifndef __SIMPLE_SYSTEM_ENCODING_H__
#define __SIMPLE_SYSTEM_ENCODING_H__

#include "System/Exception.h"
#include "System/String.h"

#include <wchar.h>

namespace System
{
    DeclareError(ConversionError);

    template <typename From, typename To>
    std::basic_string<To> convert_encoding(const std::basic_string<From> & from);

    namespace
    {
        template <typename Char>
        const Char * get_escape_hex_format();

        template <>
        inline const wchar_t * get_escape_hex_format<wchar_t>() { return L"\\x%02x"; }

        template <>
        inline const char * get_escape_hex_format<char>() { return "\\x%02x"; }
    }

    //@{
    /// Escape 7-bit ascii characters of the string to make it readable
    template <typename Char>
    std::basic_string<Char> protect(Char c)
    {
        std::basic_string<Char> out;

        if (c == 0)
        {
            out += '\\';
            out += '0';
        }
        else if (c >= '\a' and c <= '\r')
        {
            out += '\\';
            out += "abtnvfr"[c - '\a'];
        }
        else if (c == '\'' |
                 c == '"' |
                 c == '\\' |
                 c == '?' |
                 c == ' ')
        {
            out += '\\';
            out += c;
        }
        else if ((c > 0 and c < ' ') or (c & 0xFFFFFF80))
        {
            out += string_printf(get_escape_hex_format<Char>(), c);
        }
        else
            out += c;

        return out;
    }

    template <typename Char>
    std::basic_string<Char> protect(const Char * begin, const Char * end)
    {
        std::basic_string<Char> out;

        for (const Char * p = begin; p < end; ++ p)
        {
            out += protect(* p);
        }
        return out;
    }

    template <typename Char>
    inline std::basic_string<Char> protect(const std::basic_string<Char> & s)
    {
        return protect(s.data(), s.data() + s.size());
    }
    //@}

    //@{
    /// Convert the readable string into its binary originator
    template <typename Char>
    std::basic_string<Char> unprotect(const Char * begin, const Char * end)
    {
        std::basic_string<Char> out;

        for (const Char * p = begin; p < end; ++ p)
        {
            if (* p == '\\')
            {
                ++ p;
                if (* p == '0')
                    out += 0;
                else if (* p == 'x')
                {
                    int r1 = ascii_to_hex(p[1]);
                    int r2 = -1;
                    if (r1 >= 0)
                        r2 = ascii_to_hex(p[2]);
                    if (r2 < 0)
                        Abort("Bad format");
                    out += r1 * 16 + r2;
                    p += 3;
                }
                else
                {
                    int i;
                    for (i = 0; i < 7; ++ i)
                    {
                        if (* p == "abtnvfr"[i])
                        {
                            out += ('\a' + i);
                            break;
                        }
                    }

                    if (i == 7)
                        out += * p;
                }
            }
        }

        return out;
    }

    template <typename Char>
    inline std::basic_string<Char> unprotect(const std::basic_string<Char> & s)
    {
        return unprotect(s.data(), s.data() + s.size());
    }
    //@}

    template <>
    inline std::basic_string<char> convert_encoding(const std::basic_string<char> & from) { return from; }

    template <>
    inline std::basic_string<wchar_t> convert_encoding(const std::basic_string<wchar_t> & from) { return from; }

    template <>
    std::basic_string<wchar_t> convert_encoding(const std::basic_string<char> & from);

    template <>
    std::basic_string<char> convert_encoding(const std::basic_string<wchar_t> & from);

    /// Protect and make streamable any basic_string into an utf8 stream
    template <typename T>
    struct ReadableDump
    {
        ReadableDump(const std::basic_string<T> & str)
            : s(str)
        { }

        ReadableDump(const T & c)
            : s(1, c)
        { }

        std::basic_string<T> s;
    };

    template <typename T>
    std::ostream & operator << (std::ostream & os, const ReadableDump<T> & s)
    {
        const std::basic_string<T> & str = s.s;

        typename std::basic_string<T>::const_iterator i;
        for(i = str.begin(); i != str.end(); ++i)
            os << * i;
        return os;
    }

    template <>
    inline std::ostream & operator << <char> (std::ostream & os, const ReadableDump<char> & s)
    {
        const std::basic_string<char> & str = s.s;
        os << System::protect(str);
        return os;
    }

    template <>
    inline std::ostream & operator << <wchar_t> (std::ostream & os, const ReadableDump<wchar_t> & s)
    {
        const std::basic_string<wchar_t> & str = s.s;
        os << System::convert_encoding<wchar_t, char>(System::protect(str));
        return os;
    }
} // System namespace

#endif // __SIMPLE_SYSTEM_ENCODING_H__
