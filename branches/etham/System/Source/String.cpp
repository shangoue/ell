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

#include <system/String.h>
#include <system/Exception.h>

#include <iostream>

using namespace std;

namespace system
{
    void replace_char(std::string &s, char ch1, char ch2)
    {
        for(int i = 0 ; i < (int)s.length() ; i++) {
            if(s[i] == ch1)
                s[i] = ch2;
        }
    }

    void split(const string &s, StringList &tokens, const string &delimiter)
    {
        // Skip delimiters at beginning.
        string::size_type lastPos = s.find_first_not_of(delimiter, 0);

        // Find first "non-delimiter".
        string::size_type pos = s.find_first_of(delimiter, lastPos);

        while (string::npos != pos || string::npos != lastPos)
        {
            // Found a token, add it to the vector.
            tokens.push_back(s.substr(lastPos, pos - lastPos));
            // Skip delimiters.  Note the "not_of"
            lastPos = s.find_first_not_of(delimiter, pos);
            // Find next "non-delimiter"
            pos = s.find_first_of(delimiter, lastPos);
        }
    }

    void split(const string &s, StringList &tokens, const char delimiter)
    {
        char d[]={delimiter, 0};
        string delimiters(d);
        split(s, tokens, delimiters);
    }

    string join(StringList & tokens, const string &delimiter)
    {
        string ret = "" ;
        vector<string>::iterator it ;

        it = tokens.begin() ;
        while(it != tokens.end()) {
            ret += (*it) ;
            if(it + 1 != tokens.end()) {
                ret += delimiter ;
            }
            it++;
        }

        return ret ;
    }

    string join(StringList & tokens, const char delimiter)
    {
        string ret = "" ;
        vector<string>::iterator it ;

        it = tokens.begin() ;
        while(it != tokens.end()) {
            ret += (*it) ;
            if(it + 1 != tokens.end()) {
                ret += delimiter ;
            }
            it++;
        }

        return ret ;
    }

    StringListIterator contains(StringList &tokens, std::string token)
    {
        StringListIterator it;
        for(it = tokens.begin();it != tokens.end(); it++)
            if((*it) == token)
                return it;

        return it;
    }

    ostream &operator<<(ostream &os, StringList &sl)
    {
        for(int i=0;i<(int)sl.size();i++)
            os << sl[i] << " ";
        return os;
    }

#define rotate_right(value, shift) \
((value >> shift) | value << (8 * sizeof(value) - shift))
#define rotate_left(value, shift) \
((value << shift) | value >> (8 * sizeof(value) - shift))

    // Simple hash algorithm
    HashType hash_string(const std::string &str)
    {
        HashType res = 0;
        for(size_t i=0;i<str.size();i++)
        {
            res += str[i];
            res = rotate_right(res, 2);
        }
        return res;
    }

    static const int buf_size = 8192;

    template <>
    std::string string_vprintf(const char * format, va_list args)
    {
        char buf[buf_size];
        int size = vsnprintf(buf, buf_size, format, args);

        if (size >= buf_size)
        {
            size = buf_size - 1;
            buf[size] = '\0';
        }

        return std::string(buf, size);
    }

    template <>
    std::wstring string_vprintf(const wchar_t * format, va_list args)
    {
        wchar_t buf[buf_size];

        int size = vswprintf(buf, buf_size, format, args);
        if (size >= buf_size)
        {
            size = buf_size - 1;
            buf[size] = L'\0';
        }

        return std::wstring(buf, size);
    }

    int ascii_to_hex(int c)
    {
        if (c < '0') return -1;
        if (c <= '9') return (c - '0');
        if (c < 'A') return -1;
        if (c <= 'F') return (10 + c - 'A');
        if (c < 'a') return -1;
        if (c <= 'f') return (10 + c - 'a');

        return -1;
    }
} // namespace System

