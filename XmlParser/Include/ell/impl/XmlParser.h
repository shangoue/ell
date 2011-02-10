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

namespace ell
{
    inline std::string XmlGrammar::protect(const std::string & cdata)
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

    inline XmlGrammar::XmlGrammar()
    {
        document = no_look_ahead(+ ( element
                                   | comment
                                   | pi
                                   | data ) >> end [& XmlParser::on_end_of_file]);

        element = str("</") >> ident [& XmlParser::on_end_double] >> ch('>') |
                  lexeme(ch('<') >> ident [& XmlParser::element_name])
                  >> * attribute >> (str("/>") [& XmlParser::on_single] |
                                     ch('>') [& XmlParser::on_start_double]);

        attribute = ident [& XmlParser::attribute_name] >> ch('=')
                    >> lexeme(ch('\"') >> * ((+ (any - chset("\"<&"))) [& XmlParser::push_string] |
                                             reference) >> ch('\"') |
                                ch('\"') >> * ((+ (any - chset("\'<&"))) [& XmlParser::push_string] |
                                               reference) >> ch('\'')) [& XmlParser::on_attribute];

        reference = ch('&') >> (str("quot") [& XmlParser::push_quot] |
                                str("apos") [& XmlParser::push_apos] |
                                str("amp")  [& XmlParser::push_amp] |
                                str("lt")   [& XmlParser::push_lt] |
                                str("gt")   [& XmlParser::push_gt] |
                                error("unkown entity")
                                ) >> ch(';');

        comment = str("<!--") >> any * str("-->");

        pi = str("<?") >> any * str("?>");

        data = lexeme(+ ( (+ (any - (* blank >> ch('<') | ch('&')))) [& XmlParser::push_string]
                        | reference
                        | * blank >> cdata )) [& XmlParser::on_data_];

        cdata = str("<![CDATA[")
                >> lexeme(* (any - str("]]>")))[& XmlParser::push_string] >> str("]]>");

        ident = lexeme((chset("a-zA-Z_:") |
                       range<(char) 0x80, (char) 0xFF>()) >> * ( chset("a-zA-Z0-9_.:-") |
                                                                 range<(char) 0x80, (char) 0xFF>()));
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
