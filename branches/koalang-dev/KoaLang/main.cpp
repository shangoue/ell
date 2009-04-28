#include <iostream>
#include <fstream>

#include "Interpreter.h"

using namespace koalang;

int main(int argc, const char ** argv)
{
    Interpreter ki;
#   if ELL_DEBUG
    ki.flags.debug = true;
#   endif
    Map * root_context = new Map(0);
    
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

        try
        {
            Object * code = ki.parse(line, file);
            code->eval(root_context);
        }
        catch (std::runtime_error & e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    return 0;
}
