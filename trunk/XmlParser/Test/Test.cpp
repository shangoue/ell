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

#include <fstream>

#include <ell/XmlParser.h>

using namespace ell;

#define DUMP(f, ...) fprintf(stderr, f "\n" , ## __VA_ARGS__)
#define ERROR(f, ...) do { DUMP(f , ## __VA_ARGS__); abort(); } while(0)

int nonreg()
{
    struct Vector
    {
        const char * value;
        bool result;
    };

    Vector vectors[] =
    {
        {"<two roots=\"no error\" /><hem></hem>", true},
        {"<not closed=\"error\"><Closed />", false},
        {"<closed twice=\"error\"></closed></closed>", false},
        {"<not open=\"error\"></not></hum>", false},
        {"   <white>  space 2 \r\n  </white> \r\n  \t<top /> ", true},
        {"<entities inside_att=\"&quot;'&lt;'\\\"><![CDATA[]]&gt;<>\"]]>&lt;a&gt;</entities>", true}
    };

    try
    {
        for (unsigned int i = 0; i < sizeof(vectors) / sizeof(Vector); i++)
        {
            Vector * v = vectors + i;
            DUMP("\nTEST %d: %s", i, v->value);

            // Parse and check result
            XmlDomParser p;
#           if PARSER_DEBUG == 1
            p.flags.debug = true;
#           endif

            try
            {
                p.parse(v->value);
            }
            catch (std::runtime_error & e)
            {
                if (v->result)
                    ERROR("Unexpected failure: %s", e.what());
                else
                    DUMP("Parser error caught: %s", e.what());
            }

            if (v->result)
            {
                // Check DOM
                XmlNode * root1 = p.get_root();

                std::ostringstream s1;
                root1->unparse(s1);

                XmlDomParser p2;
#               if PARSER_DEBUG == 1
                p2.flags.debug = true;
#               endif

                DUMP("Reparse unparsed DOM from first parsing:\n%s", s1.str().c_str());
                try
                {
                    p2.parse(s1.str().c_str());
                }
                catch (std::runtime_error & e)
                {
                    ERROR("Unexpected failure: %s", e.what());
                }

                DUMP("Compare second DOM unparsing with first one");
                XmlNode * root2 = p2.get_root();

                std::ostringstream s2;
                root2->unparse(s2);

                if (s1.str() != s2.str())
                {
                    DUMP("\nFirst DOM:");
                    DUMP("s1=\n%s", protect(s1.str()).c_str());
                    root1->dump(std::cout);
                    DUMP("\nSecond DOM:");
                    DUMP("s2=\n%s", protect(s2.str()).c_str());
                    root2->dump(std::cout);
                    ERROR("DOMs are differents");
                }
            }
        }
    }
    catch(std::exception &e)
    {
        ERROR("%s", e.what());
        return 1;
    }

    return 0;
}

int main(int argc, const char ** argv)
{
    if (argc == 1)
    {
        DUMP("No argument given, perform non-regression tests");
        return nonreg();
    }

    if (argc > 3)
    {
        ERROR("Usage: %s input [output]", argv[0]);
        return 1;
    }

    FILE * f = fopen(argv[1], "rb");

    fseek(f, 0, SEEK_END);
    long l = ftell(f);
    rewind(f);

    char * buffer = new char[l + 1];
    if (fread(buffer, l, 1, f) != 1)
    {
        ERROR("IO error");
        return 2;
    }
    buffer[l] = '\0';
    fclose(f);

    XmlDomParser p;
#   if PARSER_DEBUG == 1
    p.flags.debug = false;
#   endif

    DUMP("Parse %s", argv[1]);

    try
    {
        p.parse(buffer);
    }
    catch (std::exception & e)
    {
        ERROR("%s", e.what());
    }

    delete [] buffer;

    XmlNode * root = p.get_root();

    if (argc > 2)
    {
        DUMP("Write DOM to %s", argv[2]);
        std::ofstream output;
        output.open(argv[2]);
        root->unparse(output, 0, 4);
        output.close();
    }
    else
    {
        root->dump(std::cout);
    }

    return 0;
}
