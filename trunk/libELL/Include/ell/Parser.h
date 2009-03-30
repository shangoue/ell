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

#ifndef INCLUDED_PARSER_PARSER_H
#define INCLUDED_PARSER_PARSER_H

#include <stdexcept>

#include <ell/Utils.h>

namespace ell
{
    template <typename Token>
    struct Node;

    /// Parser class.
    /// Work on the root node of a Grammar to parse a null-terminated buffer of Tokens.
    /// A token must be convertible to bool to know if it is a null-terminator.
    template <typename Token>
    struct Parser
    {
        Parser(const Node<Token> * _grammar = 0,
               const Node<Token> * _skipper = 0,
               const Token * begin = 0)
          : position(begin),
            grammar(_grammar),
            skipper(_skipper),
            line_number(1)
        { }

        virtual ~Parser() { }

        /// Parse a (new) buffer
        void parse(const Token * position_)
        {
            position = position_;
            line_number = 1;
            parse();
        }

        /// Go on parsing the current buffer
        void parse()
        {
            skip();
            if (not grammar->parse(this))
                raise_error(* grammar);
        }

        void raise_error(const Node<Token> & mismatch) const
        {
            std::ostringstream oss;
            oss << "expecting " << mismatch;
            raise_error(oss.str(), line_number);
        }

        /// Override this function to use your own exceptions (and put filename)
        virtual void raise_error(const std::string & msg, int line = 0) const
        {
            std::ostringstream oss;
            if (line)
                oss << line << ": ";
            oss << "before " << dump_position() << ": " << msg << std::endl;
            throw std::runtime_error(oss.str());
        }

        std::string dump_position() const
        {
            std::string s = "\"";
            const Token * p = position;
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

        struct Flags
        {
            Flags()
              : step_back(true)
              , action(true)
#               if PARSER_DEBUG == 1
              , debug(false)
              , level(0)
#               endif
            { }

            //@{
            /// To be modified only through ModifyFlag class
            bool step_back;
            bool action;
#           if PARSER_DEBUG == 1
            bool debug;
            int level;
#           endif
            //@}
        };

        struct Context
        {
            Context(int _line_number, const Token * _position)
              : line_number(_line_number),
                position(_position)
            { }

            int line_number;
            const Token * position;
        };

        /// For use from Node::parse methods
        //@{
        const Token & get() { return * position; }

        /// Going beyond end of buffer is authorized,
        /// but an error will be raised on get().
        void next()
        {
#           if PARSER_DEBUG == 1
            if (not bool(* position))
                raise_error("Unexpected end", line_number);
#           endif

            if (isnewline(position))
                ++line_number;
            ++position;
        }

        Context save_pos()
        {
            return Context(line_number, position);
        }

        void restore_pos(const Context & c)
        {
            line_number = c.line_number;
            position = c.position;
        }

        void skip()
        {
            if (skipper)
            {
#               if PARSER_DEBUG == 1 && PARSER_DUMP_SKIPPER != 1
                SafeModify<> md(flags.debug, false);
#               endif
                SafeModify<const Node<Token> *> ms(skipper, 0);

                while (ms.sav->parse(this))
                    ;
            }
        }

        Flags flags;
        //@}

        void begin_of_parsing(const Node<Token> * node)
        {
#           if PARSER_DEBUG == 1
            if (flags.debug && node->must_be_dumped())
            {
                std::cout << std::string(++flags.level, ' ');
                std::cout << "\\ " << * node << ": \t" << dump_position() << std::endl;
            }
#           endif
        }

        void end_of_parsing(const Node<Token> * node, bool match)
        {
#           if PARSER_DEBUG == 1
            if (flags.debug && node->must_be_dumped())
            {
                std::cout << std::string(flags.level--, ' ');
                std::cout << (match ? '/' : '#') << ' ' << * node << ": \t" << dump_position() << std::endl;
            }
#           endif
        }

        bool isnewline(const Token * token) const;

        const Token * position;
        const Node<Token> * grammar;
        const Node<Token> * skipper;
        int line_number;
    };

    template <>
    inline bool Parser<char>::isnewline(const char * token) const { return (* token == '\n'); }

    template <>
    inline bool Parser<wchar_t>::isnewline(const wchar_t * token) const { return (* token == L'\n'); }
}

#endif // INCLUDED_PARSER_PARSER_H
