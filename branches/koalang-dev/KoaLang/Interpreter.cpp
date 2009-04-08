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

#include "Interpreter.h"
#include "Grammar.h"

namespace koalang
{
    Interpreter::Interpreter(Grammar & g)
      : ell::Parser(& g.top, & g.skipper)
    {
        root_context = new Object(0);
        stack = new List;
    }

    void Interpreter::push_number(double n)
    {
        push(new Number(n));
    }

    void Interpreter::push_string()
    {
        std::string s;

        const char * p, *lp;
        int i;
        lp=p=Parser::position;

        for (i=0; p[i] && p[i] != '\"'; ++i)
        {
            if (p[i] == '\\')
            {
                ++i;
                s += std::string(lp, p + i - 1) + System::unprotect(p + i);
                lp=p + i + 1;
            }
        }

        if (! p[i])
            raise_error("unterminated string");

        s += std::string(lp, p + i);
        position=p + i + 1;
        push(new String(s.data(), s.data() + s.size()));
    }

    void Interpreter::push_variable(const char * begin)
    {
        // Keep them sorted
        static const char * reserved[] = {"break", "def",
                                          "do", "else",
                                          "for", "if",
                                          "in", "return",
                                          "while"};

        for (unsigned int i = 0; i < sizeof(reserved) / sizeof(const char *); i++)
        {
            int l = position - begin;
            int r = strncmp(begin, reserved[i], l);

            if (! r && reserved[i][l] == '\0') raise_error(unexp_keyword_error);

            if (r < 0) break;
        }

        push(new Variable(begin, position));
    }

    void Interpreter::begin_list()
    {
        Abstract * e1=pop();
        Abstract * e0=pop();
        List * l=new List;
        l->value.push_back(e0);
        l->value.push_back(e1);
        push(l);
        push_stack(l);
    }

    void Interpreter::push_empty()
    {
        push(new List);
    }

    void Interpreter::end_list()
    {
        pop_stack();
    }
}
