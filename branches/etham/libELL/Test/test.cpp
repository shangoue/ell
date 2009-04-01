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

#define ELL_DEBUG 1
#define ELL_DUMP_NODES 1
#include <ell/Grammar.h>
#include <ell/Parser.h>

struct ListTest : public ell::Grammar<char>
{
    struct Parser : public ell::Parser<char>
    {
        Parser(ListTest * g)
          : ell::Parser<char>(& g->root, & g->blank, "1 , 2,3,")
        {
            flags.debug = true;
        }

        void doNothing(void)
        {
            std::cout << "coucou" << std::endl;
        }

        void getValues(std::vector<unsigned long> l)
        {
            for (unsigned int i = 0; i < l.size(); ++i)
                std::cout << l[i] << ", ";
            std::cout << std::endl;
        }

        std::string s;
    };

    ListTest()
    {
        root = other [& Parser::doNothing];
        other = (dec % ch(',')) [& Parser::getValues];

        Parser parser(this);

        parser.parse();
    }

    ell::Rule<char> root, other;
};

int main()
{
    ListTest l;
    return 0;
}
