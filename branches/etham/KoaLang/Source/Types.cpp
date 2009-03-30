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

#include "koalang/Types.h"

namespace koalang
{
    const std::string Number::what(bool enclose)
    {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

    const std::string String::what(bool enclose)
    {
        return '"' + System::protect(* value) + '"';
    }

    String * String::get_slice(int s, int e)
    {
        adjust_index(s);
        adjust_index(e);

        return new String(value->data() + s, value->data() + e);
    }

    String * String::set_slice(int start, int end, Abstract * v)
    {
        adjust_index(start);
        adjust_index(end);

        std::string newstr(value->data(), start);

        if (v->isString()) newstr += * ((String *) v)->value;
        else if (! v->isNothing()) newstr += v->what();

        return new String(newstr + std::string(value->data() + end, value->size() - end));
    }

    List::List(const List & l, int s, int e)
      : value(l.value.begin() + s, l.value.begin() + e)
    { }

    const std::string List::what(bool enclose)
    {
        std::string s;

        for (int i=0; i < size(); i++)
        {
            s+=' ' + value[i]->what() + ' ';
        }

        if (enclose || size() < 2) return '(' + s + ')';
        return s;
    }

    Abstract * List::execute(Object * o)
    {
        Abstract * r;
        for (unsigned int i = 0; i < value.size(); ++i)
        {
            r = value[i]->eval(o);
        }

        return r;
    }

    Abstract * List::get_element(int i)
    {
        adjust_index(i);

        return value[i];
    }

    List * List::get_slice(int s, int e)
    {
        adjust_index(s);
        adjust_index(e);

        return new List(* this, s, e);
    }

    List * List::set_element(int i, Abstract * v)
    {
        adjust_index(i);

        List * l = new List(*this, 0, value.size());

        if (v->isNothing())
        {
            l->value.erase(l->value.begin() + i);
        }
        else
        {
            l->value[i] = v;
        }

        return l;
    }

    List * List::set_slice(int s, int e, Abstract * v)
    {
        adjust_index(s);
        adjust_index(e);

        List * newlist = new List(*this, 0, value.size());

        newlist->value.erase(value.begin() + s, value.begin() + e);

        if (v->isList())
        {
            List * l = (List *) v;

            newlist->value.insert(value.begin() + s + 1, l->value.begin(), l->value.end());
        }
        else
        {
            if (! v->isNothing()) newlist->value.insert(value.begin() + s + 1, v);
        }

        return newlist;
    }

    const std::string Object::what(bool)
    {
        std::string s;

        for (AsgMap::iterator i = assignations.begin();
             i != assignations.end();
             ++i)
        {
            s+=' ' + * i->first + " = " + i->second->what() + ' ';
        }

        return "{ " + s + " }";
    }

    Abstract * Object::get_value(Variable * name)
    {
        AsgMap::iterator i = assignations.find(name->value);

        if (i == assignations.end())
        {
            if (parent)
            {
                return parent->get_value(name);
            }

            raise_error(undef_var_error << name->what());
        }

        return i->second;
    }

    bool Object::exists(Variable * name)
    {
        AsgMap::iterator i = assignations.find(name->value);

        if (i == assignations.end())
        {
            if (parent) return parent->exists(name);
            return false;
        }

        return true;
    }

    void Object::assign_var(Variable * name, Abstract * value)
    {
        if (is_transparent)
        {
            parent->assign_var(name, value);
        }
        else
        {
            assignations[name->value]=value;
        }
    }

    Abstract * Object::ObjectIterator::next()
    {
        if (i == object->assignations.end())
        {
            return new Nothing;
        }

        return (i++)->second;
    }

    const std::string Function::what(bool enclose)
    {
        std::string s=parameters->what() + " : " + body->what();

        if (enclose) return '(' + s + ')';

        return s;
    }

    Abstract * Function::select(Abstract * p, Object * o)
    {
        Object transparent(o);

        parameters->assign(p, & transparent);
        transparent.is_transparent=true;

        return body->eval(& transparent);
    }
}
