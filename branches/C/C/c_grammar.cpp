
#include <ell/Parser.h>
#include <ell/Grammar.h>

#include "c_lexer.h"

namespace ell
{
    struct c_grammar : public Grammar<Lex>
    {
        c_grammar()
          : root()
#           define D(NAME) , NAME(Lex(TOK_##NAME))
#           include "c_tokens.def"
        {
            root = * ( function_def
                     | decl );

            function_def = ! decl_specifiers >> declarator >> ! +decl >> compound_stmt;

            decl = decl_specifiers >> ! ((declarator >> ! (ASSIGN >> init)) % COMMA) >> SEMICOLON;

            decl_specifiers = + ( TYPEDEF
                                | EXTERN
                                | STATIC
                                | AUTO
                                | REGISTER;
                                | type_specifier
                                | type_qualifier );

            declarator = ! pointer >> ( IDENTIFIER
                                      | LEFT_PAREN >> declarator >> RIGHT_PAREN ) >> + ( LEFT_BRACKET >> ! conditional_expr >> RIGHT_BRACKET
                                                                                       | LEFT_PAREN >> ! ( parameter_type_list
                                                                                                         | IDENTIFIER % COMMA ) >> RIGHT_PAREN );

            abstract_declarator
                = pointer || (( LEFT_PAREN >> ( abstract_declarator | ! parameter_type_list) >> RIGHT_PAREN 
                              | LEFT_BRACKET >> ! conditional_expr >> RIGHT_BRACKET )
                              >> * ( LEFT_BRACKET >> ! conditional_expr >> RIGHT_BRACKET
                                   | LEFT_PAREN >> ! parameter_type_list >> RIGHT_PAREN));

            compound_stmt = LEFT_BRACE >> ! (+decl || +stmt) >> RIGHT_BRACE;

            stmt = labeled_stmt | compound_stmt | expr_stmt | if_stmt | switch_stmt | iteration_stmt | jump_stmt;

            init = assign_expr | LEFT_BRACE >> init % COMMA >> ! COMMA >> RIGHT_BRACE;

            type_specifier = VOID
                           | CHAR
                           | SHORT
                           | INT
                           | LONG
                           | FLOAT
                           | DOUBLE
                           | SIGNED
                           | UNSIGNED
                           | struct_or_union
                           | ENUM >> ! IDENTIFIER >> LEFT_BRACE >> (IDENTIFIER >> ! (ASSIGN >> conditional_expr)) % COMMA >> RIGHT_BRACE
                           | IDENTIFER [P::isType];

            type_qualifier = CONST | VOLATILE;

            struct_or_union = ( STRUCT
                              | UNION ) >> ( IDENTIFIER >> ! (LEFT_BRACE >> + struct_decl >> RIGHT_BRACE)
                                           | LEFT_BRACE >> + struct_decl >> RIGHT_BRACE );

            struct_decl = + ( type_specifier 
                            | type_qualifier ) >> struct_declarator % COMMA >> SEMICOLON;

            struct_declarator = declarator || (COLON >> conditional_expr);

            pointer = STAR >> ! (+ type_qualifier || pointer);

            atome = IDENTIFIER
                  | CONSTANT
                  | STRING_LIT
                  | LEFT_PAREN >> expr >> RIGHT_PAREN;

            postfix_expr = atome >> * ( LEFT_BRACKET >> expr >> RIGHT_BRACKET
                                      | LEFT_PAREN >> ! argument_expr_list >> RIGHT_PAREN
                                      | DOT >> IDENTIFIER
                                      | PTR_OP >> IDENTIFIER
                                      | INC_OP
                                      | DEC_OP );

            argument_expr_list = assign_expr >> * (COMMA >> assign_expr);

            unary_expr = postfix_expr
                       | (INC_OP | DEC_OP) >> unary_expr
                       | (ADDROF | STAR | PLUS | MINUS | TILDE | EXCLAM) >> cast_expr
                       | SIZEOF >> ( unary_expr
                                   | LEFT_PAREN >> type_name >> RIGHT_PAREN);

            cast_expr = LEFT_PAREN >> type_name >> RIGHT_PAREN >> cast_expr
                      | unary_expr;

            multiplicative_expr = cast_expr >> * (( STAR
                                                  | SLASH
                                                  | PERCENT) >> cast_expr);

            additive_expr = multiplicative_expr >> * ((PLUS | MINUS) >> multiplicative_expr);

            shift_expr = additive_expr >> * (( LEFT_OP
                                             | RIGHT_OP ) >> additive_expr);

            relational_expr = shift_expr >> * (( LT_OP
                                               | GT_OP
                                               | LE_OP
                                               | GE_OP ) >> shift_expr);

            equality_expr = relational_expr >> * (( EQ_OP
                                                  | NE_OP ) >> relational_expr);

            and_expr = equality_expr >> * (ADDROF >> equality_expr)

            xor_expr = and_expr >> * (XOR >> and_expr);

            or_expr = xor_expr >> * (OR >> xor_expr);

            logical_and_expr = or_expr >> * (AND_OP >> or_expr);

            logical_or_expr = logical_and_expr >> * (OR_OP >> logical_and_expr);

            conditional_expr = logical_or_expr >> * (QUEST >> expr >> COLON >> conditional_expr);

            assign_expr = unary_expr >> ( ASSIGN
                                        | MUL_ASSIGN
                                        | DIV_ASSIGN
                                        | MOD_ASSIGN
                                        | ADD_ASSIGN
                                        | SUB_ASSIGN
                                        | LEFT_ASSIGN
                                        | RIGHT_ASSIGN
                                        | AND_ASSIGN
                                        | XOR_ASSIGN
                                        | OR_ASSIGN ) >> assign_expr
                        | conditional_expr;

            expr = assign_expr % COMMA;

            parameter_type_list = parameter_decl % COMMA >> ! (COMMA >> ELLIPSIS);

            parameter_decl = decl_specifiers >> ! ( declarator
                                                  | abstract_declarator );

            type_name = + ( type_specifier | type_qualifier ) >> ! abstract_declarator;

            labeled_stmt = ( IDENTIFIER
                           | CASE >> conditional_expr
                           | DEFAULT ) >> COLON >> stmt;

            expr_stmt = ! expr >> SEMICOLON;

            if_stmt = IF >> LEFT_PAREN >> expr >> RIGHT_PAREN >> stmt
                      >> ! (ELSE >> stmt)

            switch_stmt = SWITCH >> LEFT_PAREN >> expr >> RIGHT_PAREN >> stmt;

            iteration_stmt = WHILE >> LEFT_PAREN >> expr >> RIGHT_PAREN >> stmt
                           | DO >> stmt >> WHILE >> LEFT_PAREN >> expr >> RIGHT_PAREN >> SEMICOLON
                           | FOR >> LEFT_PAREN >> expr_stmt >> expr_stmt >> ! expr >> RIGHT_PAREN >> stmt;

            jump_stmt = ( GOTO >> IDENTIFIER
                        | CONTINUE
                        | BREAK
                        | RETURN >> ! expr ) >> SEMICOLON;

#           define D(rule) rule.set_name(#rule);
            RULES
#           undef D
        }

        Rule<Lex> root
#       define D(rule) , rule
        RULES;
#       undef D

#       define D(NAME) Ch<Lex> NAME;
#       include "c_tokens.def"

    };
}
