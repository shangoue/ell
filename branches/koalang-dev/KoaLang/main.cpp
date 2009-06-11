#include "Interpreter.h"

using namespace koalang;

int main(int argc, const char ** argv)
{
    std::string filename;
    Interpreter interpreter;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "-debug")
            ELL_ENABLE_DUMP(interpreter.parser);
        else if (arg == "-lexer-debug")
            ELL_ENABLE_DUMP(interpreter.parser.lexer);
        else
            filename = arg;
    }

    if (not filename.empty())
    {
        interpreter.exec_file(filename);
    }
    else
    {
        interpreter.exec_stdin();
    }

    return 0;
}
