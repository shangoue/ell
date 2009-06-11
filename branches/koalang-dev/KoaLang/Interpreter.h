#ifndef __KOALANG_INTERPRETER_H__
#define __KOALANG_INTERPRETER_H__

#include "Parser.h"
#include "Predef.h"

namespace koalang
{
    struct Interpreter
    {
        Interpreter()
          : root_context(new Map(0))
        {
            set_builtins();
        }

        void set_builtins()
        {
            root_context->value["print"] = new Print;
            root_context->value["input"] = new Input;
            root_context->value["eval"] = new Eval;
        }

        void exec_file(const std::string & filename)
        {
            std::ifstream file(filename.c_str());
            std::string file_content, line;
            while (std::getline(file, line))
                file_content += line + '\n';

            try
            {
                Object * code = parser.parse(file_content, filename);
                code->eval(root_context);
            }
            catch (std::runtime_error & e)
            {
                show_error(e);
            }
        }

        void exec_stdin()
        {
            std::string line;
            while (true)
            {
                std::cout << "> " << std::flush;
                if (not std::getline(std::cin, line))
                    break;

                try
                {
                    Object * code = parser.parse(line, "<stdin>");
                    code->eval(root_context);
                }
                catch (std::runtime_error & e)
                {
                    show_error(e);
                }
            }
        }

        void show_error(std::runtime_error & e)
        {
            for (ObjectList::iterator i = root_context->backtrace->begin();
                 i != root_context->backtrace->end();
                 ++i)
            {
                if ((* i)->location)
                    std::cerr << * (* i)->location << ": ";
                std::cerr << ** i << std::endl;
            }
            std::cerr << e.what() << std::endl;
        }

        Map * root_context;
        Parser parser;
    };
}


#endif // __KOALANG_INTERPRETER_H__
