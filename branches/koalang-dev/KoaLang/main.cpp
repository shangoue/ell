#include <iostream>

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "Interpreter.h"

int main(int argc, const char ** argv)
{
    koalang::Interpreter ki;

    if (argc == 2)
    {
        FILE * f = fopen(argv[1], "rb");
        fseek(f, 0, SEEK_END);
        long l = ftell(f);
        rewind(f);
        char * buffer = new char[l + 1];
        if (fread(buffer, l, 1, f) != 1)
            throw std::runtime_error("error reading file");
        buffer[l] = '\0';
        fclose(f);
        ki.parse(buffer, argv[1]);
        delete buffer;
    }
    else
    {
        const char * prompt = "> ";
        char * line;
        using_history();

        while ((line = readline(prompt)))
        {
            try
            {
                ki.parse(line, "<stdin>");
            }
            catch (std::runtime_error & e)
            {
                std::cerr << e.what() << '\n';
            }
            add_history(line);
            free(line);
        }
    }

    return 0;
}
