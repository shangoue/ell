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

#ifndef __SIMPLE_SYSTEM_EXCEPTION_H__
#define __SIMPLE_SYSTEM_EXCEPTION_H__

#include <system/String.h>
#include <string>

#define RaiseError(ErrorName, msg, ...)              \
    do { throw ErrorName(System::string_printf(__FILE__ ":%d" msg ".\n", __LINE__ , ## __VA_ARGS__).c_str()); } while (0)

#define DeclareError(ErrorName)                      \
struct ErrorName : public System::Error      \
{                                                    \
    ErrorName() : System::Error(#ErrorName)  \
    { }                                              \
                                                     \
    ErrorName(const std::string & s)                 \
        : System::Error(s)                   \
    { }                                              \
};                                                   \

#if SIMPLE_DEBUG == 1
#   define Assert(expr) do { if (! (expr)) {         \
        std::ostringstream oss;                      \
        oss << #expr << " = " << (expr);             \
        RaiseError(System::SystemError, "Assertion failed: %s", oss.str().c_str()); } } while (0)
#else
#   define Assert(expr)
#endif

#   define Abort(...) \
    do { RaiseError(System::SystemError, "Aborted: ", __VA_ARGS__); } while (0)

namespace system
{
    struct Error : public std::exception
    {
        //TODO: see koalang system layer
        Error(const std::string & s) throw()
          : msg(s)
        { }

        virtual ~Error() throw()
        { }

        virtual const char * what() const throw()
        {
            return msg.c_str();
        }

        std::string msg;
    };

    DeclareError(SystemError);

} // namespace System

#endif // __SIMPLE_SYSTEM_EXCEPTION_H__
