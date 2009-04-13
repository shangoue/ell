#include <iostream>

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "Interpreter.h"

int main()
{
    koalang::Interpreter ki;
    ki.flags.debug = true;

    const char * prompt = "> ";
    char * line;

    using_history();

    while ((line = readline(prompt)))
    {
        try
        {
            ki.parse(line);
            std::cout << * ki.stack->eval() << '\n';
        }
        catch (std::runtime_error & e)
        {
            std::cerr << e.what() << '\n';
            continue;
        }
        add_history(line);
        free(line);
    }

    return 0;
}
