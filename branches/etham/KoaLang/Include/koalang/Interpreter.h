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

#ifndef __KOALANG_INTERPRETER__
#define __KOALANG_INTERPRETER__

#include "parser/Parser.h"
#include "koalang/Types.h"
#include "koalang/Operators.h"

namespace koalang
{
    struct Grammar;

    class Interpreter : public parser::Parser
    {
        public:
            Interpreter(Grammar &);

            List * pop_stack();
            void push_stack(List * s);

            /// To recover on error
            void clean();

            void push_nothing();
            void push_number();
            void push_string();
            void push_variable(const char * begin);
            void push_empty();
            void begin_list();
            void end_list();

            template <const int NbArgs, const int Op>
            void push_op()
            {
                Operator<Op> * n=new Operator<Op>;
                for(int i=0; i < NbArgs; i++)
                {
                    n->params[NbArgs - 1 - i]=pop();
                }
                push(n);
            }

            Language::Object * root_context;

        private:
            //@{
            // Stack handling
            void push(Abstract * n);
            Abstract * pop();
            //@}

            std::vector<List *> stack;
    };
}
#endif // __KOALANG_INTERPRETER__
