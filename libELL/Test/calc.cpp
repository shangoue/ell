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

#include <stdio.h>
#include <stdlib.h>

#include <readline/readline.h>
#include <readline/history.h>

#define ELL_DEBUG 1
#define ELL_DUMP_NODES 1
#include "Calc.h"

int main()
{
    Calc c;
    c.flags.debug = true;

    const char * prompt = "> ";
    char * line;

    using_history();

    while ((line = readline(prompt)))
    {
        double r;
        try
        {
            r = c.eval(line);
        }
        catch (std::runtime_error & e)
        {
            fprintf(stderr, "%s\n", e.what());
            continue;
        }
        printf("= %lf\n", r);
        add_history(line);
        free(line);
    }

    return 0;
}
