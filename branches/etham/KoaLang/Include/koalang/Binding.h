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

#ifndef __KOALANG_BINDING_H__
#ifndef NUM
#   define NUM 0
#   define COMMA
#   define LST(mac)
#   define _A(n) T##n
#   define _T(n) typename _A(n)
#   define _C(n) get_cpp_param<_A(n)>(n, c)
namespace koalang
{
    template <typename T>
    struct CppFunc;
#   include "koalang/Binding.h"
}
#   undef NUM
#   undef COMMA
#   undef _A
#   undef _T
#   undef _C
#   define __KOALANG_BINDING_H__
#else
#   define ARGS LST(_A)
#   define TPL COMMA LST(_T)
#   define CALL LST(_C)

    template <typename R TPL>
    struct CppFunc<R(ARGS)> : public BaseNode<CPP_FUNC>
    {
        CppFunc(R (* f)(ARGS)) : function(f) { }

        R (* function)(ARGS);

        List * parameters;

        AbstractNode * run(Context * c)
        {
            if (NUM != ((List *) parameters)->size())
            {
                raise_error(param_nb_error + what());
            }

            return cpp2koa(function(CALL));
        }

        template <typename T>
        T get_cpp_param(int n, Context * c)
        {
            int type;
            AbstractNode * p=((* params)[n])->eval(c);
#           define DEF(t) case t: return koa2cpp<t, T>(p);
            switch (p->type()) { BASE_TYPES }
#           undef DEF
            raise_error(internal_error);
        }
    };

#   undef LST
#   undef ARGS
#   undef TPL
#   undef CALL

#if NUM < 6
#   if NUM == 5
#       undef NUM
#       define NUM 6
#       define LST(mac) mac(0), mac(1), mac(2), mac(3), mac(4), mac(5)
#   endif
#   if NUM == 4
#       undef NUM
#       define NUM 5
#       define LST(mac) mac(0), mac(1), mac(2), mac(3), mac(4)
#   endif
#   if NUM == 3
#       undef NUM
#       define NUM 4
#       define LST(mac) mac(0), mac(1), mac(2), mac(3)
#   endif
#   if NUM == 2
#       undef NUM
#       define NUM 3
#       define LST(mac) mac(0), mac(1), mac(2)
#   endif
#   if NUM == 1
#       undef NUM
#       define NUM 2
#       define LST(mac) mac(0), mac(1)
#   endif
#   if NUM == 0
#       undef COMMA
#       define COMMA ,
#       undef NUM
#       define NUM 1
#       define LST(mac) mac(0)
#   endif
#   include "koalang/Binding.h"
#endif
#endif // NUM
#endif //__KOALANG_BINDING_H__
