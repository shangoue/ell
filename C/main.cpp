#include "CGrammar.h"
#include "CParser.h"

#include <fstream>

int main(int argc, const char ** argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " file.c" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    std::string file_content, line;
    while (std::getline(file, line))
        file_content += line + '\n';

    ell::CParser p;
    ELL_ENABLE_DUMP(p);
    //ELL_ENABLE_DUMP(p.lexer);
    p.parse(file_content, argv[1]);

    return 0;
}
