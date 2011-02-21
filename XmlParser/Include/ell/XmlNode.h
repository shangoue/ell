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

#ifndef INCLUDED_ELL_XMLNODE_H
#define INCLUDED_ELL_XMLNODE_H

#include <map>

namespace ell
{
    template <typename Token>
    struct Parser;

    typedef std::map<std::string, std::string> XmlAttributesMap;

    struct XmlNode;

    /// Iterator through XmlNode children
    /// Both normal and reverse iterator
    /// (Warning: do not use first() or last() as exit condition!)
    struct XmlIterator
    {
        explicit XmlIterator(XmlNode * pos = 0)
          : current(pos)
        { }

        XmlNode * operator * () { return current; }
        operator bool () const { return current != 0; }

        XmlIterator & operator ++ ();
        XmlIterator & operator -- ();

        XmlIterator operator ++ (int);
        XmlIterator operator -- (int);

        XmlIterator operator + (int inc) const;
        XmlIterator operator - (int dec) const;

    private:
        XmlNode * current;
    };

    struct XmlNode
    {
        XmlNode(Parser<char> * _parser = 0, int _line = 0)
          : _next_sibling(NULL), _previous_sibling(NULL),
            _first_child(NULL), _last_child(NULL),
            _parent(NULL), line(_line), parser(_parser)
        { }

        /// Destruction with children nodes deletion
        virtual ~XmlNode() { delete_children(); }

        //@{
        /// Kind of node enquirement
        bool is_element() const { return name.size() != 0; }
        bool is_data() const { return name.empty(); }
        //@}

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

        /// Set output parameter 'value' only if attribute exists
        template <typename T>
        XmlNode * get_attrib_if_present(const std::string & name, T & value);

        //@{
        /// Set attribute value
        XmlNode * set_attrib(const std::string & name, const std::string & value);

        template <typename T>
        XmlNode * set_attrib(const std::string & name, const T & value);

        XmlNode * remove_attrib(const std::string & name);
        //@}

        /// Enquire about the existence of an attribute
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

        /// Concatenation of children data nodes,
        /// raise error if not an element
        void get_text(std::string & s) const;

        //@{
        /// Recursive write of resulting XML
        void unparse(std::ostream & out, int indent=0, int shift=1) const;

        friend std::ostream & operator << (std::ostream & os, const XmlNode & node)
        {
            node.unparse(os);
            return os;
        }
        //@}

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

        XmlIterator first() { return XmlIterator(_first_child); }
        XmlIterator last() { return XmlIterator(_last_child); }

        //@{
        /// Node insertion
        /// Return the address of the just-inserted node
        XmlNode * insert_sibling_node_before(XmlNode *);
        XmlNode * insert_sibling_node_after(XmlNode *);
        XmlNode * enqueue_child(XmlNode * node = new XmlNode);
        //@}

        /// Remove this node from the DOM and transfer the ownership to caller
        XmlNode * detach();

        /// Delete all children of this node
        void delete_children();

        /// Attribute map
        XmlAttributesMap attributes;

        /// Name of the node if this is an element
        std::string name;

        /// Text of the node if this is a data node
        std::string data;

        /// Textual representation of the tree starting at this node
        std::string describe() const;

        /// Nodes recursive comparison
        bool is_equal(const XmlNode & other) const;

        /// Tell the parser to raise a syntax error on the given node
        void raise_error(const std::string & msg) const
        {
            parser->line_number = line;
            parser->raise_error(msg);
        }

        //@{
        /// Links to sibbling, children and parent nodes
        /// Null if does not exist
        /// You can use them to iterate inside a loop
        XmlNode * _next_sibling, * _previous_sibling;
        XmlNode * _first_child, * _last_child;
        XmlNode * _parent;
        //@}

        /// Line information in the original file
        int line;

        /// Reference to the parser which created this DOM
        Parser<char> * parser;

        /// Forbidden
        XmlNode(const XmlNode &);
    };

    /// XML DOM visitor
    struct XmlVisitor
    {
        void process(XmlNode * node)
        {
            enterNode(node);
            for (XmlIterator i = node->first(); i; ++i)
            {
                process(* i);
            }
            leaveNode(node);
        }

        virtual void enterNode(XmlNode *) { }
        virtual void leaveNode(XmlNode *) { }
    };

    /// XML DOM visitor using callback
    template <typename T>
    struct XmlCallbackVisitor : public XmlVisitor
    {
        typedef void (T::*Callback)(XmlNode *); 

        XmlCallbackVisitor(T * caller, Callback enter_callback = 0, Callback leave_callback = 0)
          : caller(caller),
            ec(enter_callback),
            lc(leave_callback)
        { }

        virtual void enterNode(XmlNode * n) { if (ec) (caller->*ec)(n); }
        virtual void leaveNode(XmlNode * n) { if (lc) (caller->*lc)(n); }

        T * caller;
        Callback ec, lc;
    };

    namespace
    {
        template <typename T>
        void _get_attrib(const XmlNode & n, const std::string & name, T & value)
        {
            std::istringstream i(n.get_attrib(name));
            if (! (i >> value))
                n.raise_error("Wrong type for attribute " + name);
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
        void _set_attrib(XmlNode & n, const std::string & name, const T & value)
        {
            std::ostringstream oss;
            oss << value;
            n.set_attrib(name, oss.str());
        }

        template <>
        void _set_attrib<std::string>(XmlNode & n, const std::string & name, const std::string & value)
        {
            n.set_attrib(name, value);
        }
    }

    template <typename T>
    XmlNode * XmlNode::set_attrib(const std::string & name, const T & value)
    {
        _set_attrib<T>(* this, name, value);
        return this;
    }

    namespace
    {
        template <typename T>
        void _get_data(XmlNode & n, T & value)
        {
            std::istringstream i(n.get_data());
            if (! (i >> value))
                n.raise_error("Wrong type for data " + n.get_data());
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

#endif // INCLUDED_ELL_NODE_H
