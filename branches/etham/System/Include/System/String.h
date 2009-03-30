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

#ifndef __SIMPLE_SYSTEM_STRING_H__
#define __SIMPLE_SYSTEM_STRING_H__

#include "System/Exception.h"
#include <vector>
#include <cstdarg>

namespace System
{
    /// Convert just one hexadecimal digit into an integer
    /// Return -& if the parameter is not an hexadecimal digit
    int ascii_to_hex(int c);

    /// String list
    typedef std::vector<std::string> StringList;
    /// String list iterator
    typedef StringList::iterator StringListIterator;

    /// Replace all characters ch1 by ch2 in the string s
    void replace_char(std::string &s, char ch1, char ch2) ;

    //@{
    /// Split a string in substring vector
    void split(const std::string &str, StringList &tokens, const char delimiter);
    void split(const std::string &str, StringList &tokens, const std::string &delimiter);
    //@}

    //@{
    /// Join a string with a specified delimiter
    std::string join(StringList &tokens, const char delimiter);
    std::string join(StringList &tokens, const std::string &delimiter);
    //@}

    /// Check if a list contains a specified string
    StringListIterator contains(StringList &tokens, std::string token);

    /// Dump a string list
    std::ostream &operator<<(std::ostream &os, StringList &sl);

    /// Hash Type
    typedef uint32_t HashType;

    /// Hash a string
    HashType hash_string(const std::string &str);

    //@{
    /// printf into wide or narrow string
    template <typename Char>
    std::basic_string<Char> string_vprintf(const Char *format, va_list args);

    template <>
    std::string string_vprintf(const char * format, va_list args);

    template <>
    std::wstring string_vprintf(const wchar_t * format, va_list args);

    template <typename Char>
    std::basic_string<Char> string_printf(const Char * format, ...)
    {
        va_list args;
        va_start(args, format);
        std::basic_string<Char> res = string_vprintf(format, args);
        va_end(args);
        return res;
    }
    //@}
} // namespace System

#endif // __SIMPLE_SYSTEM_STRING_H__

