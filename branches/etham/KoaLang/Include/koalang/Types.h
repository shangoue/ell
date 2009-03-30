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

#ifndef __KOALANG_TYPES_H__
#define __KOALANG_TYPES_H__

#include "koalang/AbstractTypes.h"
#include "koalang/Messages.h"
#include "koalang/Dictionary.h"

namespace koalang
{
    struct Nothing : public Abstract
    {
        bool isNothing() { return true; }

        const std::string what(bool enclose=false) { return "nil"; }

        bool test() { return false; }

        bool greater_or_equal(Abstract * a) { return a->isNothing(); }

        Abstract * eval(Object *)
        {
            raise_error(eval_error + what());
        }
    };

    struct Number : public Abstract
    {
        Number(double v)
            : value(v)
        { }

        bool isNumber() { return true; }

        const std::string what(bool enclose=false);

        bool test() { return value != 0; }

        bool greater_or_equal(Abstract * a) { return a->isNumber() && value >= ((Number *) a)->value; }

        double value;
    };

    struct String : public Indexable
    {
        String(const char * begin, const char * end)
          : value(dictionary.add(begin, end))
        { }

        String(const std::string & s)
          : value(dictionary.add(s))
        { }

        bool isString() { return true; }

        const std::string what(bool enclose=false);

        bool greater_or_equal(Abstract * a) { return a->isString() && * value >= * ((String *) a)->value; }

        int size() { return value->size(); }

        Abstract * get_element(int i) { return get_slice(i, i + 1); }
        String * set_element(int i, Abstract * a) { return set_slice(i, i + 1, a); }

        String * get_slice(int, int);
        String * set_slice(int, int, Abstract *);

        const std::string * value;
    };

    struct List : public Indexable
    {
        List() { }

        /// 0 <= s, e < size() required
        List(const List &, int s, int e);

        bool isList() { return true; }

        const std::string what(bool enclose=false);

        int size() { return value.size(); }

        Abstract * get_element(int);
        List * set_element(int, Abstract *);

        List * get_slice(int, int);
        List * set_slice(int, int, Abstract *);

        /// Evaluate each contained item of the list
        Abstract * execute(Object *);

        std::vector<Abstract *> value;
    };

    struct Variable;

    struct Object : public Iterable
    {
        Object(Object * p)
          : parent(p),
            return_value(new Nothing),
            is_transparent(false),
            loops(0)
        { }

        const std::string what(bool enclose=false);

        int size() { return assignations.size(); }

        bool isObject() { return true; }

        Abstract * get_value(Variable *);

        bool exists(Variable * name);

        void assign_var(Variable * name, Abstract * value);

        /// To compare variables, we just need to compare pointers, thanks to the dictionary
        typedef std::map<const std::string *, Abstract *> AsgMap;

        struct ObjectIterator : public Iterator
        {
            ObjectIterator(Object * o)
              : i(o->assignations.begin()),
                object(o)
            { }

            Abstract * next();

            AsgMap::iterator i;
            Object * object;
        };

        Iterator * iterator() { return new ObjectIterator(this); }

        AsgMap assignations;

        Object * parent;

        Abstract * return_value;

        bool is_transparent;

        int loops;
    };

    struct Variable : public String
    {
        Variable(const char * begin, const char * end)
            : String(begin, end)
        { }

        const std::string what(bool enclose=false) { return * value; }

        bool isVariable() { return true; }

        Abstract * eval(Object * c)
        {
            return c->get_value(this);
        }

        Abstract * assign(Abstract * value, Object * c)
        {
            c->assign_var(this, value);

            return value;
        }
    };

    struct Function : public Abstract
    {
        Function(Abstract * p, Abstract * b)
          : parameters(p),
            body(b)
        { }

        bool isFunction() { return true; }

        const std::string what(bool enclose=false);

        bool test() { raise_error(test_function_error + what()); }

        bool greater_or_equal(Abstract * f) { return f == this; }

        Abstract * select(Abstract * p, Object * o);

        Abstract * parameters;
        Abstract * body;
    };
}

#endif //__KOALANG_TYPES_H__
