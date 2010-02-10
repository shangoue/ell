#include "Lexer.h"

namespace koalang
{
    Lexer::Lexer()
      : ell::Parser<char>(& top, & blank)
    {
        top = * lexeme( ident [& Lexer::push<Lex::IDENT>]
                      | string
                      | punct
                      | real [& Lexer::push_number]
                      | + ch('\n') [& Lexer::push<Lex::NL>]);

        string = ch('"') [& Lexer::push_string] >> string_char * ch('"');

        string_char = ch('\\') >> ( ch('a') [& Lexer::push_char<'\a'>]
                                  | ch('b') [& Lexer::push_char<'\b'>]
                                  | ch('t') [& Lexer::push_char<'\t'>]
                                  | ch('n') [& Lexer::push_char<'\n'>]
                                  | ch('v') [& Lexer::push_char<'\v'>]
                                  | ch('f') [& Lexer::push_char<'\f'>]
                                  | ch('r') [& Lexer::push_char<'\r'>])
                                  |            integer<unsigned long,  8, 1, 3>() [& Lexer::push_char]
                                  | ch('x') >> integer<unsigned long, 16, 1, 2>() [& Lexer::push_char]
                                  | ch('u') >> integer<unsigned long, 16, 4, 4>() [& Lexer::push_char]
                                  | ch('U') >> integer<unsigned long, 16, 8, 8>() [& Lexer::push_char]
                    | any [& Lexer::push_char] - ch('\n');

        punct = ( chset("\\<>=") >> ! ch('=')
                | chset("^,?!#*/%+[({:])}-")
                | repeat<1, 2>(chset(".@&|"))) [& Lexer::push<Lex::OP>];

        blank = chset(" \t\r") 
              | (ch('\'') >> any * ch('\n')) [& Lexer::push<Lex::NL>];

        top.name.clear();
        string_char.name.clear();

        ELL_NAME_RULE(punct);
        ELL_NAME_RULE(string);
    }
}
