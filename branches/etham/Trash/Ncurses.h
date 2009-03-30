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

#ifndef __KOALANG_SYSTEM_NCURSES_H__
#define __KOALANG_SYSTEM_NCURSES_H__
#include <ncurses.h>
#include "KoaLang/System/EditLine.h"
namespace KoaLang
{
    namespace System
    {
        class Ncurses : public EditLine
        {
            public:
                Ncurses();

                ~Ncurses();

                void set_shortcuts();

                bool is_printable(int ch);

                void edit();

            private:
                void refresh(const String & s, int cursor_pos, int begin, int end);

                void validate(const String & line);

                void show_history_items(const std::vector<String> & items);

                bool is_word_delimiter(Char _c);

                bool is_bloc_delimiter(Char c);

                int find_matching_tag(const String &, int pos);
        };
    } // System namespace
} // KoaLang namespace
#endif // __KOALANG_SYSTEM_NCURSES_H__
