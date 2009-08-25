#include "CLexer.h"

namespace ell
{
    std::ostream & operator << (std::ostream & os, const Lex & l)
    {
        if (l.line)
        {
            // Output a real token
            switch (l.type)
            {
            case TOK_IDENTIFIER:
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

        // Output expected token
#       define D(ID, NAME) if (l.type == TOK_##ID) os << '`' << NAME << '`'; else
#       include "CTokens.def"
            os << "<EOF>";
        return os;
    }

    LexemesGrammar::LexemesGrammar()
    {
#       define CL &CLexer
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
                   | real_constant ) [CL::push_temp] ) >> end;

        line_comment = lexeme(str("//") >> any * (ch('\n') | end));

        block_comment = lexeme(str("/*") >> (any | end >> error("Not terminated comment")) * str("*/"));

        op = 
#       define OP(ID, NAME, GRAM) (GRAM).operator[](CL::push<TOK_##ID>) |
#       include "CTokens.def"
             nop;

        keyword = ident[CL::search_kw][CL::push_temp];

        identifier = lexeme(( alpha
                            | ch('$') ) >> * (alnum | ch('$'))).operator[](CL::push<TOK_IDENTIFIER>);

        string_literal = lexeme(eps [CL::clear_temp] >> + (! ch('L') >> ch('\"') >> (str("\\\"") | any) [CL::add_slice] * ch('\"'))) [CL::push_string];

        line = ch('#') >> kw("line") >> lexeme(any * ch('\n')).operator[](CL::push<TOK_LINE>);
        pragma = ch('#') >> kw("pragma") >> lexeme(any * ch('\n')).operator[](CL::push<TOK_LINE>);
        
        unsigned_constant = lexeme((( ch('0') >> chset("xX") >> integer<unsigned long long, 16, 1, -1>()[CL::temp]
                                    | ch('0') >> integer<unsigned long long, 8, 1, -1>()[CL::temp]
                                    | integer<unsigned long long, 10, 1, -1>()[CL::temp]) >> (istr("ll") & istr("u")))
                                  |
                                   (( ch('0') >> chset("xX") >> integer<unsigned long, 16, 1, -1>()[CL::temp]
                                    | ch('0') >> integer<unsigned long, 8, 1, -1>()[CL::temp]
                                    | integer<unsigned long , 10, 1, -1>()[CL::temp]) >> (istr("l") & istr("u")))
                                  |
                                   (( ch('0') >> chset("xX") >> integer<unsigned, 16, 1, -1>()[CL::temp]
                                    | ch('0') >> integer<unsigned, 8, 1, -1>()[CL::temp]
                                    | integer<unsigned, 10, 1, -1>()[CL::temp]) >> istr("u")));
          
        signed_constant = lexeme((( ch('0') >> chset("xX") >> integer<long long, 16, 1, -1>()[CL::temp]
                                  | ch('0') >> integer<long long, 8, 1, -1>()[CL::temp]
                                  | integer<long long, 10, 1, -1>()[CL::temp]) >> istr("ll"))
                                |
                                 (( ch('0') >> chset("xX") >> integer<long, 16, 1, -1>()[CL::temp]
                                  | ch('0') >> integer<long, 8, 1, -1>()[CL::temp]
                                  | integer<long, 10, 1, -1>()[CL::temp]) >> istr("l"))
                                |
                                 ( ch('0') >> chset("xX") >> integer<int, 16, 1, -1>()[CL::temp]
                                 | no_suffix( ch('0') >> integer<int, 8, 1, -1>()[CL::temp]
                                            | integer<int, 10, 1, -1>()[CL::temp], ch('.'))
                                 | ! ch('L') >> ch('\'') >> ( any[CL::temp]
                                                            | ch('\\') >> ch('0') >> integer<int, 8, 0, 2>()[CL::temp]
                                                            /* TODO: escaped chars */
                                                            )));
          
        real_constant   = lexeme( real [CL::temp] >> chset("flFL") );

        root.set_transparent();
        ELL_NAME_RULE(line_comment);
        ELL_NAME_RULE(block_comment);
        ELL_NAME_RULE(op);
        ELL_NAME_RULE(keyword);
        ELL_NAME_RULE(identifier);
        ELL_NAME_RULE(string_literal);
        ELL_NAME_RULE(line);
        ELL_NAME_RULE(pragma);
        ELL_NAME_RULE(unsigned_constant);
        ELL_NAME_RULE(signed_constant);
        ELL_NAME_RULE(real_constant);
#       undef CL
    }

    CLexer::CLexer()
      : Parser<char>(& grammar.root, & grammar.blank)
    {
#       define KW(ID, NAME) keywords[NAME] = TOK_##ID;
#       include "CTokens.def"
    }

    LexemesGrammar CLexer::grammar;
}
