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

#ifndef __KOALANG_SYSTEM_STRING_H__
#define __KOALANG_SYSTEM_STRING_H__

namespace KoaLang
{
    namespace System
    {
        std::string string_printf(const char *format, ...);

        std::string string_vprintf(const char *format, va_list args);

        // Take the char after the backslash, and return the syschar
        char unprotect(const char * i);

        std::string protect(char _c);

        std::string protect(const char * begin, const char * end);

        inline std::string protect(const std::string & s) { return protect(s.data(), s.data() + s.size()); }

        std::string unprotect(char * begin, const char * end);
    } // System namespace
} // KoaLang namespace


#endif // __KOALANG_SYSTEM_STRING_H__
