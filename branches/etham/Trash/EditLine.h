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

#ifndef __KOALANG_SYSTEM_EDIT_LINE_H__
#define __KOALANG_SYSTEM_EDIT_LINE_H__

namespace KoaLang
{
    namespace System
    {
        /// This class helps to implement a nice console,
        /// by providing a wide variety of shortcuts, word-completion and history.
        class EditLine
        {
            public:
                EditLine();
                virtual ~EditLine() { }

                /// Key must be convertible into Char
                typedef int Key;
                typedef char Char;
                typedef std::basic_string<Char> String;

                enum Action
                {
#                   define D(arg) arg,
#                   define D2(arg, n) arg=n,
#                   include "KoaLang/System/ConsoleActions.h"
#                   undef D2
#                   undef D
                };

                void reset_shortcuts() { shortcuts.clear(); }

                void set_shortcut(Key k, Action a) { shortcuts[k]=a; }

                //@{
                /// For shortcuts saving and reloading
                std::string serialize_shortcuts();
                void load_shortcuts(const char * s);
                //@}

                void add_subst(Key k, const String & s) { substitutions[k]=s; }

                void reset_dictionary() { dictionary.clear(); }
                void add_word(const String & s) { dictionary.insert(s); }

                void reset_history() { history.clear(); }
                void new_line();

                /// Change the content of the current line
                void change_content(const String &);

                /// Try to interpret te key as a shortcut, or do nothing
                /// Return false if the key has been ignored
                bool send_special_key(Key);

                /// Try to interpret the key as a shortcut, or insert it in the line
                void send_printable_key(Key);

            protected:
                virtual void refresh(const String &, int cursor_pos, int begin, int end) = 0;
                virtual void validate(const String & line) = 0;
                virtual void show_history_items(const std::vector<String> & items) = 0;
                virtual bool is_word_delimiter(Char) = 0;
                virtual bool is_bloc_delimiter(Char) = 0;
                virtual int find_matching_tag(const String &, int) = 0;

            private:
                void init();
                void insert(Char);
                void insert(const String &);
                void erase(int begin, int end);
                void move_cursor(int);
                void special_move(int depl, bool (EditLine::*test)(Char));
                void refresh();

                String line;

                int cursor_pos;
                int modifiers;
                int repeats;
                int refresh_begin, refresh_end;
                bool must_refresh;
                Key buffer;
                Key macro;

                std::map<Key, Action> shortcuts;
                std::map<Key, String> substitutions;
                hash_set<String> dictionary;
                std::vector<String> history;
                std::map<Key, std::basic_string<Key> > macros;
                std::map<Key, String> buffers;
        };
    } // System namespace
} // KoaLang namespace

#endif // __KOALANG_SYSTEM_EDIT_LINE_H__
