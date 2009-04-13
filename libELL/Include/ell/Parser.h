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

#ifndef INCLUDED_ELL_PARSER_H
#define INCLUDED_ELL_PARSER_H

#include <ell/Utils.h>

namespace ell
{
    template <typename Token>
    struct Node;

    template <typename Token>
    struct Parser;

    /// Every parser should extends this class
    template <typename Token>
    struct ParserBase
    {
        ParserBase(const Node<Token> * grammar)
          : grammar(grammar)
        { }

        virtual ~ParserBase() { }

        void parse()
        {
            ((Parser<Token> *) this)->skip();
            if (not grammar->parse((Parser<Token> *) this))
                mismatch(* grammar);
        }

        void mismatch(const Node<Token> & mismatch) const
        {
            std::ostringstream oss;
            oss << "expecting " << mismatch;
            raise_error(oss.str());
        }

        /// Override this function to use your own exceptions (and put filename and line number)
        virtual void raise_error(const std::string & msg) const = 0;

        struct Flags
        {
            Flags()
              : step_back(true)
              , action(true)
#               if ELL_DEBUG == 1
              , debug(false)
              , level(0)
#               endif
            { }

            bool step_back;
            bool action;
#           if ELL_DEBUG == 1
            bool debug;
            int level;
#           endif
        };

#       if ELL_DEBUG == 1
        void begin_of_parsing(const Node<Token> * node)
        {
            if (flags.debug && node->must_be_dumped())
            {
                std::cout << std::string(++flags.level, ' ');
                std::cout << "\\ " << * node << ": \t"
                          << ((Parser<Token> *) this)->dump_position() << std::endl;
            }
        }

        void end_of_parsing(const Node<Token> * node, bool match)
        {
            if (flags.debug && node->must_be_dumped())
            {
                std::cout << std::string(flags.level--, ' ');
                std::cout << (match ? '/' : '#') << ' ' << * node << ": \t"
                          << ((Parser<Token> *) this)->dump_position() << std::endl;
            }
        }
#       endif

        Flags flags;
        const Node<Token> * grammar;
    };

    /// You will have to write an explicit specialization of this class
    /// if you do not parse a buffer of characters.
    template <typename Char>
    struct Parser : public ParserBase<Char>
    {
        Parser(const Node<Char> * grammar,
               const Node<Char> * skipper = 0)
          : ParserBase<Char>(grammar),
            line_number(1),
            position(0),
            skipper(skipper)
        { }

        void parse(const Char * buffer)
        {
            position = buffer;
            line_number = 1;
            ParserBase<Char>::parse();
        }

        void skip()
        {
            if (skipper)
            {
#               if ELL_DEBUG == 1 && ELL_DUMP_SKIPPER != 1
                SafeModify<> md(ParserBase<Char>::flags.debug, false);
#               endif
                SafeModify<const Node<Char> *> ms(skipper, 0);

                while (ms.sav->parse((Parser<Char> *) this))
                    ;
            }
        }

        void raise_error(const std::string & msg, int line_number) const
        {
            std::ostringstream oss;
            if (line_number)
                oss << line_number << ": ";
            oss << "before " << dump_position() << ": " << msg << std::endl;
            throw std::runtime_error(oss.str());
        }

        std::string dump_position() const
        {
            return ell::dump_position(position);
        }

        void raise_error(const std::string & msg) const
        {
            raise_error(msg, line_number);
        }

        struct Context
        {
            Context(Parser<Char> * parser)
              : line_number(parser->line_number),
                position(parser->position)
            { }

            void restore(Parser<Char> * parser)
            {
                parser->line_number = line_number;
                parser->position = position;
            }

            int line_number;
            const Char * position;
        };

        void next()
        {
            if (* position == '\n')
                ++line_number;
            ++position;
        }

        Char get()
        {
            return * position;
        }

        bool end()
        {
            return * position != 0;
        }

        int line_number;
        const Char * position;
        const Node<Char> * skipper;
    };
}

#endif // INCLUDED_ELL_PARSER_H