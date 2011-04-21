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

#ifndef INCLUDED_ELL_GRAMMARXML_H
#define INCLUDED_ELL_GRAMMARXML_H

#include <ell/Node.h>
#include <ell/XmlNode.h>

namespace ell
{
    template <typename Token>
    struct RuleXmlDumper : public XmlNode
    {
        RuleXmlDumper()
        {
            name = "grammar";
        }

        void add_rule_def(const Node<Token> & rule)
        {
            assert(rule.get_kind() == "rule");
            assert(rule.get_child_at(0) != 0);
            assert(rule.get_value().size());

            rules[& rule] = true;

            current = enqueue_child(new XmlNode);
            current->name = "rule";
            current->set_attrib("name", rule.get_value());

            add_node(* rule.get_child_at(0));
        }

        /// Dump the full grammar, starting at the given entry point
        void dump_grammar(const Rule<Token> & root, std::ostream & out)
        {
            rules[& root] = false;
            bool cont = true;
            while (cont)
            {
                cont = false;
                for (typename std::map<const Node<Token> *,bool>::iterator i = rules.begin();
                     i != rules.end();
                     ++i)
                {
                    if (not i->second)
                    {
                        add_rule_def(* i->first);
                        cont = true;
                        break;
                    }
                }
            }
            out << * this;
        }

    private:
        XmlNode * current;

        std::map<const Node<Token> *,bool> rules;
        
        void add_node(const Node<Token> & node)
        {
            std::string kind = node.get_kind();
            std::string value = node.get_value();

            XmlNode * parent = current;
            current = parent->enqueue_child(new XmlNode);

            if (kind == "rule" && value.size())
            {
                current->name = "subrule";
                current->set_attrib("name", value);

                rules[& node];
            }
            else
            {
                current->name = kind;
                if (value.size())
                    current->set_attrib("value", value);

                const Node<Token> * child;
                for (int i = 0; (child = node.get_child_at(i)) != 0; ++i)
                {
                    add_node(* child);
                }
            }

            current = parent;
        }
    };
}

#endif // INCLUDED_ELL_GRAMMARXML_H
