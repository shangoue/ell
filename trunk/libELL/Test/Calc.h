#include <stack>
#include <ell/Grammar.h>
#include <ell/Parser.h>

struct Calc : ell::Parser<char>, ell::Grammar<char>
{
    Calc()
      : ell::Parser<char>(& expression, & blank)
    {
        factor = dec [& Calc::push] |
                 ch('(') >> expression >> ch(')') |
                 (ch('-') >> factor) [& Calc::negate] |
                 (ch('+') >> factor);

        term = factor >> *((ch('*') >> factor) [& Calc::multiply] |
                           (ch('/') >> factor) [& Calc::divide]);

        expression = term >> *((ch('+') >> term) [& Calc::add] |
                               (ch('-') >> term) [& Calc::substract]);

        ELL_NAME_RULE(factor)
        ELL_NAME_RULE(term)
        ELL_NAME_RULE(expression)
    }

    int eval(const char * expr)
    {
        parse(expr);
        return pop();
    }
   
private:
    int pop()
    {
        int d = stack.top();
        stack.pop();
        return d;
    }

    void push(int d)
    {
        stack.push(d);
    }

    void negate()
    {
        int a = pop();
        push(- a);
    }

#   define BINARY(name, op)     \
    void name()                 \
    {                           \
        int b = pop();          \
        int a = pop();          \
        push(a op b);           \
    }

    BINARY(multiply, *)
    BINARY(divide, /)
    BINARY(add, +)
    BINARY(substract, -)
#   undef BINARY

    ell::Rule<char> factor, term, expression;
    std::stack<int> stack;
};
