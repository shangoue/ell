

# libELL Reference Manual #

This library is written for quick developpement of LL(n) parsers, or other kind of [EBNF](http://en.wikipedia.org/wiki/EBNF)-guided C++ applications.

It allows to write EBNF grammars in an executable manner, directly as C++ code.

## The grammar ##

### Grammar building basis ###

First of all, you will need to be familier to [EBNF grammars](http://en.wikipedia.org/wiki/EBNF). To sum up, EBNF is Backus Naur Form extended to allow the use of regular expression operators.

Let's take the basic grammar which defines how our mathematic expressions works :

```
  factor     : "(" expression ")" | INTEGER
  term       : factor ('*' factor | '/' factor) *
  expression : term ('+' term | '-' term) *
```


We will now try to write this grammar simply as C++ code.

The definition of each non-terminal (called rule of the grammar) can be represented by an Abstract Syntax Tree, for exemple for the rule `factor`:

```
              factor
             /      \
           "("     INTEGER
           /
       expression
         /
       ")"
```

In this tree nodes `INTEGER` and `(` are primitives. To the contrary the node `expression` is a link to another tree. Thus, the whole grammar can be represented by a graph.

The C++ code of the grammar will build this syntactic graph in memory. The parser will walk through this graph trying to match each node with the input flow, according to the principle of LL(n) parsing.

**Note:**

> This libray is not a parser generator (like bison ou antlr), but a tool to program an executable high-level grammar.
> So, there is no mecanism to find conflicts between branches : the analysis will stop at the first matching branch find within the grammar. So the order of writing alternatives (with the `|` operator) matters.
> In practical, writing ambiguous branches in the grammar is most of the times useless, as we prefer to group their prefix in a separate rule to write a grammar with a "one-token look-ahead".


Let's come back to the previous tree, and notice that its representation only supports BNF grammars, not EBNF ones. EBNF grammars extend BNF ones by the use of regular expression operators, such as thoses found in posix or perl regexp:
  * `*`: repeat the argument zero or more times
  * `+`: repeat one or more times
  * `!`: argument is optional

To represent these operators, a new tree format is needed:

```
              factor
                |
           Alternative
             /      \
      Aggregation    INTEGER
      /        \
     "("    Aggregation
           /          \
       expression     ")"
```

Complex operator `*` is represented by a binary node called `Repeat| which is parameterized by the minimum and maximum bound of this repetition (-1 as max bound means no limit).

```
        expression
            |
       Aggregation
      /          \
  term         Repeat<0, -1>
                    |
                Alternative
                 /       \
        Aggregation     Aggregation
          /     \         /      \
       '+'      term   '-'       term
```


Now, parsing an expression just consists in walking through the tree from root node and consuming tokens each time a primitive is matched.

Here below is the complete grammar translated into C++:

```
  factor = real
         | ch('(') >> expression >> ch(')')
         | (ch('-') >> factor)
         | (ch('+') >> factor);

  term = factor >> *( (ch('*') >> factor)
                    | (ch('/') >> factor)
                    | (ch('%') >> factor) );

  expression = term >> *( (ch('+') >> term)
                        | (ch('-') >> term) );
```

To keep the spirit of a BNF grammar, we jump to a new line on introducing an alternative with the `|` operator. We can also use parenthesis to avoid the use of an intermediate rule.

Notice that the use of the aggregation operator `>>` instead of just juxtaposing elements.

Now, we can add semantic actions. We do that with the `[]` operator. Such actions are executed only when the element on which they apply matches.

```
  factor = real [push] |
           ch('(') >> expression >> ch(')') |
           (ch('-') >> factor) [negate] |
           (ch('+') >> factor);

  term = factor >> *((ch('*') >> factor) [multiply] |
                     (ch('/') >> factor) [divide]);

  expression = term >> *((ch('+') ams feature a tight an>> term) [add] |
                         (ch('-') >> term) [substract]);
```

The full working exemple of this calculator is present in the repository at https://github.com/shangoue/ell/blob/master/libELL/Test/Calc.h


### Grammar primitives ###

Ell library provides a set of predefined primitives:

| Primitive | Meaning |
|:----------|:--------|
| end                  | matches the end of the input (but does not consume it!) |
| eps                  | epsilon: always matches, never consumes |
| nop                  | no-operation: never matches |
| any                  | matches any input token (eos excepted) |
| ch(token)            | matches the given token |
| str(s)               | matches a string of input tokens |
| error(msg)           | unconditional parsing error generation with custom message |

For character parsers (ie. when input tokens are C++ primitives like `char`, `int` or `wchar_t`), some specialized parser primitives are provided:

| Primitive | Meaning |
|:----------|:--------|
| chset(arg)           | give a charset. Ex: `chset("A-Z_.-")` defines `[A-Z_.-]` |
| digit                | `[0-9]` |
| range<a, b>          | matches a character in the given range |
| lower                | `[a-z]` |
| upper                | `[A-Z]` |
| alpha                | `[a-zA-Z_]` |
| alnum                | `[a-zA-Z0-9_]` |
| blank                | `[ \t\n\r]` |
| ident                | `lexeme(alpha >> * alnum)` |
| istr(s)              | matches the given string, ignoring case |
| visible\_ascii        | `[!-~]` (visibles ASCII character) |
| utf8nonascii         | valid UTF-8 character outside the ASCII character set |
| signed\_dec           | signed decimal integer |
| real                 | flotting point number (`strtod` format) |
| dec                  | decimal integer (`+ digit`) |
| hex                  | hexadecimal integer |
| oct                  | octal integer |
| bin                  | binary integer |
| kw(s)                | keyword (equivalent to `no_suffix(str(s), alnum)`) |
| ikw(s)               | case-insensitive keyword |


### Grammar operators ###

The tables below lists all the operators which can be used to write a grammar. Of course, you can use [C++ operator precedence](http://www.cppreference.com/wiki/operator_precedence).

#### Binary operators ####

| Name | Notation | Meaning |
|:-----|:---------|:--------|
| Sequence | `a >> b` | a followed by b |
| Choice | `a | b`  | a or b  |
| Exclusion | `a - b`  | a bereft of b (match a but not b) |
| List (with separator) | `a % b`  | one or more a separated by b (similar to `a >> * (b >> a)` but lefts an eventual trailing separator unconsumed) |
| Bound repetition | `a * b`  | zero or more a until b (similar to `* (a - b) >> b` but much more efficient) |
| Combination | `a & b`  | a followed or preceded by b (similar to `(a >> b) | (b >> a)`) |
| Longest | `a |``| b` | the longest of a or b (take a if equal). Avoid using it if you care in performance |

#### Unary operators ####

| Notation | Meaning |
|:---------|:--------|
| `! a`    | Optional match |
| `+ a`    | One or more repetition |
| `* a`    | Zero or more repetition |
| `a [& Class::variable]` | Assign the result of matching into a variable. |
| `a [& Class::method]` | Call the given (non-const) member function passing the matching result. The method can return a boolean type to confirm (or not) the matching of the node to which it applies. |

**Note:**

> Last two operators are called semantic operators. They have no effect if the rule on which they applied does not match. The variables or methods passed in argument must be members of the Parser class instance used for parsing.

> The types which are allowed for variables or method targetted by the semantic action depend on the node on which the action operator is applied. For example, the code below parses a list of space-separated unsigned integers, and assign them into a standard set of signed integers. The repetition operator (`+`) allows to target a STL container : without such a repetition, the compiler would issue an error when trying to resolve the `[ ]` operator:

```
struct MyParser : ell::Parser<char>, ell::Grammar<char>
{
    MyParser()
      : ell::Parser<char>(& rule, & blank)
    {
        rule = (+ dec) [& MyParser::list];
   
        parse("12 1 9");
    }
   
    ell::Rule<char> rule;
   
    std::set<int> list;
};
```

### Grammar directives ###

In addition to primitives and operators, you can use the following methods in your grammar:

| Notation | Meaning |
|:---------|:--------|
| `repeat<exact>(rule)` | Exact number of repetition |
| `repeat<min, max>(rule)` | Repetition within a range |
| `repeat(& Class::exact, rule)` | Exact repetition resolved at run-time |
| `repeat(& Class::min, &Class::max, rule)` | Range repetition resolved at run-time |
| `lexeme(rule)` | Defines a new primitive by disabling skipper and allowing look-ahead inside `rule` |
| `no_suffix(a, b)` | equivalent to `lexeme(a >> (eps - b))` |
| `look_ahead(rule)` | Allow parser step back when searching for the first matching branch along `rule` |
| `no_look_ahead(rule)` | Issue an error when the right operand of an aggregation operataor mismatch, ie. walk along the grammar in a LL(1) manner |
| `action(rule)` | Enable all semantic actions (this is the default) |
| `no_action(rule)` | Disable all semantic actions (inhibate operator[.md](.md)) |
| `consume(rule)` | Enable the tokens-consumption (this is the default) |
| `no_consume(rule)` | Restore the position of the parser in the input token list after having parse along `rule` |
| `skip(rule)` | Enable skipper for parsing the given rule |
| `skip(rule, skipper)` | Use the given skipper to parse the given rule |
| `no_skip(rule)` | Disable skipper |
| `debug(rule)` | Enable debug mode |
| `no_debug(rule)` | Disable debug mode |

## How to write your grammars ##

### Prevent branch conflicts ###

The idea between ELL is that simplicity is better that heavy-featuring.
Indeed, ELL library provides a straigth forward means to write LL(n) grammars, but there is no mecanism to detect conflicts between branch.

Writing a grammar with ELL library is no more than building up a tree along which the Parser will perform a depth-first search.

So, order does matter when writing a grammar. Consider the following (quite naïve) exemple:

```
rule1 = str("foo") >> str("bar")
      | str("foo") >> str("t");
```

If you try to parse the word `foot`, the parser will first try to match the first branch of the `|`-alternative. The second primitive `str("bar")` won't match, so the parser will fall back on the second branch.

Keep in mind that each time a node of the grammar does not match, the cursor of the parser is restored back at the position where it was before try to match this node. So the definition above will produce a working parser for word `foot`, although rather inefficient.

A better way to write the grammar would be :

```
rule1 = str("foo") >> ( str("bar")
                      | str("t") );
```

This will prevent the parser from stepping-back to a token which has already been consumed.

### Debug your grammar ###

Do not try to step inside ELL code using a debugger : you will be plagued by meaningless recursive calls !

To the contrary, you can use the dump of ELL parser. To enable it, use the `debug` directive inside your grammar definition, or just assign `flags.debug` to `true` in the constructor of your parser. The latter can be done using ELL\_ENABLE\_DUMP macro (which takes a reference on your parser).

Consider the following grammar:

```
root = brunch | dinner;

brunch = str("bacon") >> str("eggs") >> str("beer");

dinner = str("salad") >> str("cassoulet") >> str("kouign amann");
```


Now, let's parse a meal which does not match this grammar, for example:

```
bacon eggs kouign amann
```

Using macro ELL\_ENABLE\_DUMP, you can trace how the parser walks along the grammar. First character is one of:
  * `\`: parser starts trying the rule
  * `/`: parser succeeds at matching the rule
  * `#`: the rule did not match the input buffer

Then, the name of the related rule follows, and the position inside the buffer to parse.

In the dump below, you can notice than the parser first tries the `brunch` branch, then step back to the root rule to try the `dinner` branch:

```
 \ brunch or dinner: 	"bacon eggs kouign amann"
  \ brunch: 	"bacon eggs kouign amann"
   \ "bacon" "eggs" "beer": 	"bacon eggs kouign amann"
    \ "bacon" "eggs": 	"bacon eggs kouign amann"
     \ "bacon": 	"bacon eggs kouign amann"
     / "bacon": 	" eggs kouign amann"
     \ "eggs": 	"eggs kouign amann"
     / "eggs": 	" kouign amann"
    / "bacon" "eggs": 	" kouign amann"
    \ "beer": 	"kouign amann"
    # "beer": 	"kouign amann"
   # "bacon" "eggs" "beer": 	"bacon eggs kouign amann"
  # brunch: 	"bacon eggs kouign amann"
  \ dinner: 	"bacon eggs kouign amann"
   \ "salad" "cassoulet" "kouign amann": 	"bacon eggs kouign amann"
    \ "salad" "cassoulet": 	"bacon eggs kouign amann"
     \ "salad": 	"bacon eggs kouign amann"
     # "salad": 	"bacon eggs kouign amann"
    # "salad" "cassoulet": 	"bacon eggs kouign amann"
   # "salad" "cassoulet" "kouign amann": 	"bacon eggs kouign amann"
  # dinner: 	"bacon eggs kouign amann"
 # brunch or dinner: 	"bacon eggs kouign amann"
```



### Improve error messages ###

There is two ways to manage syntax errors in your ell-based parser:
> - Use the grammar directive `error` to emit a custom message at one point
> - Rely on ell automatic messages

The first method is straight-forward but requires a little more work and make your grammar bigger.

That's why the second method can be time-saving. We will see it works fine provided that you disable the `look_ahead` flag to turn your the parser into LL(1).

Let's examine again the meal grammar again:


```
root = brunch | dinner;

brunch = str("bacon") >> str("eggs") >> str("beer");

dinner = str("salad") >> str("cassoulet") >> str("kouign amann");
```

The following text will arises an error message:

```
bacon eggs kouign amann
```

```
1: before "bacon eggs kouign amann": expecting brunch or dinner
```

Not a useful message... What would you say if you forgot a semicolon at line 1384 and gcc was complaining that way:
```
inyourbigfile.c:1: Syntax error, expecting a completely valid C program file
```


Now, we will add a `no_look_ahead` directive in the grammar to tell the parser walking through it in a LL(1) manner :

```
root = no_look_ahead(brunch | dinner);

brunch = str("bacon") >> str("eggs") >> str("beer");

dinner = str("salad") >> str("cassoulet") >> str("kouign amann");
```

When look-ahead is allowed, the parser will step back in the grammar and try another branch until it has explored every possibility.

To the contrary, when look ahead is disabled, the sequence grammar operator (>>) issues an error each time its left operand has matched but not its right operand.

In other words, without look-ahead, a single token is enough to decide which branch of the LL grammar must be chosen. Once the choice is made, the first error will stop the parser.



For example, when parsing "`bacon eggs`", if we get the `bacon` token, we are sure to be in the `brunch` branch. In this branch, we expect an `eggs` token, else an error is arisen.

The related dump follows:
```
 \ brunch or dinner: 	"bacon eggs kouign amann"
  \ brunch: 	"bacon eggs kouign amann"
   \ "bacon" "eggs" "beer": 	"bacon eggs kouign amann"
    \ "bacon" "eggs": 	"bacon eggs kouign amann"
     \ "bacon": 	"bacon eggs kouign amann"
     / "bacon": 	" eggs kouign amann"
     \ "eggs": 	"eggs kouign amann"
     / "eggs": 	" kouign amann"
    / "bacon" "eggs": 	" kouign amann"
    \ "beer": 	"kouign amann"
    # "beer": 	"kouign amann"
terminate called after throwing an instance of 'std::runtime_error'
  what():  1: before "kouign amann": expecting "beer"
```


Error has been catched earlier and the user is told he should drink beer instead of eating kouign amann, which appears a lot more natural, no ?


You can look at the full exemple: https://github.com/shangoue/ell/blob/master/libELL/Test/MealParser.cpp
