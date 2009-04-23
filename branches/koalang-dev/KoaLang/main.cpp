#include <iostream>
#include <fstream>

#include "Interpreter.h"

void dump(const koalang::Object * o)
{
    std::cerr << typeid(* o).name() << ": " << * o << std::endl;
}

int main(int argc, const char ** argv)
{
    koalang::Interpreter ki;
    
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

        //try
        //{
            ki.parse(line.c_str(), "<stdin>");
        //}
        //catch (std::runtime_error & e)
        //{
        //    std::cerr << e.what() << '\n';
        //}
    }

    return 0;
}
