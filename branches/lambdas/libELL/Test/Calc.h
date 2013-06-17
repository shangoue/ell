#include <stack>
#include <ell/Grammar.h>
#include <ell/Parser.h>

struct Calc : ell::Parser<char>, ell::Grammar<char>
{
    Calc()
      : ell::Parser<char>(& root, & blank)
    {
        flags.look_ahead = false;

        factor = ch('(') >> expression >> ch(')') 

              | (ch('-') >> factor) [& Calc::negate] 
               //| (ch('-') >> factor) [[&](){push(-pop());}] 
               | (ch('+') >> factor) 
               | real [& Calc::push];

        term = factor >> *( (ch('*') >> factor) [& Calc::multiply] 
                          | (ch('/') >> factor) [& Calc::divide] );

        expression = term >> *( (ch('+') >> term) [& Calc::add] 
                              | (ch('-') >> term) [& Calc::subtract] );

        root = expression >> ell::Grammar<char>::end;

        ELL_NAME_RULE(factor);
        ELL_NAME_RULE(term);
        ELL_NAME_RULE(expression);
        root.set_name("expression");
    }

    double eval(const char * expr)
    {
        parse(expr);
        return pop();
    }
   
protected:
    double pop()
    {
        double d = stack.top();
        stack.pop();
        return d;
    }

    void push(double d)
    {
        stack.push(d);
    }

    void negate()
    {
        double a = pop();
        push(- a);
    }

#   define BINARY(name, op)     \
    void name()                 \
    {                           \
        double b = pop();       \
        double a = pop();       \
        push(a op b);           \
    }

    BINARY(multiply, *)
    BINARY(divide, /)
    BINARY(add, +)
    BINARY(subtract, -)
#   undef BINARY

    ell::Rule<char> factor, term, expression, root;
    std::stack<double> stack;
};
