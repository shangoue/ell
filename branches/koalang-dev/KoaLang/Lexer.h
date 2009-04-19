#include <ell/Grammar.h>
#include <ell/Parser.h>

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

        void push_char(char c) { lexemes.back().s += c; }

        template <const char C>
        void push_char() { lexemes.back().s += C; }

        ell::Rule<char> top, keyword, string, string_char, op, blank;
        std::vector<Lex> lexemes;
    };
}

