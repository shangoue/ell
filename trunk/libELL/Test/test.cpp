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

#include <ell/Grammar.h>
#include <ell/Parser.h>

#include "Calc.h"

#define ERROR(f, ...) do { printf("Error parsing %s: " f "\n", buffer , ## __VA_ARGS__); exit(1); } while (0)

struct Test
{
    Test(const char * name)
    {
        printf("\n## %s\n", name);
    }

    virtual ~Test()
    {
        printf("# Passed.\n");
    }
};

void check(ell::Parser<char> & parser, const char * buffer, bool status, bool full)
{
    ELL_ENABLE_DUMP(parser);
    printf("Parse %s\n", buffer);

    try
    {
        parser.parse(buffer);
    }
    catch (std::runtime_error & e)
    {
        if (status)
            ERROR("  %s", e.what());
        else
            printf("  Expected message catched: %s\n", e.what());
    }

    if (not full xor (parser.get() != '\0'))
        ERROR("  Expecting %s at %s", (full ? "<EOS>" : "not <EOS>"), parser.dump_position().c_str());
}

struct ListTest : public ell::Grammar<char>, public Test
{
    struct Parser : public ell::Parser<char>
    {
        Parser(ListTest * g)
          : ell::Parser<char>(& g->root, & g->blank)
        { }

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
      : Test("ListTest") 
    {
        root = other [& Parser::doNothing];
        other = (dec % ch(',')) [& Parser::getValues];

        ELL_NAME_RULE(root);
        ELL_NAME_RULE(other);

        Parser parser(this);

        bool status, full;
        const char * buffer;

#       define TEST(B, S, F) B; S; F; check(parser, buffer, status, full);
        TEST(buffer="1,",   status=false,  full=false);
        TEST(buffer="2 , 3",  status=true,  full=true );
        TEST(buffer=",",    status=false, full=false);
        TEST(buffer="4",    status=true,  full=true );
        TEST(buffer="5 ,6,", status=true,  full=false);
#       undef TEST
    }

    ell::Rule<char> root, other;
};

struct ListToSetTest : ell::Parser<char>, ell::Grammar<char>, Test
{
    ListToSetTest()
      : ell::Parser<char>(& rule, & blank)
      , Test("ListToSetTest")
    {
        rule = (+ dec) [& ListToSetTest::list];
   
        check(* this, "12 1 9", true, true);
    }
   
    ell::Rule<char> rule;
   
    std::set<int> list;
};

struct ActionChainTest : ell::Parser<char>, ell::Grammar<char>, Test
{
    ActionChainTest()
      : ell::Parser<char>(& top),
        Test("ActionChainTest")
    {
        top = real [& ActionChainTest::f1][& ActionChainTest::f2];

        buffer = "21.4";
        check(* this, buffer, true, true);
    }

    /// Chained actions must be compatible
    void f1(double)
    {
    }

    void f2(double v)
    {
        if (v != 21.4)
            ERROR("Expecting %lf, got %lf", 21.4, v);
    }

    ell::Rule<char> top;
    const char * buffer;
};

struct CalcTest : Calc, Test
{
    CalcTest() : Test("CalcTest")
    {
#       define TEST(E) test(#E, E, true)
        TEST(4.0/5);
        TEST(10+3.0/6-(-3));
#       undef TEST

        test("1+A", 1, false);
        test("1 A", 1, false);
        test("A",   0, false);
    }

    void test(const char * expr, double r, bool ok)
    {
        printf("Eval %s:\n", expr);

        check(* this, expr, ok, ok);

        if (ok)
        {
            double rr = pop();
            printf("%lf\n", rr);
            if (r != rr)
            {
                printf("Expecting %lf, get %lf\n", r, rr);
                exit(1);
            }
        }
    }
};

struct NoConsumeTest : ell::Grammar<char>, Test
{
    NoConsumeTest() : Test("NoConsumeTest")
    {
        root = no_look_ahead(str("toto") >> no_consume(ch('[')) >> str("[]"));

        ell::Parser<char> p(& root);

        check(p, "toto[]", true, true);
    }

    ell::Rule<char> root;
};
 

struct BigRulesTest : ell::Grammar<char>, Test
{
    struct P : public ell::Parser<char>
    {
        P(ell::Node<char> * g) : ell::Parser<char>(g)
        { }

        bool check(const char * begin)
        {
            std::cout << std::string(begin, ell::Parser<char>::position);
            return true;
        }
    };

    BigRulesTest()
      : Test("BigRulesTest")
    {
        root = ikw("hi")
               >> first
               >> no_consume(first >> second)
               >> ( first [&P::check]
                  | second [&P::check])
               >> ch('(') >> (first % ch(',')) >> ch(')')
               >> no_look_ahead( ikw("hi")
               >> first
               >> no_consume(first >> second)
               >> ( first [&P::check]
                  | second [&P::check])
               >> ch('(') >> (first % ch(',')) >> ch(')'));

        first = ikw("tto");
        second = ikw("tti");
    }

    ell::Rule<char> root, first, second;
};

struct DirectRuleAssignTest : ell::Grammar<char>, Test
{
    DirectRuleAssignTest() : Test("DirectRuleAssignTest")
    {
        r1 = r2;
        r2 = ikw("toto");
        ell::Parser<char> p(& r1);
        check(p, "toto", true, true);
    }

    ell::Rule<char> r1,r2;
};

struct LongestOpTest : ell::Grammar<char>, Test
{
    struct Parser : ell::Parser<char>
    {
        Parser(ell::Node<char> * n)
          : ell::Parser<char>(n)
        { }

        bool fail() { return false; }
    };

    LongestOpTest() : Test("LongestOpTest")
    {
        {
            ELL_NAME_RULE(r1) = str("toto") [& Parser::fail] | str("totot");
            Parser p1(& r1);
            check(p1, "totot", true, true);
        }

        {
            ELL_NAME_RULE(r1) = str("toto") [& Parser::fail] || str("totot");
            Parser p1(& r1);
            check(p1, "totot", true, true);
        }

        {
            ELL_NAME_RULE(r2) = str("toto") [& Parser::fail] || str("totot") [& Parser::fail];
            Parser p2(& r2);
            check(p2, "totot", false, false);
        }
    }

    ell::Rule<char> r1,r2;
};

struct GenericIntegerTest : ell::Grammar<char>, Test
{
    GenericIntegerTest() : Test("GenericIntegerTest")
    {
        ELL_NAME_RULE(root) = integer<unsigned, 16, 1, 3>();

        ell::Parser<char> p(& root);
        check(p, "AF0", true, true);
        check(p, "0x0", false, false);
        check(p, "", false, true);
        check(p, "1234", false, false);
        check(p, "123 4", true, false);
    };

    ell::Rule<char> root;
};

int main()
{
    ListTest();
    ListToSetTest();
    ActionChainTest();
    CalcTest();
    NoConsumeTest();
    DirectRuleAssignTest();
    LongestOpTest();
    GenericIntegerTest();

    printf("Everything is ok.\n");
    return 0;
}

