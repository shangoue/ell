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

#ifndef __KOALANG_MESSAGES__
#define __KOALANG_MESSAGES__
namespace koalang
{
#   ifndef DEF
#   define DEF(a,b,c) extern const char * a##_##b;
#   endif
    DEF(assign,             error, "Not assignable: ")
    DEF(at_root_scope,      error, "Already at root scope: ")
    DEF(eval,               error, "Cannot evaluate: ")
    DEF(incompatible_types, error, "Incompatibles types: ")
    DEF(list_expected,      error, "Expecting a list: ")
    DEF(not_found,          error, "Not found: ")
    DEF(not_indexable,      error, "Cannot index: ")
    DEF(not_iterable,       error, "Cannot iterate over: ")
    DEF(not_selectable,     error, "Cannot call or index: ")
    DEF(number_expected,    error, "Expecting a number: ")
    DEF(object_expected,    error, "Expecting an object: ")
    DEF(out_of_bound,       error, "Out of bound: ")
    DEF(test_function,      error, "Cannot test a function: ")
    DEF(undef_var,          error, "Undefined variable: ")
    DEF(unexp_char,         error, "Unexpected character: ")
    DEF(unexpected,         error, "Unexpected error: ")
    DEF(unexp_keyword,      error, "Unexpected keyword: ")
#   undef DEF

#   define raise_error(arg) throw std::runtime_error(arg)
}
#endif // __KOALANG_MESSAGES__
