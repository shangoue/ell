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

#ifdef INCLUDE_PARSER_PRIVATE_HEADER
#ifndef INCLUDED_PARSER_NEDL_H
#include "Parser.h"
#include "Grammar.h"

class Indent
{
public:
    Indent()
    {
        root = * bloc;

        bloc = indent[& NEDL::get_level] >> statement >> * (indent[& NEDL::check_level] >> statement);

        indent = lexeme_d(ch_p('\n') >> (* ch_p(' ') | * ch_p('\t')) [indent]);

        skipper = chset_p(" \r\n\t");
    }

protected:
    Rule statement;

private:
    Rule root, bloc, indent, skipper;

    std::string indent;

    int level;

    void get_level()
    {
        level = indent.size();
    }

    bool check_level()
    {
        return level == indent.size();
    }
};

class NEDL : private Indent
{
public:
    NEDL()
    {
        statement = element_def | properties_def;

        element_def = ident >> ! cssid >> ! cssclass >> ! properties_group >> ch_p(':') >> bloc;

        properties_def = (ident | cssid | cssclass) >> * (! ch_p(',') >> (ident | cssid | cssclass)) >> properties_group;

        properties_group = '(' >> * (ident >> '=' >> attribute_value) >> ')';

        cssid = '#' >> ident;

        cssclass = '.' >> ident;

        ident = lexeme_d((alpha_p | (any_p - range_p(0, 127))) >> * (alnum_p | (any_p - range_p(0, 127))));

        attribute_value = lexeme_d(ch_p('"') >> (str_p("\\\"") | any_p) * ch_p('"'));
    }

private:
    Rule element_def, properties_def, properties_group, cssid, cssclass, attribute_value;
};
#endif // INCLUDED_PARSER_NEDL_H
#endif // INCLUDE_PARSER_PRIVATE_HEADER
