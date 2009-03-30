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

#ifndef __KOALANG_DICTIONARY_H__
#define __KOALANG_DICTIONARY_H__

namespace koalang
{
    class Dictionary
    {
        public:
            const std::string * add(const char * begin, const char * end);

            const std::string * add(const std::string & s);

        private:
            typedef hash_set<std::string> HashSet;

            HashSet strings;
    };

    /// Static singleton
    extern Dictionary dictionary;
}

#endif // __KOALANG_DICTIONARY_H__
