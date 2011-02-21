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

#ifndef INCLUDED_ELL_XMLPARSER_H
#define INCLUDED_ELL_XMLPARSER_H

#include <stack>

#include <ell/Grammar.h>
#include <ell/Parser.h>

#include <ell/XmlNode.h>

namespace ell
{
    /// Basic XML Grammar.
    ///
    /// A fully reentrant grammar, used to parse XML buffers
    ///
    /// Important notice:
    ///   - DTDs are completely ignored.
    ///   - a document with several root elements will not be rejected
    ///   - white spaces are skipped, which has two effects:
    ///      * DOM is lighter, no white space data nodes generating
    ///      * Leading and trailing spaces are trimmed in DOM data nodes
    ///
    /// @see http://www.w3.org/TR/REC-xml/
    struct XmlGrammar : public Grammar<char>
    {
        /// Build the grammar
        XmlGrammar();

        /// Protect the given string using standard XML entities
        static std::string protect(const std::string & cdata);

        /// Grammar rules
        Rule<char> document, element, attribute, reference, comment, pi, cdata, data, ident;

    private:
        XmlGrammar(const XmlGrammar &);
        void operator=(const XmlGrammar &);
    };

    struct XmlParser : public Parser<char>
    {
        typedef Parser<char> base_type;

        XmlParser(XmlGrammar & grammar)
          : Parser<char>(& grammar.document, & grammar.blank)
        { }

        //@{
        /// SAX API
        virtual void on_start_element(const std::string & name, const XmlAttributesMap & attrs) = 0;
        virtual void on_end_element(const std::string & name) = 0;
        virtual void on_data(const std::string & data) = 0;
        //@}

    private:
        friend struct XmlGrammar;

        void on_data_()
        {
            on_data(cdata);
            cdata.clear();
        }

        void on_start_double()
        { 
            elements.push(element_name);
            on_start_element(element_name, attributes);
            attributes.clear();
        }

        void on_single()
        {
            on_start_element(element_name, attributes);
            attributes.clear();
            on_end_element(element_name);
        }

        void on_end_double(const std::string & name)
        {
            if (elements.empty())
                raise_error("Unexpected end of element `" + name + "`");
            const std::string & last = elements.top();
            if (name != last)
                raise_error("End of element `" + last + "` expected instead of `" + name + "`");

            on_end_element(name);
            elements.pop();
        }

        void on_attribute()
        {
            attributes[attribute_name] = cdata;
            cdata.clear();
        }

        void on_end_of_file()
        {
            if (not elements.empty())
                raise_error("Unclosed element: `" + elements.top() + "`");
        }

        void push_amp() { cdata += '&'; }
        void push_apos() { cdata += '\''; }
        void push_quot() { cdata += '\"'; }
        void push_lt() { cdata += '<'; }
        void push_gt() { cdata += '>'; }

        void push_string(const std::string & s) { cdata += s; }

        XmlAttributesMap attributes;
        std::stack<std::string> elements;
        std::string element_name;
        std::string attribute_name;
        std::string cdata;
    };

    struct XmlDomParser : public XmlParser
    {
        XmlDomParser(XmlGrammar & grammar)
          : XmlParser(grammar),
            document(),
            current(& document)
        {
            document.parser = this;
        }

        /// Document node is not the XML root element
        /// It could also contain DOCTYPE, etc.
        XmlNode * get_root() { return document.first_child(); }

        XmlNode document;
        XmlNode * current;

        void on_data(const std::string & data)
        {
            ELL_DUMP("Enqueue data `" + data + '`');
            current->enqueue_child(new XmlNode(this, line_number))->data = data;
        }

        void on_start_element(const std::string & name, const XmlAttributesMap & attrs)
        {
            ELL_DUMP("Enqueue element `" + name + '`');
            current = current->enqueue_child(new XmlNode(this, line_number));
            current->name = name;
            current->attributes = attrs;
        }

        void on_end_element(const std::string &)
        {
            current = current->parent();
        }
    };
}

#include <ell/impl/XmlParser.h>
#include <ell/impl/XmlNode.h>

#endif // INCLUDED_ELL_XMLPARSER_H
