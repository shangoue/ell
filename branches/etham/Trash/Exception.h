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

#ifndef __KOALANG_SYSTEM_EXCEPTION_H__
#define __KOALANG_SYSTEM_EXCEPTION_H__

#define RaiseError(ErrorName, args)                \
{                                                  \
    std::ostringstream oss;                        \
    oss << __FILE__ ":" << __LINE__ << ": "        \
        << args << '.' << std::ends;               \
    throw ErrorName(oss.str());                    \
}

#define DeclareError(ErrorName)                      \
struct ErrorName : public KoaLang::System::Error     \
{                                                    \
    ErrorName() : KoaLang::System::Error(#ErrorName) \
    { }                                              \
                                                     \
    ErrorName(const std::string & s)                 \
        : KoaLang::System::Error(s)                  \
    { }                                              \
};                                                   \

#if KOALANG_DEBUG == 1
#   define Assert(expr) if (! (expr)) { RaiseError(KoaLang::System::SystemError, "Assertion failed : " #expr " = " << (expr)); }
#else
#   define Assert(expr)
#endif

namespace KoaLang
{
    namespace System
    {
        /// Base KoaLang Error class
        struct Error : public std::exception
        {
            /// Constructor
            Error(const std::string & s) throw()
                : msg(s)
            { }

            /// Destructor
            virtual ~Error() throw()
            { }

            /// std::exception error description override
            virtual const char * what() const throw()
            {
                return msg.c_str();
            }

            std::string msg;
        };

        DeclareError(SystemError);

    } // namespace System
} // namespace KoaLang

#endif // __KOALANG_SYSTEM_EXCEPTION_H__
