// This file is part of Ell library.
//
// Ell library is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Ell library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Ell library.  If not, see <http://www.gnu.org/licenses/>.

#include <system/Command.h>

#include <readline/readline.h>
#include <readline/history.h>

using namespace std;

namespace system
{

    Command::Command(int argc, const char ** argv)
        : interactive_flag(false), debug_flag(false), buffer(NULL)
    {
        for (int i=1; i < argc; i++)
        {
            if (argv[i][0] == '-')
            {
                switch (argv[i][1])
                {
                    case 'i': interactive_flag=true;
                              cerr << "Interactive mode set" << endl;
                              break;

                    case 'd': debug_flag=true;
                              cerr << "Debug mode set" << endl;
                              break;

                    default: RaiseError(SystemError, "Option not understood: " << argv[i]);
                }
            }
            else
            {
                files.push_back(argv[i]);
            }
        }

        if (files.empty()) interactive_flag=true;
    }

    Command::~Command()
    {
        if (buffer) delete [] buffer;
    }

    void Command::run()
    {
        if (interactive_flag) cerr << "KoaLang, version " <<
#               include "version"
            << endl;

        for(std::list<const char *>::iterator i=files.begin(); i != files.end(); ++i)
        {
            if (debug_flag) cerr << "Reading file " << * i << endl;
            if (buffer) delete [] buffer;
            read_file(* i);
            exec_file(buffer);
            if (debug_flag) cerr << "Deleting buffer" << endl;
            delete [] buffer;
        }

        if (interactive_flag)
        {
            if (debug_flag) cerr << "Interactive mode active!" << endl;

            const char * prompt = "> ";
            char * line;

            using_history();

            while ((line = readline(prompt)))
            {
                exec_interactive(line);
                add_history(line);
                free(line);
            }
        }
    }

    void Command::read_file(const char * name)
    {
        FILE * f=fopen(name, "rb");
        if (! f) RaiseError(SystemError, "Cannot open file: " << name);

        fseek(f, 0, SEEK_END);
        long t=ftell(f);
        rewind(f);
        if (debug_flag) std::cerr << "Allocating buffer of " << t << " bytes" << std::endl;
        buffer=new char[t + 1];
        int s=fread(buffer, t, 1, f);
        if (s != 1)
        {
            fclose(f);
            delete [] buffer;
            RaiseError(SystemError, "Error reading file " << name);
        }
        buffer[t]='\0';
        fclose(f);
    }

} // System namespace
