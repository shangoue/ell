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

#ifndef INCLUDED_ELL_UNARY_NODES_H
#define INCLUDED_ELL_UNARY_NODES_H

#include <ell/UnaryNode.h>
#include <ell/Parser.h>

namespace ell
{
    /// Disable any nested semantic action
    template <typename Token, typename Child>
    struct NoAction : public UnaryNode<Token, NoAction<Token, Child>, Child>
    {
        typedef UnaryNode<Token, NoAction<Token, Child>, Child> Base;

        NoAction(const Child & target)
          : Base(target, "no_action")
        { }

        using Base::parse;

        template <typename V>
        bool parse(Parser<Token> * parser, Storage<V> & s) const
        {
            ELL_BEGIN_PARSE
            SafeModify<> m1(parser->flags.action, false);
            /// Propagating storage is useless
            match = Base::target.parse(parser);
            ELL_END_PARSE
        }
    };

    /// Forbid stepping back when evaluating a branch of the grammar, which is equivalent to consider it as LL(1)
    template <typename Token, typename Child>
    struct NoStepBack : public UnaryNode<Token, NoStepBack<Token, Child>, Child>
    {
        typedef UnaryNode<Token, NoStepBack<Token, Child>, Child> Base;

        NoStepBack(const Child & target)
          : Base(target, "no_step_back")
        { }

        using Base::parse;
        template <typename V>
        bool parse(Parser<Token> * parser, Storage<V> & s) const
        {
            ELL_BEGIN_PARSE
            SafeModify<> m1(parser->flags.step_back, false);
            match = Base::target.parse(parser, s);
            ELL_END_PARSE
        }
    };

    /// Match without consuming any token
    template <typename Token, typename Child>
    struct NoConsume : public UnaryNode<Token, NoConsume<Token, Child>, Child>
    {
        typedef UnaryNode<Token, NoConsume<Token, Child>, Child> Base;

        NoConsume(const Child & target)
          : Base(target, "no_consume")
        { }

        using Base::parse;
        template <typename V>
        bool parse(Parser<Token> * parser, Storage<V> & s) const
        {
            ELL_BEGIN_PARSE
            typename Parser<Token>::Context sav_pos = parser->save_pos();
            match = Base::target.parse(parser, s);
            parser->restore_pos(sav_pos);
            ELL_END_PARSE
        }
    };

    /// This class allows to define a new grammar terminator.
    /// When parsing parsing through its children, skipper is disabled and stepping back allowed.
    template <typename Token, typename Child>
    struct Lexeme : public UnaryNode<Token, Lexeme<Token, Child>, Child>
    {
        typedef UnaryNode<Token, Lexeme<Token, Child>, Child> Base;

        Lexeme(const Child & target)
          : Base(target, "lexeme")
        { }

        using Base::parse;
        template <typename V>
        bool parse(Parser<Token> * parser, Storage<V> & s) const
        {
            ELL_BEGIN_PARSE
            SafeModify<> m1(parser->flags.step_back, true);
            SafeModify<const Node<Token> *> m2(parser->skipper, 0);
            match = Base::target.parse(parser, s);
            ELL_END_PARSE
        }
    };

    /// Compile-time min/max repeat
    template <typename Token, typename Child, const int Min, const int Max>
    struct Repeat : public UnaryNode<Token, Repeat<Token, Child, Min, Max>, Child>
    {
        typedef UnaryNode<Token, Repeat<Token, Child, Min, Max>, Child> Base;

        using Base::target;
        using Base::parse;

        Repeat(const Child & target)
          : Base(target, "repeat")
        { }

#       define MIN Min
#       define MAX Max
#       include "RepeatMatchTpl.h"

        void describe(std::ostream & os) const
        {
            os << target << ' ';
            if (Max == -1 and Min == 0)
                os << '*';
            else if (Max == -1 and Min == 1)
                os << '+';
            else if (Max == 1 and Min == 0)
                os << '!';
            else if (Max == Min)
                os << '{' << Min << '}';
            else if (Min == 0)
                os << "{," << Max << '}';
            else if (Max == -1)
                os << '{' << Max << ",}";
            else
                os << '{' << Min << ',' << Max << '}';
        }
    };

    /// Run-time min/max repeat
    template <typename Token, typename Child, typename CP>
    struct DynRepeat : public UnaryNode<Token, DynRepeat<Token, Child, CP>, Child>
    {
        typedef UnaryNode<Token, DynRepeat<Token, Child, CP>, Child> Base;

        using Base::target;
        using Base::parse;

        DynRepeat(const Child & target,
                  const int CP::*min,
                  const int CP::*max)
          : Base(target, "dyn_repeat"),
            min(min),
            max(max)
        { }

#       define MIN (((CP *) parser)->*min)
#       define MAX (((CP *) parser)->*max)
#       include "RepeatMatchTpl.h"

        void describe(std::ostream & os) const { os << target << " {?,?}"; }

    private:
        const int CP::*min;
        const int CP::*max;
    };

    template <typename Token, typename Child, typename ConcreteParser, typename Var, typename Value>
    struct Action
    { };
    
    namespace
    {
        template <typename CP, typename V, typename A>
        bool make_action(CP * parser, A CP::*attr, Storage<V> & s);

        template <typename CP, typename V>
        bool make_action(CP * parser, V CP::*var, Storage<V> & s)
        { 
            parser->*var = s.value;
            return true;
        }

        template <typename CP, typename V>
        bool make_action(CP * parser, void(CP::*method)(const V &), Storage<V> & s)
        { 
            (parser->*method)(s.value);
            return true;
        }

        template <typename CP, typename V>
        bool make_action(CP * parser, bool(CP::*method)(const V &), Storage<V> & s)
        { 
            return (parser->*method)(s.value);
        }

        template <typename CP, typename V>
        bool make_action(CP * parser, void(CP::*method)(V), Storage<V> & s)
        { 
            (parser->*method)(s.value);
            return true;
        }

        template <typename CP, typename V>
        bool make_action(CP * parser, bool(CP::*method)(V), Storage<V> & s)
        { 
            return (parser->*method)(s.value);
        }

        template <typename CP>
        bool make_action(CP * parser, void(CP::*method)(), Storage<void> & s)
        { 
            (parser->*method)();
            return true;
        }

        template <typename CP>
        bool make_action(CP * parser, bool(CP::*method)(), Storage<void> & s)
        { 
            return (parser->*method)();
        }
    }

    template <typename Token, typename Child, typename ConcreteParser, typename Var, typename Value>
    struct Action<Token, Child, ConcreteParser, Var ConcreteParser::*, Value>
      : public UnaryNode<Token, Action<Token, Child, ConcreteParser, Var ConcreteParser::*, Value>, Child>
    {
        typedef UnaryNode<Token, Action<Token, Child, ConcreteParser, Var ConcreteParser::*, Value>, Child> Base;

        Action(const Child & target, Var ConcreteParser::*v)
          : Base(target, "action"),
            var(v)
        { }

        using Base::parse;
        template <typename V>
        bool parse(Parser<Token> * parser, Storage<V> & s) const
        {
            ELL_BEGIN_PARSE
            if (parser->flags.action)
            {                                         
                Storage<Value> sa;
                match = Base::target.parse(parser, sa);
                if (match)
                {
                    match = make_action((ConcreteParser *) parser, var, sa);
                    assign(s, sa);
                }
            }
            else
                match = Base::target.parse(parser);
            ELL_END_PARSE
        }

    private:
        Var ConcreteParser::*var;
    };
}

#endif // INCLUDED_ELL_UNARY_NODES_H
