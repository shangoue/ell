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

#include <ell/XmlParser.h>

namespace ell
{
    XmlGrammar XmlParser::grammar;

    std::string XmlGrammar::protect(const std::string & cdata)
    {
        std::string protected_data;
        size_t pos = 0;

        while (pos < cdata.size())
        {
            size_t r = cdata.find_first_of("\"\'&<>", pos);

            if (r != std::string::npos)
            {
                protected_data += cdata.substr(pos, r - pos);

                switch (cdata[r])
                {
                case '\"': protected_data += "&quot;"; break;
                case '\'': protected_data += "&apos;"; break;
                case '&': protected_data += "&amp;"; break;
                case '<': protected_data += "&lt;"; break;
                case '>': protected_data += "&gt;"; break;
                }
                pos = r + 1;
            }
            else
            {
                protected_data += cdata.substr(pos);
                break;
            }
        }

        return protected_data;
    }

    XmlGrammar::XmlGrammar()
    {
        document = no_step_back(+ (element |
                                comment |
                                cdata |
                                pi |
                                data) >> eos [& XmlParser::on_end_of_file]);

        element = str("</") >> ident [& XmlParser::on_end_double] >> ch('>') |
                  lexeme(ch('<') >> ident [& XmlParser::element_name])
                  >> * attribute >> (str("/>") [& XmlParser::on_single] |
                                     ch('>') [& XmlParser::on_start_double]);

        attribute = ident [& XmlParser::attribute_name] >> ch('=')
                    >> lexeme(ch('\"') >> * ((+ (any - chset("\"<&"))) [& XmlParser::push_string] |
                                             reference) >> ch('\"') |
                                ch('\"') >> * ((+ (any - chset("\'<&"))) [& XmlParser::push_string] |
                                               reference) >> ch('\'')) [& XmlParser::on_attribute];

        reference = ch('&') >> (str("quot") [& XmlParser::push_char<'\"'>] |
                                str("apos") [& XmlParser::push_char<'\''>] |
                                str("amp")  [& XmlParser::push_char<'&'>] |
                                str("lt")   [& XmlParser::push_char<'<'>] |
                                str("gt")   [& XmlParser::push_char<'>'>] |
                                error("unkown entity")
                                ) >> ch(';');

        comment = str("<!--") >> any * str("-->");

        pi = str("<?") >> any * str("?>");

        cdata = str("<![CDATA[")
                >> lexeme(* (any - str("]]>"))) [& XmlParser::on_data_] >> str("]]>");

        data = lexeme(+ ((+ (any - (* blank >> chset("<&")))) [& XmlParser::push_string] |
                          reference)) [& XmlParser::on_data_];

        ident = lexeme((chset("a-zA-Z_:") |
                       range<0x80, 0xFF>()) >> * (chset("a-zA-Z0-9_.:-") |
                                                  range<0x80, 0xFF>()));
        ELL_NAME_RULE(document);
        ELL_NAME_RULE(element);
        ELL_NAME_RULE(attribute);
        ELL_NAME_RULE(reference);
        ELL_NAME_RULE(comment);
        ELL_NAME_RULE(pi);
        ELL_NAME_RULE(cdata);
        ELL_NAME_RULE(data);
        ELL_NAME_RULE(ident);
    }
}
