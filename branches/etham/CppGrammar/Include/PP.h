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

#ifndef __NAPALM_PARSER_PP_H__
#define __NAPALM_PARSER_PP_H__

#include "Napalm/Parser/Parser.h"
#include "Napalm/Parser/Rule.h"

namespace Napalm
{
    namespace Parser
    {
        typedef bool (* PPCallback)(const std::string & ressourceId, std::string & file_content);

        class PP : public Parser<char>
        {
        public:
            PP(PPCallback fileSystem_,  std::string & output_);

            void preprocess(const std::string & ressourceId_);

        private:
            std::string ressourceId;
            const char * lastPos;
            PPCallback fileSystem;
            std::string & output;

            void printAnchor();
            const char * get_filename();
            void include(const char * begin);
            void copyText(const char * end);
            void skip(const char * begin);

            //bool checkFunction(const char * begin);
            //void pushParamValue(const char * begin);
            //void expandFunction(const char * begin);
            //void expandConstant(const char * begin);
            //void expand();
            //void pushMacroValue(const char * begin);
            //void newMacro(const char * begin);
            //void pushMacroParam(const char * begin);

            //struct Macro
            //{
            //    std::vector<std::string> params;
            //    std::string value;
            //};

            //struct MacroOnExpansion
            //{
            //    MacroMap::iterator macro;
            //    std::vector<std::string> params_value;
            //};

            //typedef std::map<std::string, Macro> MacroMap;

            //MacroMap macros;
            //std::vector<MacroOnExpansion> onExpansion;

            struct Grammar
            {
                Grammar();

                Rule<char> top, directive, skipped, include, space;
                //Rule<char> function, constant;
                //Rule<char> ident, paramValue, token;
                //Rule<char> define, functionDef, constantDef;
                //Rule<char> value, endl;
            };

            static Grammar grammar_;
        };
    } // Parser namespace
} // Napalm namespace

#endif // __NAPALM_PARSER_PP_H__

