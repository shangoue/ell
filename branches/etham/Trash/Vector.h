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

#ifndef __KOALANG_SYSTEM_VECTOR__
#define __KOALANG_SYSTEM_VECTOR__
#include "KoaLang/System/Exception.h"
namespace KoaLang
{
    namespace System
    {
        template <typename T, const int G=4>
        struct Vector
        {
            Vector() : s(0), allocated_size(0) { }

            Vector(const Vector & v)
            {
                s=v.s;
                allocated_size=s;
                content=new T *[allocated_size];
                memcpy(content, v.content, s * sizeof(T *));
            }

            Vector(const Vector & v1, const Vector & v2)
            {
                s=v1.s + v2.s;
                allocated_size=s;
                content=new T *[allocated_size];
                memcpy(content, v1.content, v1.s * sizeof(T *));
                memcpy(content + v1.s, v2.content, v2.s * sizeof(T *));
            }

            Vector(const Vector & v, int from, int to);

            void push_back(T * n)
            {
                if (s == allocated_size)
                {
                    allocated_size+=G;
                    T ** c=new T *[allocated_size];
                    if (s)
                    {
                        memcpy(c, content, s * sizeof(T *));
                        delete[] content;
                    }
                    content=c;
                }
                content[s++]=n;
            }

            T * back() { return content[s - 1]; }

            void pop_back()
            {
                if (! s) RaiseError(SystemError, "Pop from an empty stack");
                content[s]=NULL;    // Necessary to keep smart pointers up-to-date
                s-=1;
            }

            bool empty() const { return s == 0; }

            ~Vector() { if (allocated_size) delete[] content; }

            int check_index(int i) const
            {
                if (i < 0) i+=s;
                if (i < 0 || i >= s)
                {
                    RaiseError(SystemError, "Read beyond array boundary");
                }
                return i;
            }

            T * get(int i) { return content[check_index(i)]; }

            const T * get(int i) const { return content[check_index(i)]; }

            void set(int i, T * value)
            {
                if (i < 0) i+=s;
                if (i < 0)
                {
                    RaiseError(SystemError, "Cannot extend a list with the negative notation");
                }
                if (i >= allocated_size)
                {
                    allocated_size=i + G;
                    T ** c=new T *[allocated_size];
                    if (s)
                    {
                        memcpy(c, content, s * sizeof(T *));
                        delete[] content;
                    }
                    content=c;
                    for (int j=s; j < i; j++)
                    {
                        content[j]=NULL;
                    }
                }
                content[i]=value;
            }

            int size() const { return s; }

            T ** content;

            int s, allocated_size;
        };
    } // System namespace
} // KoaLang namespace
#endif // __KOALANG_SYSTEM_VECTOR__
