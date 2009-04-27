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
#include "Operators.h"

namespace ell
{
    using namespace koalang;

    template <>
    struct Parser<Lex> : public ParserBase<Lex>
    {
        Parser()
          : ParserBase<Lex>(& grammar.top, & grammar.newline)
        {
            stack = new Block;
        }

        void parse(const char * buffer, const char * filename)
        {
            file = filename;
            lexer.parse(buffer);
            position = lexer.lexemes.begin();
            ParserBase<Lex>::parse();

#           if ELL_DEBUG == 1
            std::cerr << "stack: " << * stack << '\n';
            std::cerr << "eval: \n";
#           endif
            koalang::Object * v = stack->eval(0);
            std::cerr << "result: " << * v << '\n';
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
              : position(parser->position),
                stack_size(parser->stack->value.size())
            { }

            void restore(Parser<Lex> * parser)
            {
                parser->position = position;
#               if ELL_DEBUG == 1
                for (size_t i = parser->stack->value.size(); i > stack_size; --i)
                    std::cout << "Cancel push of " << * parser->stack->value[i - 1] << std::endl;
#               endif
                parser->stack->value.resize(stack_size);
            }

            std::vector<Lex>::const_iterator position;
            size_t stack_size;
        };

        void next() { ++position; }

        Lex get() { return * position; }

        bool end() { return position->type == Lex::END; }

        std::string dump_position() const
        {
            std::ostringstream os;
            for (std::vector<Lex>::const_iterator i = position; i->type != Lex::END; ++i)
            {
                switch (i->type)
                {
                case Lex::END:   os << "<end>"; break;
                case Lex::NL:    os << "\\n"; break;
                case Lex::NUM:   os << i->n; break;
                case Lex::STR:   os << '"' << i->s << '"'; break;
                case Lex::IDENT:
                case Lex::OP:    os << i->s; break;
                }
                os << ' ';
            }
            return os.str();
        }

        template <typename Object>
        void push(const Lex & lex) { push(new Object(lex)); }

        template <typename Object>
        void append(const Lex & lex) { push(new Object(lex)); append(); }

        template <typename Object>
        void push_unary(const Lex * name)
        {
            Object * op = new Object;
            op->target = pop();
            op->name = name->s;
            push(op);
        }

        template <typename Object>
        void push_binary(const Lex * name)
        {
            Object * op = new Object;
            op->right = pop();
            op->left = pop();
            op->name = name->s;
            push(op);
        }

        void push_define()
        {
            Assign * def = new Assign;
            def->name = '=';
            Function * func = new Function;

            func->body = pop();
            func->right = pop()->to<koalang::List>()->value;
            def->left = pop();
            func->left = pop()->to<koalang::List>()->value;

            def->right = func;
            push(def);
        }

        void push_bunch()
        {
            koalang::List * l = new koalang::List;
            Object * second = pop();
            l->value.push_back(pop());
            l->value.push_back(second);
            push(l);
        }

        void push(Object * o)
        {
#           if ELL_DEBUG == 1
            std::cout << "Push " << * o << std::endl;
#           endif
            stack->value.push_back(o);
        }

        Object * pop()
        {
            Object * o = stack->value.back();
#           if ELL_DEBUG == 1
            std::cout << "Pop " << * o << std::endl;
#           endif
            stack->value.pop_back();
            return o;
        }

        void append()
        {
#           if ELL_DEBUG == 1
            std::cout << "Append";
#           endif
            Object * o = pop();
            ((koalang::List *) stack->value.back())->value.push_back(o);
        }

        std::vector<Lex>::const_iterator position;
        std::string file;

        Lexer lexer;
        koalang::Grammar grammar;

        koalang::List * stack;
    };

    template <>
    inline void ell::Parser<Lex>::push<koalang::List>(const Lex & lex)
    {
        push(new koalang::List);
    }

    template <>
    inline void ell::Parser<Lex>::push<koalang::Block>(const Lex & lex)
    {
        push(new koalang::Block);
    }
}

namespace koalang
{
    typedef ell::Parser<Lex> Interpreter;
}
#endif // __KOALANG_INTERPRETER__
