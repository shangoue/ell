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

#ifndef __KOALANG_INTERPRETER__
#define __KOALANG_INTERPRETER__

#include "Grammar.h"
#include "Types.h"

namespace ell
{
    using namespace koalang;

    template <>
    struct Parser<Lex> : public ParserBase<Lex>
    {
        Parser(const Node<Lex> * grammar, const Node<Lex> * skipper)
          : ParserBase<Lex>(grammar, skipper)
        { }

        void parse(const std::string & filename, const std::vector<Lex> & lexemes)
        {
            file = filename;
            position = lexemes.begin();
            ParserBase<Lex>::parse();
        }

        void raise_error(const std::string & msg) const
        {
            std::ostringstream oss;
            oss << file << ":" << position->line << ": ";
            oss << "before " << dump_position() << ": " << msg << std::endl;
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
            {
                switch (i->type)
                {
                case Lex::END:   os << "end"; break;
                case Lex::NL:    os << "newline"; break;
                case Lex::NUM:   os << i->n; break;
                case Lex::STR:   os << '"' << i->s << '"'; break;
                case Lex::IDENT:
                case Lex::OP:    os << i->s; break;
                }
                os << ' ';
            }
            return os.str();
        }

        std::vector<Lex>::const_iterator position;
        std::string file;
    };
}

namespace koalang
{
    struct Interpreter : public ell::Parser<Lex>
    {
        Interpreter()
          : ell::Parser<Lex>(& grammar.top, & grammar.newline)
        {
            root_scope = new Map;
            stack = new Block;
        }

        void parse(const char * buffer, const char * file = "<stdin>")
        {
            lexer.parse(buffer);
            ell::Parser<Lex>::parse(file, lexer.lexemes);
        }

        Lexer lexer;
        Grammar grammar;

        template <typename Object>
        void push(const Lex & lex)
        {
            stack->value.push_back(new Object(lex));
        }

        template <typename Object>
        void binary(const Lex * name)
        {
            Object * op = new Object;
            op->right = pop();
            op->left = pop();
            op->name = name->s;
            push(op);
        }

        template <typename Object>
        void unary(const Lex * name)
        {
            Object * op = new Object;
            op->target = pop();
            op->name = name->s;
            push(op);
        }

        void define()
        {
            //TODO
        }

        bool is_defined(const Lex * name)
        {
            // TODO
            pop();
            return false;
        }

        Object * push(Object * o) { stack->value.push_back(o); return o; }
        Object * pop() { Object * o = stack->value.back(); stack->value.pop_back(); return o; }

        Lex op;
        Map * root_scope;
        Block * stack;
    };
}
#endif // __KOALANG_INTERPRETER__
