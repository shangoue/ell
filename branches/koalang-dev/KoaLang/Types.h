#ifndef __KOALANG_TYPES_H__
#define __KOALANG_TYPES_H__

#include "Location.h"

namespace koalang
{
    struct Map;

    struct Object
    {
        Object()
          : location(0)
        { }

        virtual ~Object() { }

        // Evaluation with backtrace update
        Object * eval(Map * context);

        virtual Object * concrete_eval(Map * context) = 0;

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

        Location * location;
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

        Real * concrete_eval(Map * context)
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

        Object * concrete_eval(Map * context)
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

        Object * concrete_eval(Map * context);

        ObjectList value;
    };


    struct Map : public Object
    {
        Map(Map * parent = 0)
          : parent(parent)
        {
            if (parent)
                backtrace = parent->backtrace;
            else
                backtrace = new ObjectList;
        }

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

        Map * concrete_eval(Map * context) { return this; }

        void describe(std::ostream & os) const { os << '{' << value << '}'; }

        ObjectMap value;
        Map * parent;
        ObjectList * backtrace;
    };


    struct Function : public Object
    {
        Object * concrete_eval(Map * context) { return body->eval(context); }

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

        Object * concrete_eval(Map * context)
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
        Map * concrete_eval(Map * context)
        {
            Map * locals = new Map(context);
            List::concrete_eval(locals);
            return locals;
        }

        void describe(std::ostream & os) const { os << '{' << value << '}'; }
    };
}

#endif //__KOALANG_TYPES_H__
