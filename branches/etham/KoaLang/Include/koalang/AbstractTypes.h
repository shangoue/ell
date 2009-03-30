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

#ifndef __KOALANG_ABSTRACT_TYPES_H__
#define __KOALANG_ABSTRACT_TYPES_H__

#include "koalang/Defs.h"
#include "koalang/Messages.h"

namespace koalang
{
    struct Object;
    struct Function;

    struct Abstract
    {
        virtual ~Abstract() { }

        virtual const std::string what(bool enclose=false) = 0;

        /// This function is overloaded only for 3 types of nodes:
        ///   - operators are applied
        ///   - variables are resolved in given context
        ///   - Nothing raise an error
        virtual Abstract * eval(Object *) { return this; }

        /// This function is only overloaded by List
        /// Called at evaluation of a scope operator, and to run a program
        virtual Abstract * execute(Object * o) { return eval(o); }

        virtual Abstract * select(Abstract *, Object *)
        {
            raise_error(not_selectable_error + what());
        }

        virtual Abstract * assign(Abstract *, Object *)
        {
            raise_error(assign_error + what());
        }

        virtual bool test() = 0;

        virtual bool greater_or_equal(Abstract *) = 0;

#           define D(i) virtual bool is##i() { return false; }
        ABSTRACT_TYPES
        TYPES
#           undef D
    };

    inline bool equal(Abstract * a, Abstract * b)
    {
        return a->greater_or_equal(b) && b->greater_or_equal(a);
    }

    struct Iterable : public Abstract
    {
        bool isIterable() { return true; }

        struct Iterator
        {
            virtual ~Iterator() { }

            virtual Abstract * next() = 0;
        };

        virtual Iterator * iterator() = 0;

        bool greater_or_equal(Abstract * a)
        {
            if (a->isIterable())
            {
                Iterator * i1 = iterator();
                Iterator * i2 = ((Iterable *) a)->iterator();

                while (1)
                {
                    Abstract * p1 = i1->next();
                    Abstract * p2 = i2->next();

                    if (p2->isNothing()) return true;
                    if (p1->isNothing()) return false;

                    if (! p1->greater_or_equal(p2)) return false;
                    if (! p2->greater_or_equal(p1)) return true;
                }
            }

            return false;
        }

        bool test() { return size() != 0; }

        virtual int size() = 0;
    };

    /// An indexable type is not mutable
    struct Indexable : public Iterable
    {
        bool isIndexable() { return true; }

        virtual Abstract * get_element(int) = 0;
        virtual Indexable * set_element(int, Abstract *) = 0;

        virtual Indexable * get_slice(int, int) = 0;
        virtual Indexable * set_slice(int, int, Abstract *) = 0;

        void adjust_index(int & i)
        {
            if ((i = i % size()) < 0)
            {
                i = (i + size()) % size();
            }
        }

        Abstract * select(Abstract * p, Object * o);

        struct IndexableIterator : public Iterator
        {
            IndexableIterator(Indexable * i)
              : indexable(i),
                index(0)
            { }

            Abstract * next();

            Indexable * indexable;
            int index;
        };

        Iterator * iterator() { return new IndexableIterator(this); }
    };
}

#endif //__KOALANG_ABSTRACT_TYPES_H__
