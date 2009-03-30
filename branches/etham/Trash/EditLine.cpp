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

#include "KoaLang/System/EditLine.h"
#include "KoaLang/Parser/XmlParser.h"

using namespace std;

namespace KoaLang
{
    using namespace Parser;

    namespace System
    {
        EditLine::EditLine()
        {
            init();
        }

        string EditLine::serialize_shortcuts()
        {
            ostringstream s;
            s << "<shortcuts>\n";
            for (map<Key, Action>::iterator i = shortcuts.begin(); i != shortcuts.end(); ++i)
            {
                switch (i->second)
                {
#                   define D2(a, b) D(a)
#                   define D(a) \
                    case a: s << " <defkey name=\"" #a "\">" << i->first << "</defkey>\n"; break;
#                   include "KoaLang/System/ConsoleActions.h"
#                   undef D
#                   undef D2
                }
            }
            s << "</shortcuts>\n";
            return s.str();
        }

        void EditLine::load_shortcuts(const char * s)
        {
            XmlNode document;
            XmlParser p(document);
            if (! p.parse(s))
                RaiseError(SystemError, "Bad XML shortcuts file");

            for (XmlNode * child=document.check_name("shortcuts")->first_child();
                 child != document.last_child();
                 child = child->next_sibling())
            {
                string action=child->get_attrib("name");
                Key k;
                child->get_data(k);
#               define D2(a, b) D(a)
#               define D(a) if (action == #a) set_shortcut(k, a); else
#               include "KoaLang/System/ConsoleActions.h"
#               undef D
#               undef D2
                RaiseError(SystemError, "Unhandled action: " << action);
            }
        }

        void EditLine::new_line()
        {
            if (line.size())
            {
                history.push_back(line);
                line.clear();
            }

            init();
            refresh();
        }

        void EditLine::init()
        {
            cursor_pos=0;
            modifiers=0;
            repeats=0;
            buffer=0;
            macro=0;

            refresh_begin=0;
            refresh_end=0;
            must_refresh=true;
        }

        void EditLine::change_content(const String & value)
        {
            int shift=line.size() - cursor_pos;
            line=value;
            cursor_pos=line.size() - shift;
            if (cursor_pos < 0) cursor_pos=0;
        }

        void EditLine::insert(Char c)
        {
            int sav_pos=cursor_pos;
            line.insert(cursor_pos, 1, c);
            cursor_pos+=1;
            if (refresh_begin > sav_pos) refresh_begin=sav_pos;
            if (refresh_end < cursor_pos) refresh_end=cursor_pos;
            must_refresh=true;
        }

        void EditLine::insert(const String & s)
        {
            int sav_pos=cursor_pos;
            line.insert(cursor_pos, s);
            cursor_pos+=s.size();
            if (refresh_begin > sav_pos) refresh_begin=sav_pos;
            if (refresh_end < cursor_pos) refresh_end=cursor_pos;
            must_refresh=true;
        }

        void EditLine::erase(int begin, int end)
        {
            if (begin < (signed int) line.size() && end > 0)
            {
                line.erase(begin, end);
                if (refresh_begin > begin) refresh_begin=begin;
                refresh_end=line.size();
                must_refresh=true;
            }
        }

        // Notice that cursor can be one char after the end of line
        void EditLine::move_cursor(int pos)
        {
            if (pos < 0) pos=0;
            else if (pos > (signed int) line.size()) pos=line.size();

            if (cursor_pos != pos)
            {
                cursor_pos=pos;
                must_refresh=true;
            }
        }

        void EditLine::refresh()
        {
            if (must_refresh)
            {
                refresh(line, cursor_pos, refresh_begin, refresh_end);
                must_refresh=false;
            }
            refresh_begin=line.size();
            refresh_end=0;
        }

        void EditLine::send_printable_key(Key k)
        {
            if (! send_special_key(k))
            {
                insert(k);
                refresh();
            }
        }

        void EditLine::special_move(int depl, bool (EditLine::*test)(Char))
        {
            do
            {
                move_cursor(cursor_pos + depl);
            }
            while (cursor_pos < (signed int) line.size() &&
                   ! (this->*test)(line[cursor_pos]));
        }

        bool EditLine::send_special_key(Key k)
        {
            if (substitutions.find(k) != substitutions.end())
            {
                insert(substitutions[k]);
            }
            else if (shortcuts.find(k) != shortcuts.end())
            {
                switch (shortcuts[k])
                {
                    case VALIDATE: validate(line);
                                   break;
                    case CANCEL: modifiers=0;
                                 break;
                    case MOVE_FORWARD: move_cursor(cursor_pos + 1);
                                       break;
                    case MOVE_BACKWARD: move_cursor(cursor_pos - 1);
                                        break;
                    case MOVE_BEGIN_LINE: move_cursor(0);
                                          break;
                    case MOVE_END_LINE: move_cursor(line.size());
                                        break;
                    case MOVE_FORWARD_WORD: special_move(1, & EditLine::is_word_delimiter);
                                            break;
                    case MOVE_BACKWARD_WORD: special_move(-1, & EditLine::is_word_delimiter);
                                             break;
                    case MOVE_FORWARD_BLOC: special_move(1, & EditLine::is_bloc_delimiter);
                                            break;
                    case MOVE_BACKWARD_BLOC: special_move(-1, & EditLine::is_bloc_delimiter);
                                             break;
                    case MOVE_MATCHING_TAG: move_cursor(find_matching_tag(line, cursor_pos));
                                            break;
                    default:
                        RaiseError(SystemError, "Action not implemented");
                };
            }
            else return false;

            refresh();
            return true;
        }
    } // System namespace
}   // KoaLang namespace
