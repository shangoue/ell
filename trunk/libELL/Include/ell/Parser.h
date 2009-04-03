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

#include <stdexcept>
#include <ell/Utils.h>

namespace ell
{
    template <typename Token>
    struct Node;

    /// Parser class.
    /// Work on the root node of a Grammar to iterate over a container of (Token *).
    /// A token must be convertible to bool to know if it is a null-terminator.
    template <typename Token>
    struct Parser
    { };

    template <typename Token>
    struct ParserBase
    {
        ParserBase(const Node<Token> * grammar,
                   const Node<Token> * skipper = 0,
                   const Token * begin = 0)
          : position(begin),
            grammar(grammar),
            skipper(skipper)
        { }

        virtual ~ParserBase() { }

        /// Parse a (new) buffer
        void parse(const Token * position_)
        {
            position = position_;
            parse();
        }

        /// Go on parsing the current buffer
        void parse()
        {
            skip();
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

            //@{
            /// To be modified only through ModifyFlag class
            bool step_back;
            bool action;
#           if ELL_DEBUG == 1
            bool debug;
            int level;
#           endif
            //@}
        };

        /// Redefine your context if needed
        struct Context
        {
            Context(ParserBase * parser)
              : position(parser->position)
            { }

            void restore(ParserBase * parser)
            {
                parser->position = position;
            }

            Token * position;
        };

        /// Redefine the way you iterate through tokens if needed
        void next()
        { 
            ++position;
        }

        /// Redefine this for custom dump
        std::string dump_position()
        {
            std::ostringstream oss;
            oss << * position;
            return oss.str();
        }

        const Token & get()
        {
            return * position;
        }

        void skip()
        {
            if (skipper)
            {
#               if ELL_DEBUG == 1 && ELL_DUMP_SKIPPER != 1
                SafeModify<> md(flags.debug, false);
#               endif
                SafeModify<const Node<Token> *> ms(skipper, 0);

                while (ms.sav->parse((Parser<Token> *) this))
                    ;
            }
        }

        void begin_of_parsing(const Node<Token> * node)
        {
#           if ELL_DEBUG == 1
            if (flags.debug && node->must_be_dumped())
            {
                std::cout << std::string(++flags.level, ' ');
                std::cout << "\\ " << * node << ": \t"
                          << ((Parser<Token> *) this)->dump_position() << std::endl;
            }
#           endif
        }

        void end_of_parsing(const Node<Token> * node, bool match)
        {
#           if ELL_DEBUG == 1
            if (flags.debug && node->must_be_dumped())
            {
                std::cout << std::string(flags.level--, ' ');
                std::cout << (match ? '/' : '#') << ' ' << * node << ": \t"
                          << ((Parser<Token> *) this)->dump_position() << std::endl;
            }
#           endif
        }

        Flags flags;
        const Token * position;
        const Node<Token> * grammar;
        const Node<Token> * skipper;
    };

    template <typename Char>
    struct StringParser : public ParserBase<Char>
    {
        StringParser(const Node<Char> * grammar,
                     const Node<Char> * skipper = 0,
                     const Char * begin = 0)
          : ParserBase<Char>(grammar, skipper, begin)
        {
            line_number = 1;
        }

        typedef ParserBase<Char> Base;
        using Base::position;

        void parse(const Char * position)
        {
            line_number = 1;
            Base::parse(position);
        }

        void raise_error(const std::string & msg, int line_number) const
        {
            std::ostringstream oss;
            if (line_number)
                oss << line_number << ": ";
            oss << "before " << dump_position() << ": " << msg << std::endl;
            throw std::runtime_error(oss.str());
        }

        void raise_error(const std::string & msg) const
        {
            raise_error(msg, line_number);
        }

        std::string dump_position() const
        {
            std::string s = "\"";
            const Char * p = position;
            while (* p and p - position < 31)
            {
                s += protect_char(* p);
                ++p;
            }
            s += "\"";
            if (s.size() == 2)
                return "<EOS>";
            if (* p)
                s += "...";
            return s;
        }

        struct Context
        {
            Context(StringParser<Char> * parser)
              : line_number(parser->line_number),
                position(parser->position)
            { }

            void restore(StringParser<Char> * parser)
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

        int line_number;
    };

    template <>
    struct Parser<char> : public StringParser<char>
    {
        Parser(const Node<char> * grammar,
               const Node<char> * skipper = 0,
               const char * begin = 0)
          : StringParser<char>(grammar, skipper, begin)
        { }
    };

    template <>
    struct Parser<wchar_t> : public StringParser<wchar_t>
    { 
        Parser(const Node<wchar_t> * grammar,
               const Node<wchar_t> * skipper = 0,
               const wchar_t * begin = 0)
          : StringParser<wchar_t>(grammar, skipper, begin)
        { }
    };
}

#endif // INCLUDED_ELL_PARSER_H
