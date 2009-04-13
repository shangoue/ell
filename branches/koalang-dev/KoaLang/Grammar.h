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

#ifndef __KOALANG_GRAMMAR__
#define __KOALANG_GRAMMAR__

#include <ell/Grammar.h>

namespace koalang
{
    struct Lex
    {
        enum Type
        {
            END,
            NL,
            NUM, // TODO: diff real from int
            STR,
            IDENT,
            OP
        } type;

        Lex(Type type = END, int line = 0)
          : type(type),
            line(line)
        { }

        Lex(double n, int line = 0)
          : type(NUM),
            n(n),
            line(line)
        { }

        Lex(const char * begin, const char * end, Type type, int line = 0)
          : type(type),
            s(begin, end),
            line(line)
        { }

        Lex(const char * s, Type type, int line = 0)
          : type(type),
            s(s),
            line(line)
        { }

        operator double () const { return n; }

        operator const std::string & () const { return s; }

        friend std::ostream & operator << (std::ostream & os, const Lex & l)
        {
            switch (l.type)
            {
            case END:   os << "end"; break;
            case NL:    os << "newline"; break;
            case NUM:   os << "number"; break;
            case STR:   os << "string"; break;
            case IDENT: os << "identifier"; break;
            case OP:    os << l.s; break;
            }
            return os;
        }

        bool operator == (const Lex & other) const
        {
            if (type != other.type)
                return false;

            if (type == OP)
                return (s == other.s);
            else
                return true;
        }

        double n;
        std::string s;
        int line;
    };

    struct Lexer : public ell::Grammar<char>,
                   public ell::Parser<char>
    {
        Lexer();

        void parse(const char * buffer)
        {
            lexemes.clear();
            ell::Parser<char>::parse(buffer);
            lexemes.push_back(Lex());
        }

        void push_number(double n) { lexemes.push_back(Lex(n, line_number)); }

        template <const Lex::Type T>
        void push(const char * begin) { lexemes.push_back(Lex(begin, position, T, line_number)); }

        void push_string() { lexemes.push_back(Lex(Lex::STR, line_number)); }

        void push_char(char c) { lexemes.end()->s += c; }

        template <const char C>
        void push_char() { lexemes.end()->s += C; }

        void open(const char * brace)
        {
            nesting.push_back(lexemes.size());
            lexemes.push_back(Lex(brace, position, Lex::OP, line_number));
        }

        void close(const char * brace)
        {
            Lex * last = 0;
            if (not nesting.empty())
            {
                last = & lexemes[nesting.back()];
                if (((last->s[0] == '(') & (* brace == ')')) |
                    ((last->s[0] == '{') & (* brace == '}')) |
                    ((last->s[0] == '[') & (* brace == ']')))
                {
                    nesting.pop_back();
                    lexemes.push_back(Lex(brace, position, Lex::OP, line_number));
                    return;
                }
            }

            std::ostringstream oss;
            oss << "Unexpected '" << * brace << "'";
            if (last)
                oss << " while '" << last->s[0] << "' openned on line " << last->line;
            raise_error(oss.str());
        }

        void newline()
        {
            if (nesting.empty() or lexemes[nesting.back()].s[0] == '{')
                lexemes.push_back(Lex(Lex::NL, line_number));
        }

        ell::Rule<char> top, keyword, string, string_char, op;
        std::vector<Lex> lexemes;
        std::vector<int> nesting;
    };
}

namespace ell
{
    using namespace koalang;

    template <>
    struct Parser<Lex> : public ParserBase<Lex>
    {
        Parser(const Node<Lex> * grammar)
          : ParserBase<Lex>(grammar)
        { }

        void parse(const std::string & filename, const std::vector<Lex> lexemes)
        {
            file = filename;
            position = lexemes.begin();
            ParserBase<Lex>::parse();
        }

        void skip()
        { }

        void raise_error(const std::string & msg) const
        {
            std::ostringstream oss;
            oss << file << ":" << position->line << ": ";
            oss << "before " << * position << ": " << msg << std::endl;
            throw std::runtime_error(oss.str());
        }

        struct Context
        {
            Context(Parser<Lex> * parser)
              : position(parser->position)
            { }

            void restore(Parser<Lex> * parser)
            {
                parser->position = position;
            }

            std::vector<Lex>::const_iterator position;
        };

        void next()
        {
            ++position;
        }

        Lex get()
        {
            return * position;
        }

        bool end()
        {
            return position->type == Lex::END;
        }

        std::string dump_position() const
        {
            std::ostringstream os;
            for (std::vector<Lex>::const_iterator i = position; i->type != Lex::END; ++i)
                os << * i << ' ';
            return os.str();
        }

        std::vector<Lex>::const_iterator position;
        std::string file;
    };
}

namespace koalang
{
    struct Grammar : public ell::Grammar<Lex>
    {
        Grammar();

        ell::Character<Lex> op(const char * op)
        {
            return ell::Character<Lex>(Lex(op, Lex::OP));
        }

        ell::Character<Lex> number, string, identifier, newline;

        ell::Rule<Lex> top, statement, function, expression,
                       order, sum, product, unary, selection,
                       scoped, atome;
    };
}

#endif // __KOALANG_GRAMMAR__
