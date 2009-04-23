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
#include <fstream>

#define ELL_DEBUG 1
#define ELL_DUMP_NODES 1
#include "Calc.h"

int main(int argc, const char ** argv)
{
    Calc c;
    c.flags.debug = true;

    std::string line, file = "<stdin>";
    std::istream * i = & std::cin;
    const char * prompt = "> ";

    if (argc == 2)
    {
        file = argv[1];
        i = new std::ifstream(argv[1]);
        prompt = "";
    }

    while (true)
    {
        std::cout << prompt << std::flush;

        if (not std::getline(* i, line))
            break;

        double r;
        try
        {
            r = c.eval(line.c_str());
        }
        catch (std::runtime_error & e)
        {
            std::cerr << e.what() << std::endl;
            continue;
        }
        std::cout << "= " << r << std::endl;
    }

    return 0;
}
