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

#ifndef __KOALANG_SYSTEM_COMMAND_H__
#define __KOALANG_SYSTEM_COMMAND_H__

namespace KoaLang
{
    namespace System
    {
        class Command
        {
            public:
                /// Parse option (and set flags according to)
                /// Non options are considered as files and stored in the list below
                Command(int argc, const char ** argv);

                virtual ~Command();

                // Exec each file, and finish by an interactive mode if set or if no files
                void run();

                //@{
                /// No return status.
                /// Commands are supposed to raise exceptions if smthg goes wrong
                virtual void exec_file(const char * buffer) = 0;
                virtual void exec_interactive(const std::string & line) = 0;
                //@}

            protected:
                std::list<const char *> files;
                bool interactive_flag;
                bool debug_flag;
                const char * prg_name;

            private:
                /// Read a file into a unique big buffer of the right size
                void read_file(const char * name);
                char * buffer;
        };

    } // System namespace
} // KoaLang namespace

#endif // __KOALANG_SYSTEM_COMMAND_H__
