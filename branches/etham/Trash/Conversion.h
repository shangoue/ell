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

#ifndef __KOALANG_SYSTEM_CONVERSION_H__
#define __KOALANG_SYSTEM_CONVERSION_H__

namespace KoaLang
{
    namespace System
    {
        enum {CONV_SUCCESS, CONV_PARTIAL, CONV_ERROR};

        int utf8_to_wide_string(std::wstring & to, const char * from, size_t size);

    } // System namespace
} // KoaLang namespace

#endif // __KOALANG_SYSTEM_CONVERSION_H__
