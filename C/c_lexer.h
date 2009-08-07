#include <ell/Parser.h>
#include <ell/Grammar.h>

#include <map>
#include <vector>

namespace ell
{
    enum LexId
    {
#       define D(NAME) TOK_##NAME,
#       include "c_tokens.def"
        LAST_TOKEN
    };

    struct Constant
    {
        enum { SIGNED, UNSIGNED, IEEE754 } arith;
        int bytes;
        union
        {
            long double real_;
            unsigned long long unsigned_;
            signed long long signed_;
        };

#       define D(type)             \
        void operator = (type v)   \
        {                          \
            real_ = v;             \
            arith = IEEE754;       \
            bytes = sizeof(type);  \
        }
        D(float) D(double) D(long double)
#       undef D

#       define D(type)             \
        void operator = (type v)   \
        {                          \
            unsigned_ = v;         \
            arith = UNSIGNED;      \
            bytes = sizeof(type);  \
        }
        D(unsigned) D(unsigned long) D(unsigned long long)
#       undef D

#       define D(type)             \
        void operator = (type v)   \
        {                          \
            unsigned_ = v;         \
            arith = SIGNED;        \
            bytes = sizeof(type);  \
        }
        D(int) D(long) D(long long)
#       undef D
    };

    struct Lex
    {
        Lex(LexId type = LAST_TOKEN, int line = 0)
          : type(type),
            line(line)
        { }

        Lex(const std::string & s, LexId type, int line = 0)
          : type(type),
            s(s),
            line(line)
        { }

        bool operator == (const Lex & other) const
        {
            return (type == other.type);
        }

        const char * get_name() const;

        LexId type;
        std::string s;
        int line;
        Constant value;

#       define D(T) void operator = (T v) { value = v; type = TOK_CONSTANT; }
        D(float) D(double) D(long double)
        D(int) D(long) D(long long)
        D(unsigned) D(unsigned long) D(unsigned long long)
#       undef D
    };

    std::ostream & operator << (std::ostream & os, const Lex & l);

    struct LexemesGrammar : public Grammar<char>
    {
        using Grammar<char>::integer;

        LexemesGrammar();

        Rule<char> root,
                   line_comment, block_comment,
                   op, keyword,
                   identifier,
                   string_literal,
                   line, pragma,
                   unsigned_constant, signed_constant, real_constant;
    };

    struct Lexer : public Parser<char>
    {
        Lexer();

        void parse(const char * buffer, int start_line)
        {
            lexemes.clear();
            Parser<char>::parse(buffer, start_line);
            lexemes.push_back(Lex());
        }

        template <const LexId T>
        void push(const std::string & s)
        {
            lexemes.push_back(Lex(s, T, line_number));
        }

        void push_temp(const std::string & s)
        {
            temp.s = s;
            temp.line = line_number;
            lexemes.push_back(temp);
        }

        void clear_temp()
        {
            temp.s.clear();
            temp.line = line_number;
        }

        void add_slice(const std::string & s)
        {
            temp.s += s;
        }

        void push_string()
        {
            temp.type = TOK_STRING_LIT;
            lexemes.push_back(temp);
        }

        bool search_kw(const std::string & s)
        {
            std::map<const char *, LexId>::iterator i = keywords.find(s.c_str());
            if (i != keywords.end())
            {
                temp.type = i->second;
                return true;
            }

            return false;
        }

        std::vector<Lex> lexemes;
        std::map<const char *, LexId> keywords;
        Lex temp;

        static LexemesGrammar grammar;
    };
}
