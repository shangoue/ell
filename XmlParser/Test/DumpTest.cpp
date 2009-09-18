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

using namespace ell;

struct TestGrammar : public Grammar<char>
{
    TestGrammar()
    {
        ELL_NAME_RULE(sentence) =
            (subjverb >> ! (ch(' ') >> target)) % (ch('.') >> ch(' '));

        ELL_NAME_RULE(subjverb) =
              ( str("il") | str("je") ) >> root 
            | str("tu") >> root >> ch('s') 
            | str("nous") >> root >> str("ons") 
            | str("vous") >> root >> str("z")
            | str("ils") >> root >> str("nt");

        ELL_NAME_RULE(target) =
            str("une pomme") | str("un saucisson") | str("du cassoulet");

        ELL_NAME_RULE(root) =
            str("mange") | str("cache") | str("donne");
    }

    Rule<char> sentence, subjverb, target, root;
};

#include <ell/XmlDump.h>

int main()
{
    TestGrammar tg;
    RuleXmlDumper<char> rxd;

    rxd.add_rule_def(tg.sentence);
    rxd.add_rule_def(tg.subjverb);
    rxd.add_rule_def(tg.target);
    rxd.add_rule_def(tg.root);

    rxd.unparse(std::cout);
    
    return 0;
}

