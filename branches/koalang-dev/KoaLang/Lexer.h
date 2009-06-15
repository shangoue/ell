#include <ell/Grammar.h>
#include <ell/Parser.h>

#include "Location.h"

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

        Lex(Type type = END, Location * location = 0)
          : type(type),
            location(location)
        { }

        Lex(double n, Location * location = 0)
          : type(NUM),
            n(n),
            location(location)
        { }

        Lex(const char * begin, const char * end, Type type, Location * location = 0)
          : type(type),
            s(begin, end),
            location(location)
        { }

        Lex(const char * s, Type type, Location * location = 0)
          : type(type),
            s(s),
            location(location)
        { }

        operator double () const { return n; }

        operator const std::string & () const { return s; }

        // used for parser primitives description
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
        Location * location;
    };

    struct Lexer : public ell::Grammar<char>,
                   public ell::Parser<char>
    {
        Lexer();

        void parse(const char * buffer, const std::string & filename, int line = 1)
        {
            lexemes.clear();
            file = new std::string(filename);
            ell::Parser<char>::parse(buffer, line);
            lexemes.push_back(Lex());
        }

        void push_number(double n) { lexemes.push_back(Lex(n, get_location())); }

        template <const Lex::Type T>
        void push(const char * begin) { lexemes.push_back(Lex(begin, position, T, get_location())); }

        void push_string() { lexemes.push_back(Lex(Lex::STR, get_location())); }

        void push_char(char c) { lexemes.back().s += c; }

        template <const char C>
        void push_char() { lexemes.back().s += C; }

        Location * get_location()
        {
            return new Location(file, ell::Parser<char>::line_number);
        }

        std::string * file;
        ell::Rule<char> top, string, string_char, punct, blank;
        std::vector<Lex> lexemes;
    };
}

