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

#include <System/Encoding.h>
#include <string>

namespace system
{
    template <>
    std::basic_string<wchar_t> convert_encoding(const std::basic_string<char> & from)
    {
        // TODO: mbstowcs is not supposed to be thread safe
        // Study the needs for using mbsrtowcs instead
        const size_t size = 2048;
        wchar_t buffer[size + 1];
        size_t n = mbstowcs(buffer, from.c_str(), size);

        if (n == (size_t) -1)
            RaiseError(ConversionError, "Impossible to convert string with current locale");
        else if (n == size)
            RaiseError(ConversionError, "Buffer overflow");

        return std::basic_string<wchar_t>(buffer, n);
    }

    template <>
    std::basic_string<char> convert_encoding(const std::basic_string<wchar_t> & from)
    {
        // TODO: wcstombs is not supposed to be thread safe
        // Study the needs for using wcsrtombs instead
        const size_t size = 4096;
        char buffer[size + 1];
        size_t n = wcstombs(buffer, from.c_str(), size);

        if (n == (size_t) -1)
            RaiseError(ConversionError, "Impossible to convert string with current locale");
        else if (n == size)
            RaiseError(ConversionError, "Buffer overflow");

        return std::basic_string<char>(buffer, n);
    }
}
