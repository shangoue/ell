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

#ifndef INCLUDED_PARSER_XMLNODE_H
#define INCLUDED_PARSER_XMLNODE_H

#include <ell/Parser.h>

namespace ell
{
    template <typename Token>
    class Parser;

    typedef std::map<std::string, std::string> XmlAttributesMap;

    struct XmlNode
    {
        XmlNode(Parser<char> * _parser, int _line = 0)
          : _next_sibling(NULL), _previous_sibling(NULL),
            _first_child(NULL), _last_child(NULL),
            _parent(NULL), line(_line), parser(_parser)
        { }

        /// Destruction with children nodes deletion
        virtual ~XmlNode();

        bool is_element() const { return name.size() != 0; }
        bool is_data() const { return name.empty(); }

        //@{
        /// Attribute handling, raise error if attribute does not exist
        const std::string get_attrib(const std::string & name) const;

        template <typename T>
        T get_attrib(const std::string & name) const;

        template <typename T>
        XmlNode * get_attrib(const std::string & name, T & value);

        XmlNode * check_attrib(const std::string & name, const std::string & value);
        XmlNode * check_attrib_present(const std::string & name);
        //@}

        template <typename T>
        XmlNode * get_attrib_if_present(const std::string & name, T & value);

        XmlNode * set_attrib(const std::string & name, const std::string & value);

        bool has_attrib(const std::string & name) const;

        //@{
        /// Element name handling, raise error if not an element
        const std::string & get_name() const;
        XmlNode * get_name(std::string & name);
        XmlNode * set_name(const std::string & name);
        XmlNode * check_name(const std::string & name);
        //@}

        //@{
        /// Data handling, raise error if it is an element
        const std::string & get_data() const;

        template <typename T>
        XmlNode * get_data(T & data);

        XmlNode * set_data(const std::string & data);
        XmlNode * check_data(const std::string & data);
        //@}

        /// Recursive write of resulting XML
        void unparse(std::ostream & out, int indent=0, int shift=1) const;

        /// Dump a visual representation of the DOM tree
        void dump(std::ostream & out, int indent=0, int shift=1) const;

        //@{
        /// Tree walking
        /// If the targetted node does not exist, raise an error
        XmlNode * next_sibling() const;
        XmlNode * previous_sibling() const;
        XmlNode * first_child() const;
        XmlNode * last_child() const;
        XmlNode * parent() const;
        //@}

        //@{
        /// Node insertion
        /// Return the address of the just-inserted node
        XmlNode * insert_sibling_node_before(XmlNode *);
        XmlNode * insert_sibling_node_after(XmlNode *);
        XmlNode * enqueue_child(XmlNode *);
        //@}

        XmlAttributesMap attributes;

        std::string name;
        std::string data;

        XmlNode * _next_sibling, * _previous_sibling;
        XmlNode * _first_child, * _last_child;
        XmlNode * _parent;

        int line;
        Parser<char> * parser;

        std::string describe() const;

    private:
        /// Forbidden
        XmlNode(const XmlNode &);
    };

    namespace
    {
        template <typename T>
        void _get_attrib(const XmlNode & n, const std::string & name, T & value)
        {
            std::istringstream i(n.get_attrib(name));
            if (! (i >> value))
                n.parser->raise_error("Wrong type for attribute " + name, n.line);
        }

        template <>
        inline void _get_attrib<std::string>(const XmlNode & n, const std::string & name, std::string & value)
        {
            value = n.get_attrib(name);
        }
    }

    template <typename T>
    T XmlNode::get_attrib(const std::string & name) const
    {
        T value;
        _get_attrib<T>(* this, name, value);
        return value;
    }

    template <typename T>
    XmlNode * XmlNode::get_attrib(const std::string & name, T & value)
    {
        _get_attrib<T>(* this, name, value);
        return this;
    }

    template <typename T>
    XmlNode * XmlNode::get_attrib_if_present(const std::string & name, T & value)
    {
        if (has_attrib(name)) _get_attrib(* this, name, value);
        return this;
    }

    namespace
    {
        template <typename T>
        void _get_data(XmlNode & n, T & value)
        {
            std::istringstream i(n.get_data());
            if (! (i >> value))
                n.parser->raise_error("Wrong type for data " + n.get_data(), n.line);
        }

        template <>
        inline void _get_data<std::string>(XmlNode & n, std::string & d)
        {
            d=n.get_data();
        }
    }

    template <typename T>
    XmlNode * XmlNode::get_data(T & value)
    {
        _get_data(* this, value);
        return this;
    }
}

#endif // INCLUDED_PARSER_NODE_H
