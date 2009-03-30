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

#include <iostream>

#define PARSER_DEBUG 1
#define PARSER_DUMP_NODES 1
#include <ell/Grammar.h>
#include <ell/Parser.h>

struct ListTest : public ell::Grammar<char>
{
    struct Parser : public ell::Parser<char>
    {
        Parser(ListTest * g)
          : ell::Parser<char>(& g->root2, & g->blank, "1, 2, 3")
        {
            flags.debug = true;
        }

        void printValue(unsigned long v)
        {
            std::cout << v << std::endl;
        }

        void print(const std::string & s)
        {
            std::cout << "-" << s << "-" << std::endl;
        }

        void doNothing(void)
        {
            std::cout << "coucou" << std::endl;
        }

        std::string s;
    };

    ListTest()
    {
        root = dec [& Parser::printValue][& Parser::print] % ch(',')[& Parser::s];

        root2 = root [& Parser::doNothing];

        Parser parser(this);

        parser.parse();
    }

    ell::Rule<char> root, root2;
};

int main()
{
    ListTest l;
    return 0;
}
