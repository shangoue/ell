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

#ifndef __KOATEST_H__
#define __KOATEST_H__

#include <System/Command.h>

#include <KoaLang/Grammar.h>
#include <KoaLang/Interpreter.h>

struct KoaTest : public KoaLang::System::Command
{
    KoaTest(int argc, const char ** argv);

    void exec_interactive(const std::string & line);
    void exec_file(const char * s);

    KoaLang::Language::Grammar grammar;
    KoaLang::Language::Interpreter interpreter;
};
#endif // __KOATEST_H__
