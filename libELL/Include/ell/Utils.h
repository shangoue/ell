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

#ifndef INCLUDED_ELL_UTILS_H
#define INCLUDED_ELL_UTILS_H

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <algorithm>

#include <wchar.h>
#include <stdlib.h>
#include <string.h>

#ifndef ELL_DEBUG
# ifdef NDEBUG
#  define ELL_DEBUG             0
# else
#  define ELL_DEBUG             1
# endif
#endif

#define ELL_DUMP_NODES        1
#define ELL_DUMP_ACTIONS      1
#define ELL_DUMP_SKIPPER      1

# define ELL_BEGIN_PARSE bool match = false; parser->begin_of_parsing(this);
# define ELL_END_PARSE   parser->end_of_parsing(this, match); return match;

# define ELL_PARSER_FLAGS     \
    ELL_FLAG(look_ahead, LkA) \
    ELL_FLAG(action, Act)     \
    ELL_FLAG(skip, Skp)       \
    ELL_FLAG(debug, Dbg)      \

#if ELL_DEBUG == 1
#define ELL_DUMP(arg) do { if (flags.debug) std::cout << arg << std::endl; } while (0)
#define ELL_ENABLE_DUMP(parser) do { (parser).flags.debug = true; } while (0)
#define ELL_DISABLE_DUMP(parser) do { (parser).flags.debug = false; } while (0)
#else
#define ELL_DUMP(arg)
#define ELL_ENABLE_DUMP(parser)
#define ELL_DISABLE_DUMP(parser)
#endif

namespace ell
{
    /// std::basic_string replacement for allocation-free parsers
    template <typename Token>
    struct basic_string
    {
        friend inline std::string operator + (const ell::basic_string<Token> & l, const std::basic_string<Token> & r)
        {
            return std::string(l.position, l.size) + r;
        }

        friend inline std::string operator + (const std::basic_string<Token> & l, const ell::basic_string<Token> & r)
        {
            return l + std::string(r.position, r.size);
        }

        bool operator != (const basic_string<Token> & other) const
        {
            return (size != other.size) || memcmp(position, other.position, size) != 0;
        }

        bool operator == (const basic_string<Token> & other) const
        {
            return (size == other.size) && memcmp(position, other.position, size) == 0;
        }

        const Token * position;
        size_t size;
    };

    typedef basic_string<char> string;

    /// Exception-safe temporary modification
    template <typename T = bool>
    struct SafeModify
    {
        SafeModify(T & _var, T value)
            : var(_var), sav(_var)
        {
            var = value;
        }

        ~SafeModify() { var = sav; }

        T & var;
        T sav;
    };

    //@{
    /// Escape 7-bit ascii characters of the string to make it readable
    inline std::string protect_char(int c)
    {
        std::ostringstream oss;

        if (c == 0)
            oss << "\\0";
        else if ((c >= '\a') & (c <= '\r'))
            oss << '\\' << "abtnvfr"[c - '\a'];
        else if ((c == '\'') | (c == '"') | (c == '\\'))
            oss << '\\' << (char) c;
        else if (((c > 0) & (c < ' ')) | ((c & 0xFFFFFF80) != 0))
            oss << "\\<" << std::hex << c << ">";
        else
            oss << (char) c;

        return oss.str();
    }

    template <typename Token>
    std::string protect(std::basic_string<Token> s)
    {
        std::ostringstream oss;
        oss << s;
        return oss.str();
    }

    template <>
    inline std::string protect<char>(std::basic_string<char> s)
    {
        std::ostringstream oss;
        for (unsigned int i = 0; i < s.size(); ++i)
            oss << protect_char(s[i]);
        return oss.str();
    }

    template <>
    inline std::string protect<wchar_t>(std::basic_string<wchar_t> s)
    {
        std::ostringstream oss;
        for (unsigned int i = 0; i < s.size(); ++i)
            oss << protect_char(s[i]);
        return oss.str();
    }

    template <typename Char>
    std::string dump_position(const Char * position)
    {
        std::string s = "\"";
        const Char * p = position;
        while (* p && p - position < 31)
        {
            s += protect_char(* p);
            ++p;
        }
        s += "\"";
        if (s.size() == 2)
            return "end";
        if (* p)
            s += "...";
        return s;
    }
    //@}
}

#endif // INCLUDED_ELL_UTILS_H
