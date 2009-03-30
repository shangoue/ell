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

#include "Napalm/CommonHeaders.h"

#include "Napalm/Parser/PP.h"
#include "Napalm/Parser/Grammar.h"

namespace Napalm
{
    namespace Parser
    {
        PP::Grammar PP::grammar_;

        //bool PP:checkFunction(const char * begin)
        //{
        //    std::string name(begin, position - 1);
        //    MacroMap::iterator i = macros.find(name);
        //    if (i != functions.end() && i->second.params.size() != 0)
        //    {
        //        onExpansion.push_back(MacroOnExpansion(i));
        //        return true;
        //    }
        //    return false;
        //}

        //void PP:pushParamValue(const char * begin)
        //{
        //    onExpansion.back().params.push_back(std::string(begin, position));
        //}

        //void PP:expandFunction(const char * begin)
        //{
        //    // Check number of parameters
        //    if (onExpansion.params_value.size() != onExpansion.macro->second.params.size())
        //    {
        //        raise_error("%s: Bad number of arguments", onExpansion.macro->first._str());
        //    }

        //    expand();
        //}

        //void PP:expandConstant(const char * begin)
        //{
        //    std::string name(begin, position);
        //    MacroMap::iterator i = macros.find(name);

        //    // If not a constant, match anyway
        //    if (i == functions.end())
        //        return;

        //    onExpansion.push_back(MacroOnExpansion(i));
        //    expand();
        //}

        //void PP:expand()
        //{
        //    // Check for non-recursion
        //    for (std::vector<MacroOnExpansion>::iterator i = onExpansion.begin();
        //         i != onExpansion.back();
        //         i++)
        //    {
        //        if (i->macro == onExpansion.back()->macro)
        //            raise_error("%s: Recursive macro expansion", onExpansion.back()->macro->first);
        //    }

        //}

        PP::PP(PPCallback fileSystem_,  std::string & output_)
            : fileSystem(fileSystem_),
            output(output_)
        {
            grammar = & grammar_.top;
            skipper = & grammar_.space;
        }

        void PP::preprocess(const std::string & ressourceId_)
        {
            std::string buffer;
            if (! (* fileSystem)(ressourceId_, buffer))
            {
                throw Napalm::System::ErrorRaiser<ParserError>(NULL, 0)
                    ("File not found: %s", ressourceId_.c_str());
            }

            SafeModify<std::string> sm(ressourceId, ressourceId_);
            lastPos = buffer.c_str();
            printAnchor();
            parse(buffer.c_str());
            copyText(position);
        }

        void PP::printAnchor()
        {
            output += System::string_printf("# %d \"%s\"\n",
                line_number,
                ressourceId.c_str());
        }

        const char * PP::get_filename()
        {
            return ressourceId.c_str();
        }

        void PP::include(const char * begin)
        {
            std::string res(begin + 1, position - 1);
            Parser<char>::Context pc = save_pos();

            try
            {
                preprocess(res);
            }
            catch (ParserError & e)
            {
                // Backtrace
                restore_pos(pc);
                raise_error("Including file:\n%s", e.what());
            }

            restore_pos(pc);
            printAnchor();
            lastPos = position;
        }

        void PP::copyText(const char * end)
        {
            output += std::string(lastPos, end);
            lastPos = end;
        }

        void PP::skip(const char * begin)
        {
            copyText(begin);
            lastPos = position;
        }

        PP::Grammar::Grammar()
        {
            typedef char token_t;

            top = * no_step_back_d(//function |
                //constant |
                directive |
                skipped |
                range_p(1, 255));

            //function = lexeme_d(ident >> ch_p('(')) [& PP:checkFunction]  >>
            //           ! (paramValue % ch_p(',')) >> ch_p(')') [& PP:expandFunction];
            //
            //paramValue = lexeme_d(+ (range(1, 255) - ch_p(','))) [& PP::pushParamValue];

            //constant = token [& PP::expandConstant];

            directive = ch_p('#') [& PP::copyText]
            >> //(define |
                include
                /*ifdef |
                ifndef
                if_ |
                else_ |
                endif |
                elif |
                error |
                line |
                warning |
                pragma)*/
                ;


            skipped = lexeme_d((str_p("//") >> range_p(1, 255) * ch_p('\n') |
                str_p("/*") >> range_p(1, 255) * str_p("*/")) [& PP::skip] |
                ch_p('"') >> (ch_p('\\') >> ! ch_p('"') | range_p(1, 255)) * ch_p('"') |
                ch_p('\'') >> (ch_p('\\') >> ! ch_p('\'') | range_p(1, 255)) * ch_p('\''));

            //ident = chset_p("a-zA-Z_") >> * chset_p("a-zA-Z0-9_");

            //token = lexeme_d(ident);

            //define = str_p("define") >> (functionDef | constantDef);

            include = str_p("include") >> (ch_p('"') >> range_p(1, 255) * ch_p('"') |
                ch_p('<') >> range_p(1, 255) * ch_p('>')) [& PP::include];

            //functionDef = lexeme_d(ident [& PP::currentMacro] >> ch_p('(')) >>
            //              ! (token [& PP::pushMacroParam] % ch_p(',')) >> ch_p(')') >>
            //              value [& PP:addFunction];

            //constantDef = token [& PP::currentMacro] >> value [& PP:addConstant];

            //value = lexeme_d((ch_p('\\') >> ! endl | range(1, 255)) * endl);

            //endl = ! ch_p('\r') >> ch_p('\n');

            space = ch_p(' ') | ch_p('\t');

            //NAPALM_PARSER_NAME_RULE(function);
            //NAPALM_PARSER_NAME_RULE(constant);
            NAPALM_PARSER_NAME_RULE(directive);
            NAPALM_PARSER_NAME_RULE(skipped);
            //NAPALM_PARSER_NAME_RULE(ident);
            //NAPALM_PARSER_NAME_RULE(paramValue);
            //NAPALM_PARSER_NAME_RULE(token);
            //NAPALM_PARSER_NAME_RULE(define);
            NAPALM_PARSER_NAME_RULE(include);
            //NAPALM_PARSER_NAME_RULE(functionDef);
            //NAPALM_PARSER_NAME_RULE(constantDef);
            //NAPALM_PARSER_NAME_RULE(value);
            //NAPALM_PARSER_NAME_RULE(endl);
            NAPALM_PARSER_NAME_RULE(space);
        }
    } // Parser namespace
} // Napalm namespace

