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

    /// Every Parser extends this class
    template <typename Token>
    struct ParserBase
    {
        ParserBase(const Node<Token> * grammar, const Node<Token> * skipper = 0)
          : grammar(grammar),
            skipper(skipper)
        { }

        virtual ~ParserBase() { }

        void parse()
        {
            ((Parser<Token> *) this)->skip();
            if (not grammar->parse((Parser<Token> *) this))
                mismatch(* grammar);
        }

        void skip()
        {
            if (flags.skip & (skipper != 0))
            {
#               if ELL_DEBUG == 1 && ELL_DUMP_SKIPPER != 1
                SafeModify<> md(flags.debug, false);
#               endif
                SafeModify<> ms(flags.skip, false);

                while (skipper->parse((Parser<Token> *) this))
                    ;
            }
        }

        /// Return the number of tokens consumed since the given context
        /// (must be overriden with use of next() for non contiguous buffers)
        //virtual size_t measure(Context & start) = 0;

        void mismatch(const Node<Token> & mismatch) const
        {
            std::ostringstream oss;
            oss << "expecting \"" << mismatch << "\"";
            raise_error(oss.str());
        }

        /// Override this function to use your own exceptions (and put filename and line number)
        virtual void raise_error(const std::string & msg) const = 0;

        struct Flags
        {
            Flags()
#           if ELL_DEBUG == 1
              : debug(false) , level(0)
#           endif
            {
#               define ELL_FLAG(FLAG, N) FLAG = true;
                ELL_PARSER_FLAGS
#               undef ELL_FLAG
            }

#           define ELL_FLAG(FLAG, N) bool FLAG;
            ELL_PARSER_FLAGS
#           undef ELL_FLAG

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
                ++flags.level;
                std::cout << std::string(flags.level, ' ')
                          << "\\ " << * node << ": \t"
                          << ((Parser<Token> *) this)->dump_position() << std::endl;
            }
        }

        void end_of_parsing(const Node<Token> * node, bool match)
        {
            if (flags.debug && node->must_be_dumped())
            {
                std::cout << std::string(flags.level, ' ')
                          << (match ? '/' : '#') << ' ' << * node << ": \t"
                          << ((Parser<Token> *) this)->dump_position() << std::endl;
                if (flags.level > 0)
                    --flags.level;
            }
        }
#       else
        void begin_of_parsing(const Node<Token> *) { }
        void end_of_parsing(const Node<Token> *, bool) { }
#       endif

        Flags flags;
        const Node<Token> * grammar;
        const Node<Token> * skipper;
    };

    /// Typical parser for buffer of contiguous characters, with handling of newlines.
    template <typename Char>
    struct CharParser : public ParserBase<Char>
    {
        CharParser(const Node<Char> * grammar,
                   const Node<Char> * skipper)
          : ParserBase<Char>(grammar, skipper),
            line_number(1),
            position(0)
        { }

        void parse(const Char * buffer, int start_line = 1)
        {
            position = buffer;
            line_number = start_line;
            ParserBase<Char>::parse();
        }

        std::string dump_position() const
        {
            std::string s = "\"";
            const Char * p = position;
            while (* p && p - position < 31)
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

        void raise_error(const std::string & msg) const
        {
            std::ostringstream oss;
            if (not file.empty())
                oss << file << ":";
            if (line_number)
                oss << line_number << ": ";
            oss << "before " << dump_position() << ": " << msg << std::endl;
            throw std::runtime_error(oss.str());
        }

        struct Context
        {
            Context(CharParser<Char> * parser)
              : line_number(parser->line_number),
                position(parser->position)
            { }

            void restore(CharParser<Char> * parser)
            {
                parser->line_number = line_number;
                parser->position = position;
            }

            int line_number;
            const Char * position;
        };

        size_t measure(Context & start)
        {
            return position - start.position;
        }

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
            return * position == 0;
        }

        int line_number;
        const Char * position;
        std::string file;
    };

    template <>
    struct Parser<char> : public CharParser<char>
    {
        Parser(const Node<char> * grammar = 0,
               const Node<char> * skipper = 0)
          : CharParser<char>(grammar, skipper)
        { }
    };

    template <>
    struct Parser<wchar_t> : public CharParser<wchar_t>
    {
        Parser(const Node<wchar_t> * grammar = 0,
               const Node<wchar_t> * skipper = 0)
          : CharParser<wchar_t>(grammar, skipper)
        { }
    };

    /// Parser at syntaxic level only, works on a vector of lexemes
    template <typename Lex>
    struct VectorParser : public ParserBase<Lex>
    {
        VectorParser(const Node<Lex> * grammar)
          : ParserBase<Lex>(grammar)
        { }

        void parse(const std::vector<Lex> & lexemes)
        {
            position = lexemes.begin();
            ParserBase<Lex>::parse();
        }

        struct Context
        {
            Context(VectorParser<Lex> * parser)
            {
                position = parser->position;
            }

            void restore(VectorParser<Lex> * parser)
            {
                parser->position = position;
            }

            typename std::vector<Lex>::const_iterator position;
        };

        void skip()
        { }

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
            return ! bool(* position);
        }

        size_t measure(Context & start)
        {
            return position - start.position;
        }

        std::string dump_position() const
        {
            std::ostringstream os;
            typename std::vector<Lex>::const_iterator it = position;
            int i = 0;
            os << '"';
            while (bool(* it) && i < 9)
            {
              os << * it << ' ';
              ++it;
              ++i;
            }
            os << '"';
            if (i == 9)
              os << "...";
            else
              os << "<EOS>";
            return os.str();
        }

        typename std::vector<Lex>::const_iterator position;
    };
}

#endif // INCLUDED_ELL_PARSER_H