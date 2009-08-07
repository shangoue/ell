#include "c_lexer.h"

namespace ell
{
    const char * Lex::get_name() const
    {
        static const char * names[] = {
#       define D(NAME) #NAME,
#       include "c_tokens.def"
        "<end of file>" };

        return names[(int) type];
    }

    std::ostream & operator << (std::ostream & os, const Lex & l)
    {
        if (l.line)
        {
            // Output a real token
            switch (l.type)
            {
            case TOK_CONSTANT:
                return os << l.s;

            case TOK_LINE:
                return os << "# line " << l.s;

            case TOK_PRAGMA:
                return os << "#pragma " << l.s;

            case TOK_STRING_LIT:
                return os << '"' << l.s << '"';

            default:
                break;
            }
        }

        return os << l.get_name();
    }

    LexemesGrammar::LexemesGrammar()
    {
        root = * ( line_comment 
                 | block_comment
                 | op 
                 | keyword
                 | identifier
                 | string_literal
                 | line
                 | pragma
                 | ( unsigned_constant 
                   | signed_constant
                   | real_constant ) [&Lexer::push_temp]);

        line_comment = str("//") >> * (any - (ch('\n') | end));

        block_comment = str("/*") >> (any | end >> error("Not terminated comment")) * str("*/");

        op = 
#       define OP(NAME, GRAM) (GRAM).operator[](&Lexer::push<TOK_##NAME>) |
#       include "c_tokens.def"
             nop;

        keyword = no_suffix(ident [&Lexer::search_kw], alnum)[&Lexer::push_temp];

        identifier      = lexeme(alpha | ch('$') >> * (alnum | '$')).operator[](&Lexer::push<TOK_IDENTIFIER>);

        string_literal  = lexeme(eps [&Lexer::clear_temp] >> + (! ch('L') >> ch('\"') >> (str("\\\"") | any) [&Lexer::add_slice] * ch('\"'))) [&Lexer::push_string];

        line            = ch('#') >> kw("line") >> (any * ch('\n')).operator[](&Lexer::push<TOK_LINE>);
        pragma          = ch('#') >> kw("pragma") >> (any * ch('\n')).operator[](&Lexer::push<TOK_LINE>);
        
        unsigned_constant = lexeme((( ch('0') >> chset("xX") >> integer<char, unsigned long long, 16, 1, -1>()[&Lexer::temp]
                                    | ch('0') >> integer<char, unsigned long long, 8, 1, -1>()[&Lexer::temp]
                                    | integer<char, unsigned long long, 10, 1, -1>()[&Lexer::temp]) >> (istr("ll") & istr("u")))
                                  |
                                   (( ch('0') >> chset("xX") >> integer<char, unsigned long, 16, 1, -1>()[&Lexer::temp]
                                    | ch('0') >> integer<char, unsigned long, 8, 1, -1>()[&Lexer::temp]
                                    | integer<char, unsigned long , 10, 1, -1>()[&Lexer::temp]) >> (istr("l") & istr("u")))
                                  |
                                   (( ch('0') >> chset("xX") >> integer<char, unsigned, 16, 1, -1>()[&Lexer::temp]
                                    | ch('0') >> integer<char, unsigned, 8, 1, -1>()[&Lexer::temp]
                                    | integer<char, unsigned, 10, 1, -1>()[&Lexer::temp]) >> istr("u")));
          
        signed_constant = lexeme((( ch('0') >> chset("xX") >> integer<char, long long, 16, 1, -1>()[&Lexer::temp]
                                  | ch('0') >> integer<char, long long, 8, 1, -1>()[&Lexer::temp]
                                  | integer<char, long long, 10, 1, -1>()[&Lexer::temp]) >> istr("ll"))
                                |
                                 (( ch('0') >> chset("xX") >> integer<char, long, 16, 1, -1>()[&Lexer::temp]
                                  | ch('0') >> integer<char, long, 8, 1, -1>()[&Lexer::temp]
                                  | integer<char, long, 10, 1, -1>()[&Lexer::temp]) >> istr("l"))
                                |
                                 ( ch('0') >> chset("xX") >> integer<char, int, 16, 1, -1>()[&Lexer::temp]
                                 | no_suffix( ch('0') >> integer<char, int, 8, 1, -1>()[&Lexer::temp]
                                            | integer<char, int, 10, 1, -1>()[&Lexer::temp], ch('.'))
                                 | ! ch('L') >> ch('\'') >> ( any[&Lexer::temp]
                                                            | ch('\\') >> ch('0') >> integer<char, int, 8, 0, 2>()[&Lexer::temp]
                                                            /* TODO: escaped chars */
                                                            )));
          
        real_constant   = lexeme( single_real[&Lexer::temp] >> istr("f")
                                | double_real[&Lexer::temp] >> istr("l")
                                | real[&Lexer::temp]);
    }

    Lexer::Lexer()
    {
#       define KW(NAME, VALUE) keyword.insert(VALUE);
#       include "c_tokens.def"
    }
}
