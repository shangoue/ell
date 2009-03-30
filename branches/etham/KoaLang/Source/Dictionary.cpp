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

#include "KoaLang/Language/Dictionary.h"

namespace koalang
{
    namespace Language
    {
        Dictionary dictionary;

        const std::string * Dictionary::add(const char * begin, const char * end)
        {
            HashSet::iterator i = strings.insert(std::string(begin, end)).first;

            const std::string & s = * i;

            return & s;
        }

        const std::string * Dictionary::add(const std::string & s)
        {
            HashSet::iterator i = strings.insert(s).first;

            const std::string & _s = * i;

            return & _s;
        }
    } // Language namespace
}
