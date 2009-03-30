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
#include <ncurses.h>
#include "KoaLang/System/Ncurses.h"
namespace KoaLang
{
    namespace System
    {
        Ncurses::Ncurses()
        {
            newterm(NULL, stdout, stdin);
            def_prog_mode();
            cbreak();
            noecho();
            nonl();
            keypad(stdscr, TRUE);
        }

        Ncurses::~Ncurses()
        {
            endwin();
        }

        void Ncurses::set_shortcuts()
        {
            int ch;
#           define D2(a, b) D(a)
#           define D(a)                               \
            printw("Type a shortcut for " #a ": ");   \
            ::refresh();                              \
            ch = getch();                             \
            printw("\n");                             \
            set_shortcut(ch, a);

#           include "KoaLang/System/ConsoleActions.h"
#           undef D
#           undef D2
        }

        bool Ncurses::is_printable(int ch)
        {
            return ! (ch & 0xFFFFFF00 || ch < 32);
        }

        void Ncurses::edit()
        {
            printw("> ");
            ::refresh();

            while (1)
            {
                int ch = getch();

                if (ch == 27) break;

                if (is_printable(ch))
                    send_printable_key(ch);
                else
                    send_special_key(ch);
            }
        }

        void Ncurses::refresh(const String & s, int cursor_pos, int begin, int end)
        {
            move(0, begin + 2);
            for (int i = begin; i < end; ++i)
            {
                if (i < (signed int) s.size())
                    addch(s[i]);
                else
                    addch(' ');
            }
            move(0, cursor_pos + 2);
            ::refresh();
        }

        void Ncurses::validate(const String & line)
        {
            endwin();
            printf("Receive command %s\n", line.c_str());
            fflush(stdout);
            ::refresh();

            new_line();
        }

        void Ncurses::show_history_items(const std::vector<String> & items)
        { }

        bool Ncurses::is_word_delimiter(Char _c)
        {
            unsigned char c = (unsigned char) _c;
            bool inword = (c >= 'a' && c <= 'z') ||
                          (c >= 'A' && c <= 'Z') ||
                          (c >= '0' && c <= '9') ||
                          (c == '_') ||
                          (c >= 128);

            return ! inword;
        }

        bool Ncurses::is_bloc_delimiter(Char c)
        {
            return c <= 32;
        }

        int Ncurses::find_matching_tag(const String &, int pos)
        {
            return pos;
        }
    } // System namespace
} // KoaLang namespace
