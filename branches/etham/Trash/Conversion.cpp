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

#include "KoaLang/CommonHeaders.h"

#include "KoaLang/System/Conversion.h"

namespace KoaLang
{
    namespace System
    {
        using namespace std;

        int utf8_to_wide_string(wstring & to, const char * s, size_t size)
        {
            locale::global(locale(""));

            const codecvt<wchar_t, char, mbstate_t> &f =
                use_facet<codecvt<wchar_t, char, mbstate_t> >(locale());

            mbstate_t state = mbstate_t();

            wchar_t buffer[11];
            wchar_t * end_of_conversion=buffer;
            codecvt_base::result r;

            const char * first=s;
            const char * last=s + size;
            const char * next=first;

            while (first < last)
            {
                r = f.in(state, first, last, next, buffer, buffer + 11, end_of_conversion);

                if (r == codecvt_base::noconv)
                {
                    return CONV_ERROR;
                }
                else if (r == codecvt_base::error)
                {
                    return CONV_ERROR;
                }

                to.append(buffer, end_of_conversion - buffer);
                first = next;
            }
            return CONV_SUCCESS;
        }

#       if 0
        int utf8_to_wide_string(wstring & to, const char * _s, size_t size)
        {
            const __u8 * s=(const __u8 *) _s;
            size_t i = 0;

            while (i < size)
            {
                if (s[i] < 0x80)
                {
                    to.append(1, s[i]);
                    i+=1;
                }
                else if (s[i] >= 0xC2 && s[i] <= 0xDF)   // 0xC2 because UC must be greated or equal to 0x80
                {
                    if ((s[i + 1] & 0xC0) == 0x80)
                    {
                        to.append(1, ((s[i]     & 0x1F) << 6) +
                                      (s[i + 1] & 0x3F));
                        i+=2;
                    }
                    else return (s[i + 1] ? CONV_ERROR : CONV_PARTIAL);
                }
                else if (s[i] >= 0xE0 && s[i] <= 0xEF)
                {
                    if ((s[i + 1] & 0xC0) == 0x80)
                    {
                        if ((s[i + 2] & 0xC0) == 0x80)
                        {
                            to.append(1, ((s[i]     & 0x0F) << 12) +
                                         ((s[i + 1] & 0x3F) << 6) +
                                          (s[i + 2] & 0x3F));
                            i+=3;
                        }
                        else return (s[i + 2] ? CONV_ERROR : CONV_PARTIAL);
                    }
                    else return (s[i + 1] ? CONV_ERROR : CONV_PARTIAL);
                }
                else if (s[i] >= 0xF0 && s[i] <= 0xF4)
                {
                    if ((s[i + 1] & 0xC0) == 0x80)
                    {
                        if ((s[i + 2] & 0xC0) == 0x80)
                        {
                            if ((s[i + 3] & 0xC0) == 0x80)
                            {
                                to.append(1, ((s[i]     & 0x07) << 18) +
                                             ((s[i + 1] & 0x3F) << 12) +
                                             ((s[i + 2] & 0x3F) << 6) +
                                              (s[i + 3] & 0x3F));
                                i+=4;
                            }
                            else return (s[i + 3] ? CONV_ERROR : CONV_PARTIAL);
                        }
                        else return (s[i + 2] ? CONV_ERROR : CONV_PARTIAL);
                    }
                    else return (s[i + 1] ? CONV_ERROR : CONV_PARTIAL);
                }
                else return CONV_ERROR;
            }

            return CONV_SUCCESS;
        }
#       endif
    }
}
