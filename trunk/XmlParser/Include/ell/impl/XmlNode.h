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

#include <cassert>

namespace ell
{
    inline XmlNode * XmlNode::remove_attrib(const std::string & name)
    {
        XmlAttributesMap::iterator i = attributes.find(name);
        if (i == attributes.end())
            raise_error(describe() + ": no such attribute: " + name);
        attributes.erase(i);
        return this;
    }

    inline const std::string XmlNode::get_attrib(const std::string & name) const
    {
        assert(is_element());
        XmlAttributesMap::const_iterator i = attributes.find(name);
        if (i == attributes.end())
            raise_error(describe() + ": no such attribute: " + name);
        return i->second;
    }

    inline XmlNode * XmlNode::set_attrib(const std::string & name, const std::string & value)
    {
        assert(is_element());
        attributes[name] = value;
        return this;
    }

    inline bool XmlNode::has_attrib(const std::string & name) const
    {
        assert(is_element());
        XmlAttributesMap::const_iterator i = attributes.find(name);
        return i != attributes.end();
    }

    inline XmlNode * XmlNode::check_attrib(const std::string & name, const std::string & value)
    {
        const std::string & val=get_attrib(name);
        if (val != value)
            raise_error(describe() + ": " +
                        name + "=\"" + val +
                        "\", expecting \"" + value + "\"");
        return this;
    }

    inline XmlNode * XmlNode::check_attrib_present(const std::string & name)
    {
        get_attrib(name);
        return this;
    }

    inline const std::string & XmlNode::get_name() const
    {
        assert(is_element());
        return name;
    }

    inline XmlNode * XmlNode::get_name(std::string & n)
    {
        n=get_name();
        return this;
    }

    inline XmlNode * XmlNode::set_name(const std::string & n)
    {
        assert(data.empty());
        name=n;
        return this;
    }

    inline XmlNode * XmlNode::check_name(const std::string & n)
    {
        if (n != get_name())
            raise_error(describe() + ": expecting element " + n);
        return this;
    }

    inline const std::string & XmlNode::get_data() const
    {
        assert(is_data());
        return data;
    }

    inline XmlNode * XmlNode::set_data(const std::string & d)
    {
        assert(is_data());
        data=d;
        return this;
    }

    inline XmlNode * XmlNode::check_data(const std::string & d)
    {
        if (d != get_data())
            raise_error(describe() + ": value \"" + data + "\", expecting \"" + d + "\"");
        return this;
    }

    inline void XmlNode::dump(std::ostream & out, int indent, int shift) const
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

    inline void XmlNode::unparse(std::ostream & out, int indent, int shift) const
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
                for (XmlNode * p = _first_child;
                    p;
                    p = p->_next_sibling)
                {
                    p->unparse(out, indent + 1, shift);
                }

                out << indent_str << "</" << name << ">\n";
            }
        }
    }

    inline XmlNode * XmlNode::next_sibling() const
    {
        if (! _next_sibling)
            raise_error(describe() + ": no next sibling");
        return _next_sibling;
    }

    inline XmlNode * XmlNode::previous_sibling() const
    {
        if (! _previous_sibling)
            raise_error(describe() + ": no previous sibling");
        return _previous_sibling;
    }

    inline XmlNode * XmlNode::first_child() const
    {
        if (! _first_child)
            raise_error(describe() + ": no child");
        return _first_child;
    }

    inline XmlNode * XmlNode::last_child() const
    {
        if (! _last_child)
            raise_error(describe() + ": no child");
        return _last_child;
    }

    inline XmlNode * XmlNode::parent() const
    {
        if (! _parent)
            raise_error(describe() + ": no parent");
        return _parent;
    }

    inline XmlNode * XmlNode::insert_sibling_node_before(XmlNode * p)
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

    inline XmlNode * XmlNode::insert_sibling_node_after(XmlNode * p)
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

    inline XmlNode * XmlNode::enqueue_child(XmlNode * p)
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

    inline XmlNode * XmlNode::detach()
    {
        if (_previous_sibling)
        {
            _previous_sibling->_next_sibling = _next_sibling;
        }
        else if (_parent)
        {
            _parent->_first_child = _next_sibling;
        }

        if (_next_sibling)
        {
            _next_sibling->_previous_sibling = _previous_sibling;
        }
        else if (_parent)
        {
            _parent->_last_child = _previous_sibling;
        }

        _previous_sibling = _next_sibling = 0;
        _parent = 0;
        return this;
    }

    inline void XmlNode::delete_children()
    {
        XmlNode * sav_p, * p;
        for (p=_first_child; p; p=sav_p)
        {
            sav_p=p->_next_sibling;
            delete p;
        }
    }

    inline std::string XmlNode::describe() const
    {
        std::ostringstream oss;
        if (line)
            oss << line << ": ";
        if (is_element())
        {
            oss << "Element `" << name << "`";
            for (XmlAttributesMap::const_iterator io, i = attributes.begin();
                i != attributes.end();
                ++i)
            {
                oss << " " << i->first << "=" << i->second;
            }
        }
        else
            oss << "Data \"" + protect(data) + "\"";
        return oss.str();
    }

    inline bool XmlNode::is_equal(const XmlNode & other) const
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

    inline void XmlNode::get_text(std::string & s) const
    {
        s.clear();
        const char * sep = "";
        for (XmlNode * p = _first_child;
            p;
            p = p->_next_sibling)
        {
            if (p->is_data())
            {
                s += sep + p->get_data();
                sep = " ";
            }
        }
    }

    inline XmlIterator & XmlIterator::operator ++ ()
    {
        current=current->_next_sibling;
        return *this;
    }

    inline XmlIterator & XmlIterator::operator -- ()
    {
        current=current->_previous_sibling;
        return *this;
    }

    inline XmlIterator XmlIterator::operator ++ (int)
    {
        XmlIterator it(* this);
        ++(* this);
        return it;
    }

    inline XmlIterator XmlIterator::operator -- (int)
    {
        XmlIterator it(* this);
        --(* this);
        return it;
    }

    inline XmlIterator XmlIterator::operator + (int inc) const
    {
        XmlIterator it(*this);
        for (int i = 0; i < inc; i++)
            ++it;
        return it;
    }

    inline XmlIterator XmlIterator::operator - (int dec) const
    {
        XmlIterator it(*this);
        for (int i = 0; i < dec; i++)
            --it;
        return it;
    }
}

#endif // INCLUDED_ELL_IMPL_XMLNODE_H
