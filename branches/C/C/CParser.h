#ifndef INCLUDED_ELL_C_PARSER_H
#define INCLUDED_ELL_C_PARSER_H

#include <ell/Parser.h>

#include "CLexer.h"
#include "CGrammar.h"

namespace ell
{
    template <>
    struct Parser<Lex>  : public VectorParser<Lex>
    {
        Parser(const Node<Lex> * grammar)
          : VectorParser<Lex>(grammar)
        { }
    };

    struct CParser : public Parser<Lex>
    {
        CParser()
          : Parser<Lex>(& grammar.root)
        { }

        void parse(const std::string & content, const std::string & filename, int start_line = 1)
        {
            file = filename;
            lexer.parse(content.c_str(), start_line);
            Parser<Lex>::parse(lexer.lexemes);
        }

        bool is_type(const Lex & l)
        {
            return typenames.find(l.s) != typenames.end();
        }

        void raise_error(const std::string & msg) const
        {
            std::ostringstream oss;
            if (not file.empty())
                oss << file << ":";
            if (position->line)
                oss << position->line << ": ";
            oss << "before " << dump_position() << ": error: " << msg << std::endl;
            throw std::runtime_error(oss.str());
        }

        std::set<std::string> typenames;
        CLexer lexer;
        std::string file;
        static CGrammar grammar;

        /// Semantics material
        Type * type;

        void clear_type()
        {
            type = new BaseType;
        }

        void set_flag(const Lex & l)
        {
            BaseType * t = (BaseType *) type;
            switch (l.type)
            {
#           define D(ID, var) case TOK_##ID: t->var = C::BaseType::ID; break;
            D(EXTERN, storage)
            D(STATIC, storage)
            D(AUTO, storage)
            D(REGISTER, storage)

            D(VOID, basic_type)
            D(CHAR, basic_type)
            D(SHORT, basic_type)
            D(FLOAT, basic_type)
            D(BOOL, basic_type)

            D(SIGNED, arithm)
            D(UNSIGNED, arithm)
#           undef D

            case LONG:
                switch (t->basic_type)
                {
                case C::BaseType::INT:
                    t->basic_type = C::BaseType::LONG;
                    break;
                case C::BaseType::LONG:
                    t->basic_type = C::BaseType::LONG_LONG;
                    break;
                case C::BaseType::DOUBLE:
                    t->basic_type = C::BaseType::LONG_DOUBLE;
                    t->arithm = C::BaseType::IEEE754;
                    break;
                default:
                    raise_error("Unexpected token");
                }
                break;

            case INT:
                break;
            case DOUBLE:
                if (t->basic_type == C::BaseType::LONG)
                    t->basic_type = C::BaseType::LONG_DOUBLE;
                else
                    t->basic_type = C::BaseType::DOUBLE;
                t->arithm = C::BaseType::IEEE754;
                break;

            case CONST: t->const_flag = true; break;
            case VOLATILE: t->const_flag = true; break;
            }                         
        }

        void emit_declare(const Lex & ident)
        {
        }
    };
}

#endif // INCLUDED_ELL_C_PARSER_H
