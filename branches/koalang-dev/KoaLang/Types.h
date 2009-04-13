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
    struct Object
    {
        virtual ~Object() { }

        virtual const Object * eval() const { return this; }

        virtual void describe(std::ostream & os) const = 0;

        template <typename Type>
        bool is() { return dynamic_cast<Type *>(this); }

        friend std::ostream & operator << (std::ostream & os, const Object & obj)
        {
            obj.describe(os);
            return os;
        }
    };

    struct Real : public Object
    {
        Real(double v)
          : value(v)
        { }

        void describe(std::ostream & os) const { os << value; }

        double value;
    };

    struct String : public Object
    {
        String(const std::string & s)
          : value(s)
        { }

        void describe(std::ostream & os) const { os << value; }

        const std::string value;
    };

    struct Variable : public String
    {
        Variable(const std::string & s)
          : String(s)
        { }
    };

    struct List : public Object
    {
        void describe(std::ostream & os, const char * braces = "()") const
        {
            os << braces[0];
            for (std::vector<Object *>::const_iterator i = children.begin();
                 i != children.end();
                 ++i)
                os << ' ' << **i << ' ';
            os << braces[1];
        }
    };

    struct Block : public List
    {
        void describe(std::ostream & os) const { List::describe(os, "{}"); }
    };

    struct Map : public Object
    {
        std::map<std::string, Object *> value;
    };
}

#endif //__KOALANG_TYPES_H__
