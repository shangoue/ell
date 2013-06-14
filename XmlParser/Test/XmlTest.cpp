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
#include <iostream>
#include <cstdlib>

#include <ell/XmlParser.h>

using namespace ell;

#define DUMP(f, ...) fprintf(stderr, f "\n" , ## __VA_ARGS__)
#define ERROR(f, ...) do { DUMP(f , ## __VA_ARGS__); abort(); } while(0)

void nonreg()
{
    struct Vector
    {
        int id;
        const char * value;
        bool result;
    };

    Vector vectors[] =
    {
        {0, "<two roots=\"no error\" /><hem></hem>", true},
        {1, "<not closed=\"error\"><Closed />", false},
        {2, "<closed twice=\"error\"></closed></closed>", false},
        {3, "<not open=\"error\"></not></hum>", false},
        {4, "   <white>  space 2 \r\n  </white> \r\n  \t<top /> ", true},
        {5, "<entities inside_att=\"&quot;'&lt;'\\\"><![CDATA[]]&gt;<>\"]]>&lt;a&gt;</entities>", true},
        {6, "<special\x00e9 char_in_ident_\x00e9=\"\x00e9\"></special\x00e9>", true}
    };

    try
    {
        // Parser test
        for (unsigned int i = 0; i < sizeof(vectors) / sizeof(Vector); i++)
        {
            Vector * v = vectors + i;
            DUMP("\nTEST %d: %s", v->id, v->value);

            // Parse and check result
            XmlGrammar g;
            XmlDomParser p(g);
            ELL_ENABLE_DUMP(p);

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
                std::ostringstream oss;
                oss << * root1;
                XmlDomParser p2(g);
                ELL_ENABLE_DUMP(p2);

                DUMP("Reparse unparsed DOM from first parsing: %s\n", oss.str().c_str());
                try
                {
                    p2.parse(oss.str().c_str());
                }
                catch (std::runtime_error & e)
                {
                    ERROR("Unexpected failure: %s", e.what());
                }

                DUMP("Compare second DOM unparsing with first one");
                XmlNode * root2 = p2.get_root();

                if (! root1->is_equal(* root2))
                {
                    DUMP("\nFirst DOM:");
                    root1->dump(std::cout);
                    DUMP("\nSecond DOM:");
                    root2->dump(std::cout);
                    ERROR("DOMs are differents");
                }
                else
                    DUMP("Ok.");
            }
        }

        // Test for DOM modification methods
        {
            XmlGrammar g;
            XmlDomParser p(g);
            ELL_ENABLE_DUMP(p);

            DUMP("Parse initial xml");
            p.parse("<html><head><meta1 /><style></style><script /></head><body></body></html>");

            DUMP("Modify DOM");
            XmlNode * document = p.get_root();
            XmlNode * style = document->check_name("html")
                              ->first_child()->check_name("head")
                              ->first_child()->next_sibling()->check_name("style");
            style->insert_sibling_node_before(new XmlNode)->set_name("other1");
            style->insert_sibling_node_after(new XmlNode)->set_name("other2");
            style->previous_sibling()->check_name("other1")->previous_sibling()->check_name("meta1")
                 ->insert_sibling_node_before(new XmlNode)->set_name("otherother");
            XmlNode * body = document->first_child()->next_sibling()->check_name("body");

            body->enqueue_child(new XmlNode)->set_name("p")
                ->enqueue_child(new XmlNode)->set_data("Hello, world!");
            body->insert_sibling_node_after(new XmlNode)->set_name("magic");

            DUMP("Unparse modified DOM");
            
            std::ostringstream oss;
            oss << * document;

            XmlDomParser p2(g);
            ELL_ENABLE_DUMP(p2);

            try
            {
                p2.parse("<html><head><otherother /><meta1 /><other1 /><style></style><other2 /><script /></head><body><p>Hello, world!</p></body><magic /></html>");
            }
            catch (std::runtime_error & e)
            {
                ERROR("Unexpected failure: %s", e.what());
            }

            DUMP("Compare second DOM unparsing with first one");
            XmlNode * root2 = p2.get_root();

            if (! document->is_equal(* root2))
            {
                DUMP("\nFirst DOM:");
                document->dump(std::cout);
                DUMP("\nSecond DOM:");
                root2->dump(std::cout);
                ERROR("DOMs are differents");
            }
            else
                DUMP("Ok.");
        }

        // Test DOM iterator
        {
            DUMP("Check iterator");
            XmlGrammar g;
            XmlDomParser p(g);

            p.parse("<racine><hello/><you />hi<How do=\"you\">do</How></racine>");
            for (XmlIterator i=p.get_root()->first(); i; ++i)
            {
                std::cout << ** i;
            }
        }
    }
    catch(std::exception &e)
    {
        ERROR("%s", e.what());
    }
}

int main(int argc, const char ** argv)
{
    if (argc == 1)
    {
        DUMP("No argument given, perform non-regression tests");
        nonreg();
        DUMP("Everything is Ok.\n");
        return 0;
    }

    if (argc > 3)
    {
        ERROR("Usage: %s input [output]", argv[0]);
        return 1;
    }

    std::ifstream file(argv[1]);
    std::string file_content, line;
    while (std::getline(file, line))
        file_content += line + '\n';

    XmlGrammar g;
    XmlDomParser p(g);
    ELL_ENABLE_DUMP(p);

    DUMP("Parse %s", argv[1]);

    try
    {
        p.parse(file_content.c_str());
    }
    catch (std::exception & e)
    {
        ERROR("%s", e.what());
    }

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
