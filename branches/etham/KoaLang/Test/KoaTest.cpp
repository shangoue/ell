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

#include "KoaTest.h"
#include "curses.h"


using namespace std;
using namespace KoaLang;
using namespace KoaLang::Language;
using namespace KoaLang::System;

KoaTest::KoaTest(int argc, const char ** argv)
    : Command(argc, argv), interpreter(grammar)
{
    interpreter.flags.debug = Command::debug_flag;
}

void KoaTest::exec_interactive(const std::string & str)
{
    exec_file(str.c_str());
}

void KoaTest::exec_file(const char * s)
{
    interpreter.clean();

    // The list in wich will be put the line of code
    interpreter.push_stack(new List);

    try
    {
        interpreter.parse(s);
    }
    catch (Parser::ParserError & e)
    {
        cerr << e.what() << endl;
        interpreter.pop_stack();
        return;
    }
    catch (System::Error & e)
    {
        cerr << e.what() << endl;
        interpreter.pop_stack();
        return;
    }

    try
    {
        List * l=interpreter.pop_stack();
        cout << l->what() << endl;

        Abstract * v = l->eval(interpreter.root_context);
        cout << v->what() << endl;

        v = l->execute(interpreter.root_context);
        cout << v->what() << endl;
    }
    catch (LanguageError & e)
    {
        cout << e.what() << endl;
    }
    catch (System::Error & e)
    {
        cerr << e.what() << endl;
    }
}
