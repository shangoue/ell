#ifndef __KOALANG_PARSER__
#define __KOALANG_PARSER__

#include "Grammar.h"
#include "Types.h"
#include "Operators.h"

namespace ell
{
    using koalang::Lex;
    using koalang::Object;

    template <>
    struct Parser<Lex> : public ParserBase<Lex>
    {
        Parser()
          : ParserBase<Lex>(& grammar.program, & grammar.skipper),
            stack(0)
        { }

        koalang::List * parse(const std::string & buffer, const std::string & filename)
        {
            stack = new koalang::List;
            lexer.parse(buffer.c_str(), filename);
            position = lexer.lexemes.begin();
            ParserBase<Lex>::parse();
            return stack;
        }

        void raise_error(const std::string & msg) const
        {
            std::ostringstream oss;
            oss << * position->location << ": " << "before " << dump_position()
                                        << ": " << msg << std::endl;
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
                if (parser->flags.debug)
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
            std::vector<Lex>::const_iterator i = position;
            int stop = 3;
            bool sep = false;
            while (stop)
            {
                switch (i->type)
                {
                case Lex::END:   stop = 0; os << " \\0"; break;
                case Lex::NL:    stop--; os << "   "; break;

                case Lex::STR:   sep = false; os << '"' << i->s << '"'; break;
                case Lex::OP:    sep = false; os << i->s; break;

                case Lex::NUM:   if (sep) os << ' '; os << i->n; sep = true; break;
                case Lex::IDENT: if (sep) os << ' '; os << i->s; sep = true; break;
                }
                ++i;
            }
            return os.str();
        }

        template <typename Object>
        void push(const Lex & lex)
        {
            Object * o = new Object(lex);
            o->location = lex.location;
            push(o);
        }

        template <typename Object>
        void append(const Lex & lex)
        {
            Object * o = new Object(lex);
            o->location = lex.location;
            push(o);
            append();
        }

        template <typename Object>
        void push_unary(const Lex * name)
        {
            Object * op = new Object;
            op->location = name->location;
            op->target = pop();
            op->name = name->s;
            push(op);
        }

        template <typename Object>
        void push_binary(const Lex * name)
        {
            Object * op = new Object;
            op->location = name->location;
            op->right = pop();
            op->left = pop();
            op->name = name->s;
            push(op);
        }

        void push_define()
        {
            koalang::Assign * def = new koalang::Assign;
            def->name = '=';
            koalang::Function * func = new koalang::Function;

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
            ELL_DUMP("Push " << * o);
            stack->value.push_back(o);
        }

        Object * pop()
        {
            Object * o = stack->value.back();
            ELL_DUMP("Pop " << * o);
            stack->value.pop_back();
            return o;
        }

        void append()
        {
            Object * o = pop();
            ELL_DUMP("and append it");
            ((koalang::List *) stack->value.back())->value.push_back(o);
        }

        std::vector<Lex>::const_iterator position;
        std::string file;

        koalang::Lexer lexer;
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
    typedef ell::Parser<Lex> Parser;
}
#endif // __KOALANG_PARSER__
