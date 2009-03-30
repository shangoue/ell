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

#ifndef __KOALANG_DEFS__
#define __KOALANG_DEFS__

#define NULLARY_OPS                                                \
    D(0, BREAK, "break")                                           \

#define UNARY_OPS                                                  \
    D(1, RET, "return " << P1)                                     \
    D(1, NEG, "- " << P1)                                          \
    D(1, NOT, "! " << P1)                                          \
    D(1, COUNT, "# " << P1)                                        \

#define BINARY_OPS                                                 \
    D(2, EQ, P1 << " == " << P2)                                   \
    D(2, NE, P1 << " != " << P2)                                   \
    D(2, LE, P1 << " <= " << P2)                                   \
    D(2, GE, P1 << " >= " << P2)                                   \
    D(2, LT, P1 << " < " << P2)                                    \
    D(2, GT, P1 << " > " << P2)                                    \
                                                                   \
    D(2, MULT, P1 << " * " << P2)                                  \
    D(2, DIV, P1 << " / " << P2)                                   \
    D(2, MODULO, P1 << " % " << P2)                                \
    D(2, ADD, P1 << " + " << P2)                                   \
    D(2, SUB, P1 << " - " << P2)                                   \
                                                                   \
    D(2, AND, P1 << " & " << P2)                                   \
    D(2, OR, P1 << " | " << P2)                                    \
    D(2, XOR, P1 << " ^ " << P2)                                   \

#define SPECIAL_EVAL_OPS                                           \
    D(1, SCOPE, "{ " << P1 << " }")                                \
    D(1, PARENT, ".." << P1)                                       \
    D(2, MEMBER, P1 << '.' << P2)                                  \
    D(2, ASSIGN, P1 << " = " << P2)                                \
    D(2, WHILE, "while " << P1 << '\n' << P2)                      \
    D(2, DOWHILE, "do\n" << P1 << "\nwhile " << P2)                \
    D(3, IF, "if " << P1 << '\n' << P2 << "\nelse\n" << P3)        \
    D(3, FOREACH, "for " << P1 << " in " << P2 << '\n' << P3)      \
    D(3, DEF, "def " << P1 << '[' << P2 << "]: " << P3)

#define ASSIGNABLE_OPS                                             \
    D(2, SELECT, P1 << '[' << P2 << ']')                           \
    D(3, SLICE, P1 << '[' << P2 << ".." << P3 << ']')              \

#define OPERATORS \
  NULLARY_OPS UNARY_OPS BINARY_OPS SPECIAL_EVAL_OPS ASSIGNABLE_OPS

namespace koalang
{
#   define ABSTRACT_TYPES D(Iterable) D(Indexable)
#   define TYPES D(Number) D(String) D(List) D(Function) D(Object) D(Variable) D(Nothing)

#   define D(args, op, str) op,
    enum { OPERATORS };
#   undef D

    extern const char * op_str[];
}
#endif // __KOALANG_DEFS__
