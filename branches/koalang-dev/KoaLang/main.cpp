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
        std::string buffer;
        buffer.resize(l + 1);
        if (fread(buffer.data(), l, 1, f) != 1)
            throw std::runtime_error("error reading file");
        fclose(f);
        ki.parse(buffer, argv[1]);
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
