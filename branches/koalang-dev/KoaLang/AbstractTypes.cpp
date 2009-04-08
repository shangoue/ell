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

#include "koalang/AbstractTypes.h"
#include "koalang/Types.h"

namespace koalang
{
    Abstract * Indexable::IndexableIterator::next()
    {
        if (index == indexable->size())
            return new Nothing;

        return indexable->get_element(index++);
    }

    Abstract * Indexable::select(Abstract * p, Object * o)
    {
        if (! p->isNumber()) raise_error(number_expected_error + p->what());

        int n = (int) ((Number *) p)->value;

        return get_element(n);
    }
}
