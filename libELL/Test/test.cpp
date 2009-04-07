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

#include <iostream>
#include <cstdio>
#include <cstdlib>

#define ELL_DEBUG 1
#define ELL_DUMP_NODES 0

#include <ell/Grammar.h>
#include <ell/Parser.h>

#include "Calc.h"

#define ERROR(f, ...) do { printf("Error parsing %s: " f "\n", buffer , ## __VA_ARGS__); exit(1); } while (0)

void check(ell::Parser<char> & parser, const char * buffer, bool status, bool full)
{
    printf("Parse %s\n", buffer);
    try
    {
        parser.parse(buffer);
    }
    catch (std::runtime_error & e)
    {
        if (status)
            ERROR("%s", e.what());
    }
    if (not full xor bool(parser.get()))
        ERROR("Expecting %s at %s", (full ? "<EOS>" : "smthg"), parser.position);
}

struct ListTest : public ell::Grammar<char>
{
    struct Parser : public ell::Parser<char>
    {
        Parser(ListTest * g)
          : ell::Parser<char>(& g->root, & g->blank)
        {
            flags.debug = true;
        }

        void doNothing(void)
        {
            std::cout << "coucou" << std::endl;
        }

        void getValues(const std::vector<unsigned long> & l)
        {
            for (unsigned int i = 0; i < l.size(); ++i)
                std::cout << l[i] << ", ";
            std::cout << std::endl;
        }

        std::string s;
    };

    ListTest()
    {
        root = other [& Parser::doNothing];
        other = (dec % ch(',')) [& Parser::getValues];

        ELL_NAME_RULE(root);
        ELL_NAME_RULE(other);

        Parser parser(this);

        bool status, full;
        const char * buffer;

#       define TEST(B, S, F) B; S; F; check(parser, buffer, status, full);
        TEST(buffer="1,",   status=true,  full=false);
        TEST(buffer="2 , 3",  status=true,  full=true );
        TEST(buffer=",",    status=false, full=false);
        TEST(buffer="4",    status=true,  full=true );
        TEST(buffer="5 ,6,", status=true,  full=false);
#       undef TEST
    }

    ell::Rule<char> root, other;
};

struct MyParser : ell::Parser<char>, ell::Grammar<char>
{
    MyParser()
      : ell::Parser<char>(& rule, & blank)
    {
        rule = (+ dec) [& MyParser::list];
   
        check(* this, "12 1 9", true, true);
    }
   
    ell::Rule<char> rule;
   
    std::set<int> list;
};

struct CalcTest : public Calc
{
    CalcTest()
    {
#       define TEST(E) test(#E, E)
        TEST(4/5);
        TEST(10+3/6-(-3));
#       undef TEST
    }

    void test(const char * expr, int r)
    {
        printf("Eval %s:\n", expr);
        int rr = eval(expr);
        printf("%d\n", rr);
        if (r != rr)
        {
            printf("Expecting %d, get %d\n", r, rr);
            abort();
        }
    }
};

int main()
{
    ListTest l;
    MyParser p;
    CalcTest c;

    printf("Everything is ok.\n");
    return 0;
}
