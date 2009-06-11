#ifndef __KOALANG_LOCATION_H__
#define __KOALANG_LOCATION_H__

namespace koalang
{
    struct Location
    {
        Location(std::string * file, int line)
          : file(file), line(line)
        { }

        std::string * file;
        int line;
    };

    inline std::ostream & operator << (std::ostream & os, const Location & l)
    {
        os << * l.file << ':' << l.line;
        return os;
    }
}

#endif //__KOALANG_LOCATION_H__
