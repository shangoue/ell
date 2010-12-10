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

#ifndef INCLUDED_ELL_IMPL_XMLNODE_H
#define INCLUDED_ELL_IMPL_XMLNODE_H

#include <ell/XmlNode.h>
#include <ell/XmlParser.h>

#include <cassert>

namespace ell
{
    const std::string XmlNode::get_attrib(const std::string & name) const
    {
        assert(is_element());
        XmlAttributesMap::const_iterator i = attributes.find(name);
        if (i != attributes.end())
            return i->second;
        raise_error(describe() + ": no such attribute: " + name);
        return NULL;
    }

    XmlNode * XmlNode::set_attrib(const std::string & name, const std::string & value)
    {
        assert(is_element());
        attributes[name] = value;
        return this;
    }

    bool XmlNode::has_attrib(const std::string & name) const
    {
        assert(is_element());
        XmlAttributesMap::const_iterator i = attributes.find(name);
        return i != attributes.end();
    }

    XmlNode * XmlNode::check_attrib(const std::string & name, const std::string & value)
    {
        const std::string & val=get_attrib(name);
        if (val == value) return this;
        raise_error(describe() + ": " +
                    name + "=\"" + val +
                    "\", expecting \"" + value + "\"");
        return NULL;
    }

    XmlNode * XmlNode::check_attrib_present(const std::string & name)
    {
        get_attrib(name);
        return this;
    }

    const std::string & XmlNode::get_name() const
    {
        assert(is_element());
        return name;
    }

    XmlNode * XmlNode::get_name(std::string & n)
    {
        n=get_name();
        return this;
    }

    XmlNode * XmlNode::set_name(const std::string & n)
    {
        assert(data.empty());
        name=n;
        return this;
    }

    XmlNode * XmlNode::check_name(const std::string & n)
    {
        if (n == get_name())
            return this;
        raise_error(describe() + ": expecting element " + n);
        return NULL;
    }

    const std::string & XmlNode::get_data() const
    {
        assert(is_data());
        return data;
    }

    XmlNode * XmlNode::set_data(const std::string & d)
    {
        assert(is_data());
        data=d;
        return this;
    }

    XmlNode * XmlNode::check_data(const std::string & d)
    {
        if (d == get_data())
            return this;
        raise_error(describe() + ": value \"" + data + "\", expecting \"" + d + "\"");
        return NULL;
    }

    void XmlNode::dump(std::ostream & out, int indent, int shift) const
    {
        std::string indent_str(indent * shift, ' ');
        out << indent_str << describe() << std::endl;
        for (XmlNode * p = _first_child;
            p;
            p = p->_next_sibling)
        {
            p->dump(out, indent + 1, shift);
        }
    }

    void XmlNode::unparse(std::ostream & out, int indent, int shift) const
    {
        if (is_data())
        {
            out << XmlGrammar::protect(data) << '\n';
        }
        else
        {
            std::string indent_str(indent * shift, ' ');
            out << indent_str << '<' << name;

            for (XmlAttributesMap::const_iterator i=attributes.begin();
                i != attributes.end();
                ++i)
            {
                out << ' ' << i->first << "=\"" << XmlGrammar::protect(i->second) << "\"";
            }

            if (! _first_child)
            {
                out << " />\n";
            }
            else
            {
                out << ">\n";
                XmlNode * last = _first_child;
                for (XmlNode * p = _first_child;
                    p;
                    p = p->_next_sibling)
                {
                    p->unparse(out, indent + 1, shift);
                    last = p;
                }

                out << indent_str << "</" << name << ">\n";
            }
        }
    }

    XmlNode * XmlNode::next_sibling() const
    {
        if (_next_sibling)
            return _next_sibling;
        raise_error(describe() + ": no next sibling");
        return NULL;
    }

    XmlNode * XmlNode::previous_sibling() const
    {
        if (_previous_sibling)
            return _previous_sibling;
        raise_error(describe() + ": no previous sibling");
        return NULL;
    }

    XmlNode * XmlNode::first_child() const
    {
        if (_first_child)
            return _first_child;
        raise_error(describe() + ": no child");
        return NULL;
    }

    XmlNode * XmlNode::last_child() const
    {
        if (_last_child)
            return _last_child;
        raise_error(describe() + ": no child");
        return NULL;
    }

    XmlNode * XmlNode::parent() const
    {
        if (_parent) return _parent;
        raise_error(describe() + ": no parent");
        return NULL;
    }

    XmlNode * XmlNode::insert_sibling_node_before(XmlNode * p)
    {
        p->_next_sibling=this;
        p->_previous_sibling=_previous_sibling;
        p->_parent=_parent;
        if (_parent->_first_child == this)
            _parent->_first_child=p;
        else
            _previous_sibling->_next_sibling=p;
        _previous_sibling=p;
        return p;
    }

    XmlNode * XmlNode::insert_sibling_node_after(XmlNode * p)
    {
        p->_previous_sibling=this;
        p->_next_sibling=_next_sibling;
        p->_parent=_parent;
        if (_parent->_last_child == this)
            _parent->_last_child=p;
        else
            _next_sibling->_previous_sibling=p;
        _next_sibling=p;
        return p;
    }

    XmlNode * XmlNode::enqueue_child(XmlNode * p)
    {
        if (_last_child)
            _last_child->_next_sibling=p;
        else
            _first_child=p;
        p->_previous_sibling=_last_child;
        _last_child=p;
        p->_parent=this;
        return p;
    }

    XmlNode::~XmlNode()
    {
        XmlNode * sav_p, * p;
        for (p=_first_child; p; p=sav_p)
        {
            sav_p=p->_next_sibling;
            delete p;
        }
    }

    std::string XmlNode::describe() const
    {
        std::ostringstream oss;
        oss << line << ": ";
        if (is_element())
            oss << "Element `" << name << "`";
        else
            oss << "Data \"" + protect(data) + "\"";
        return oss.str();
    }

    bool XmlNode::is_equal(const XmlNode & other) const
    {
        if (name != other.name ||
            data != other.data ||
            attributes.size() != other.attributes.size())
            return false;

        for (XmlAttributesMap::const_iterator io, i = attributes.begin();
            i != attributes.end();
            ++i)
        {
            io = other.attributes.find(i->first);
            if (io == other.attributes.end() ||
                i->second != io->second)
                return false;
        }

        for (XmlNode * child = _first_child, * other_child = other._first_child;
             child || other_child;
             child = child->_next_sibling, other_child = other_child->_next_sibling)
        {
            if (! child || ! other_child)
                return false;

            bool r = child->is_equal(* other_child);

            if (! r)
                return false;
        }

        return true;
    }
}

#endif // INCLUDED_ELL_IMPL_XMLNODE_H
