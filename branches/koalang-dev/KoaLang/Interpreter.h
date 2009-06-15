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

        std::string read_file(const std::string & filename)
        {
            std::string file_content, line;
            std::ifstream file(filename.c_str());
            while (std::getline(file, line))
                file_content += line + '\n';
            return file_content;
        }

        void exec_file(const std::string & filename)
        {
            std::string file_content = read_file(filename);
            try
            {
                Object * code = parser.parse(file_content, filename);
                code->eval(root_context);
            }
            catch (std::runtime_error & e)
            {
                show_backtrace(e);
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
                    //std::cout << * code << '\n';
                    Object * o = code->eval(root_context);
                    std::cout << "  " << * o << '\n';
                }
                catch (std::runtime_error & e)
                {
                    show_backtrace(e);
                }
            }
        }

        void exec_with_trace(const std::string & filename)
        {
            std::vector<std::string> lines;
            std::string cmd, line, last_cmd;
            std::ifstream file(filename.c_str());
            while (std::getline(file, line))
                lines.push_back(line + '\n');

            for (int l = 1; l <= (int) lines.size(); ++l)
            {
                std::cout << filename << ":" << l << ": " << lines[l - 1];
                std::cout << "> " << std::flush;

                last_cmd = cmd;
                if (not std::getline(std::cin, cmd))
                    break;
                if (cmd.empty())
                    cmd = last_cmd;

                try
                {
                    if (cmd == "n")
                    {
                        Object * code = parser.parse(lines[l - 1], filename, l);
                        ++l;
                        code->eval(root_context);
                    }
                    else if (cmd == "l")
                    {
                        for (int ctx = l - 3; ctx <= l + 3; ++ctx)
                        {
                            if (ctx >= 1 and ctx <= (int) lines.size())
                            {
                                std::cout << lines[ctx - 1];
                            }
                        }
                    }
                    else
                    {
                        Object * code = parser.parse(cmd, "<stdin>");
                        Object * o = code->eval(root_context);
                        std::cout << "  " << * o << '\n';
                    }
                }
                catch (std::runtime_error & e)
                {
                    show_backtrace(e);
                }
            }
        }

        void show_backtrace(std::runtime_error & e)
        {
            for (ObjectList::iterator i = root_context->backtrace->begin();
                 i != root_context->backtrace->end();
                 ++i)
            {
                if ((* i)->location)
                    std::cerr << * (* i)->location << ": ";
                std::cerr << ** i << '\n';
            }
            std::cerr << e.what() << '\n';
        }

        Map * root_context;
        Parser parser;
    };
}


#endif // __KOALANG_INTERPRETER_H__
