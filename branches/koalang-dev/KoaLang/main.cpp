#include <iostream>
#include <fstream>

#include "Parser.h"

using namespace koalang;

int main(int argc, const char ** argv)
{
    Parser ki;
    Map * root_context = new Map(0);
    std::string filename;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "-debug")
            ELL_ENABLE_DUMP(ki);
        else if (arg == "-lexer-debug")
            ELL_ENABLE_DUMP(ki.lexer);
        else
            filename = arg;
    }

    if (not filename.empty())
    {
        std::ifstream file(filename.c_str());

        std::string file_content, line;
        while (std::getline(file, line))
            file_content += line + '\n';

        try
        {
            Object * code = ki.parse(file_content, filename);
            code->eval(root_context);
        }
        catch (std::runtime_error & e)
        {
            std::cerr << e.what() << '\n';
        }
    }
    else
    {
        std::string line;
        while (true)
        {
            std::cout << "> " << std::flush;
            if (not std::getline(std::cin, line))
                break;

            try
            {
                Object * code = ki.parse(line, "<stdin>");
                code->eval(root_context);
            }
            catch (std::runtime_error & e)
            {
                std::cerr << e.what() << '\n';
            }
        }
    }

    return 0;
}
