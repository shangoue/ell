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

namespace koalang
{
    struct Map;

    struct Object
    {
        virtual ~Object() { }

        virtual Object * eval(Map * context) = 0;

        virtual void describe(std::ostream & os) const = 0;

        template <typename Type>
        bool is()
        {
            return dynamic_cast<Type *>(this);
        }

        template <typename Type>
        Type * to()
        {
            if (is<Type>())
                return (Type *) this;
            throw std::runtime_error("Type error");
        }

        friend std::ostream & operator << (std::ostream & os, const Object & obj)
        {
            obj.describe(os);
            return os;
        }

        //TODO: add location information for back-trace
    };

    typedef std::vector<Object *> ObjectList;
    inline std::ostream & operator << (std::ostream & os, const ObjectList & l)
    {
        for (ObjectList::const_iterator i = l.begin(); i != l.end(); ++i)
            os << ' ' << **i << ' ';
        return os;
    }

    typedef std::map<std::string, Object *> ObjectMap;
    inline std::ostream & operator << (std::ostream & os, const ObjectMap & l)
    {
        for (ObjectMap::const_iterator i = l.begin(); i != l.end(); ++i)
            os << ' ' << i->first << " = " << *i->second << ' ';
        return os;
    }

    struct Real : public Object
    {
        Real(double value)
          : value(value) 
        { }

        void describe(std::ostream & os) const
        {
            os << value;
        }

        Real * eval(Map * context)
        {
            return this;
        }

        double value;
    };

    struct String : public Object
    {
        String(const std::string & value)
          : value(value)
        { }

        void describe(std::ostream & os) const
        {
            os << '"' << value << '"';
        }

        Object * eval(Map * context)
        {
            return this;
        }

        std::string value;
    };

    struct List : public Object
    {
        void describe(std::ostream & os) const
        {
            os << '(' << value << ')';
        }

        Object * eval(Map * context);

        ObjectList value;
    };

    struct Map : public Object
    {
        Map(Map * parent = 0)
          : parent(parent)
        { }

        Object * look_up(const std::string & name) const
        {
            ObjectMap::const_iterator i = value.find(name);
            if (i == value.end())
            {
                if (parent)
                    return parent->look_up(name);
                else
                    throw std::runtime_error("No variable named " + name);
            }
            return i->second;
        }

        Map * eval(Map * context) { return this; }

        void describe(std::ostream & os) const { os << '{' << value << '}'; }

        ObjectMap value;
        Map * parent;
    };

    struct Function : public Object
    {
        Object * eval(Map * context) { return body->eval(context); }

        void describe(std::ostream & os) const
        {
            os << "( <" << left << "><" << right << "> -> " << * body << " )";
        }

        ObjectList left, right;
        Object * body;
    };

    struct Variable : public String
    {
        Variable(const std::string & name) : String(name) { }

        void describe(std::ostream & os) const { os << value; }

        Object * eval(Map * context)
        {
            Object * v = context->look_up(value);
            if (v->is<Function>())
            {
                Function * f = (Function *) v;
                if (not f->left.empty() or not f->right.empty())
                    throw std::runtime_error("Function called without parameters");

                return f->eval(context);
            }
            else
                return v;
        }
    };

    struct Block : public List
    {
        Map * eval(Map * context)
        {
            Map * locals = new Map(context);
            List::eval(locals);
            return locals;
        }

        void describe(std::ostream & os) const { os << '{' << value << '}'; }
    };
}

#endif //__KOALANG_TYPES_H__
