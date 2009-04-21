#include <iostream>
#include <fstream>

#include "Interpreter.h"

int main(int argc, const char ** argv)
{
    koalang::Interpreter ki;
    ki.flags.debug = true;
    
    std::string line, file = "<stdin>";
    std::istream * i = & std::cin;

    if (argc == 2)
    {
        file = argv[1];
        i = new std::ifstream(argv[1]);
    }

    std::cout << "> " << std::flush;
    while (std::getline(* i, line))
    {
        try
        {
            ki.parse(line.c_str(), "<stdin>");
        }
        catch (std::runtime_error & e)
        {
            std::cerr << e.what() << '\n';
        }
        std::cout << "> ";
    }

    return 0;
}
