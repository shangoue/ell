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


#include <ell/Grammar.h>
#include <ell/Parser.h>

/// This grammar tolds if you are eating a brunch or a dinner.
/// Just write what you eat, and you should know what time is it...
struct MealGrammar : ell::Grammar<char>
{
    MealGrammar()
    {                                    
        root = no_look_ahead( brunch [&MealGrammar::isBrunch]
                            | dinner [&MealGrammar::isDinner] );

        ELL_NAME_RULE(brunch) = str("bacon") >> str("eggs") >> str("beer");
        ELL_NAME_RULE(dinner) = str("salad") >> str("cassoulet") >> str("kouign amann");

        root.set_transparent();
    }

    void isBrunch() { std::cout << "You are eating a brunch\n"; }
    void isDinner() { std::cout << "You are eating your dinner\n"; }

    ell::Rule<char> root, brunch, dinner;
};

int main()
{
    MealGrammar gram;

    ell::Parser<char> p(& gram.root, & gram.blank);
    ELL_ENABLE_DUMP(p);

    try
    {
        p.parse("bacon eggs kouign amann");
    }
    catch (std::exception & e)
    {
        std::cerr << "Expected error caught:\n" << e.what() << "\n";
    }

    return 0;
}
