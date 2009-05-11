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
#include <cwchar>

#ifndef ELL_DEBUG
#ifdef NDEBUG
#define ELL_DEBUG             0
#else
#define ELL_DEBUG             1
#endif
#endif

#define ELL_DUMP_NODES        1
#define ELL_DUMP_ACTIONS      1
#define ELL_DUMP_SKIPPER      0

#define ELL_NAME_RULE(arg) arg.set_name(#arg);
#define ELL_TRANSPARENT_RULE(arg) arg.set_name(0);

# define ELL_BEGIN_PARSE bool match = false; parser->begin_of_parsing(this);
# define ELL_END_PARSE   parser->end_of_parsing(this, match); return match;

# define ELL_PARSER_FLAGS           \
    ELL_FLAG(look_ahead, LookAhead) \
    ELL_FLAG(consume, Consume)      \
    ELL_FLAG(action, Action)        \
    ELL_FLAG(skip, Skip)            \

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
        else if (c >= '\a' and c <= '\r')
            oss << '\\' << "abtnvfr"[c - '\a'];
        else if ((c == '\'') | (c == '"') | (c == '\\'))
            oss << '\\' << (char) c;
        else if ((c > 0 and c < ' ') or (c & 0xFFFFFF80))
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
        while (* p and p - position < 31)
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
