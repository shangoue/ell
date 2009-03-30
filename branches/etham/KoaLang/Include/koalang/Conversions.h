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

#ifndef __KOALANG_CONVERSIONS__
#define __KOALANG_CONVERSIONS__
namespace koalang
{
    template <const int KoaT, typename CppT>
    CppT koa2cpp(AbstractNode * p);

    template <typename CppT>
    AbstractNode * cpp2koa(CppT p);

#   define DEF(CppT)                               \
    template <>                                    \
    CppT koa2cpp<NUMBER, CppT>(AbstractNode * p)   \
    {                                              \
        return (CppT) ((Number *) p)->value; \
    }
    DEF(char) DEF(int) DEF(long) DEF(float) DEF(double)
#   undef DEF

    template <>
    char * koa2cpp<STRING, char *>(AbstractNode * p)
    {
        return ((String *) p)->value.c_str();
    }

    template <>
    std::string & koa2cpp<STRING, std::string &>(AbstractNode * p)
    {
        return ((String *) p)->value;
    }

    template <>
    char koa2cpp<STRING, char>(AbstractNode * p)
    {
        return ((String *) p)->value[0];
    }
}
#endif // __KOALANG_CONVERSIONS__
