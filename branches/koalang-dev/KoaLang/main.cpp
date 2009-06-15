#include "Interpreter.h"

using namespace koalang;

int main(int argc, const char ** argv)
{
    std::string filename;
    Interpreter interpreter;
    bool debug = false;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "-dump")
            ELL_ENABLE_DUMP(interpreter.parser);
        else if (arg == "-lexer-dump")
            ELL_ENABLE_DUMP(interpreter.parser.lexer);
        else if (arg == "-debug")
            debug = true;
        else if (arg[0] == '-')
            std::cerr << "Not understood option: " << arg << '\n';
        else
            filename = arg;
    }

    if (not filename.empty())
    {
        if (debug)
            interpreter.exec_with_trace(filename);
        else
            interpreter.exec_file(filename);
    }
    else
    {
        interpreter.exec_stdin();
    }

    return 0;
}
