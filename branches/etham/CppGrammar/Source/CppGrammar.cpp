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

#include "CppGrammar.h"

#define kw_p(k) lexeme_d(str_p(k) >> check_d(! alnum_p))
#define typedef_name identifier
#define namespace_name identifier
#define hex_quad integer_p(unsigned, 16, 4, 4)
#define enum_name identifier
#define template_name identifier

CppLexer::CppLexer()
{
    token = lexeme_d(header_name |
                     integer_literal |
                     character_literal |
                     floating_literal |
                     string_literal |
                     (kw_p("false") | kw_p("true")) |
                     punctuation |
                     identifier);

    header_name = '<' >> (any - '\n') * '>' | '\"' >> (any - '\n') * '\"';

    identifier = alpha_p >> * alnum_p;

    punctuation = (str_p("new") | str_p("delete") |
                   str_p("and_eq") | str_p("and") |
                   str_p("bitand") | str_p("bitor") | str_p("compl") |
                   str_p("not_eq") | str_p("not") |
                   str_p("or_eq") | str_p("or") |
                   str_p("xor_eq") | str_p("xor")) >> check_d(any_p - alnum_p) |
                   "..." | "->*" | "->" | "--" | "++" | " ||" | "&&" |
                   "##" | "<:" | ":>" | "<%" | "%>" | "%:" | "%:%:" | "::" | ".*" |
                   ("<<" | ">>" | '!' | '<' | '>' | '+' | '-' | '*' | '/' | '%' | '^' | '&' | '|' | '=') >> ! ch_p('=') |
                   '{' | '}' | '[' | ']' | '#' | '(' | ')' | ';' | ':' | '?' | '.' | '~' | ',';

    integer_literal = (str_p("0x") >> hex_p |
                       ch_p('0') >> oct_p |
                       uint_p) >> ! (set_p("uU") >> ! set_p("lL") |
                                     set_p("lL") >> ! set_p("uU"));

    character_literal = lexeme_d(! ch_p('L') >> ch_p('\'') >> c_char * ch_p('\''));

    string_literal = lexeme_d(! ch_p('L') >> ch_p('\"') >> c_char * ch_p('\"'));

    c_char = ch_p('\\') >> (set_p("abfnrtv?\"\'\\") |
                            integer_p(unsigned, 8, 1, 3) |
                            ch_p('x') >> integer_p(unsigned, 16, 1, 2) |
                            ch_p('u') >> hex_quad |
                            ch_p('U') >> hex_quad >> hex_quad) |
             any_p - ch_p('\n');

    floating_literal = real_p >> ! set_p("flFL");
}

CppGrammar::CppGrammar()
{
    //1.1  Keywords
    class_name = identifier | template_id;


    //1.4  Expressions
    primary_expression = literal |
                         kw_p("this") |
            :: identifier
            :: operator_function_id
            :: qualified_id
            ( expression )
            id_expression

    id_expression =
            unqualified_id
            qualified_id
    id_expression =
            unqualified_id
            qualified_id

    unqualified_id =
            identifier
            operator_function_id
            conversion_function_id
            ~ class_name
            template_id
    qualified_id =
            nested_name_specifier ! template unqualified_id
    nested_name_specifier =
            class_or_namespace_name :: ! nested_name_specifier

    class_or_namespace_name =
            class_name
            namespace_name
    postfix_expression =
            primary_expression
            postfix_expression [ expression ]
            postfix_expression ( ! expression_list )
            simple_type_specifier ( ! expression_list )
            postfix_expression . ! template ::opt id_expression
            postfix_expression -> ! template ::opt id_expression
            postfix_expression . pseudo_destructor_name
            postfix_expression -> pseudo_destructor_name
            postfix_expression ++
            postfix_expression --
            dynamic_cast < type_id > ( expression )
            static_cast < type_id > ( expression )
            reinterpret_cast < type_id > ( expression )
            const_cast < type_id > ( expression )
            typeid ( expression )
            typeid ( type_id )

    expression_list =
            assignment_expression
            expression_list , assignment_expression
    pseudo_destructor_name =
            ::opt ! nested_name_specifier type_name :: ~ type_name
            ::opt ! nested_name_specifier ~ type_name
    unary_expression =
            postfix_expression
            ++  cast_expression
            --  cast_expression
            unary_operator cast_expression
            sizeof unary_expression
            sizeof ( type_id )
            new_expression
            delete_expression
    unary_operator = one of
            *  &  +  -  !  ~
    new_expression =
            ::opt new ! new_placement new_type_id ! new_initializer
            ::opt new ! new_placement ( type_id ) ! new_initializer
    new_placement =
            ( expression_list )
    new_type_id =
            type_specifier_seq ! new_declarator
    new_declarator =
            ptr_operator ! new_declarator
            direct_new_declarator
    direct_new_declarator =
            [ expression ]
            direct_new_declarator [ constant_expression ]
    new_initializer =
            ( ! expression_list )
    delete_expression =
            ::opt delete cast_expression
            ::opt delete [ ] cast_expression
    cast_expression =
            unary_expression
            ( type_id ) cast_expression
    pm_expression =
            cast_expression
            pm_expression .* cast_expression
            pm_expression ->* cast_expression
    multiplicative_expression =
            pm_expression
            multiplicative_expression * pm_expression
            multiplicative_expression / pm_expression
            multiplicative_expression % pm_expression
    additive_expression =
            multiplicative_expression
            additive_expression + multiplicative_expression
            additive_expression - multiplicative_expression

    shift_expression =
            additive_expression
            shift_expression << additive_expression
            shift_expression >> additive_expression
    relational_expression =
            shift_expression
            relational_expression < shift_expression
            relational_expression > shift_expression
            relational_expression <= shift_expression
            relational_expression >= shift_expression
    equality_expression =
            relational_expression
            equality_expression == relational_expression
            equality_expression != relational_expression
    and_expression =
            equality_expression
            and_expression & equality_expression
    exclusive_or_expression =
            and_expression
            exclusive_or_expression ^ and_expression
    inclusive_or_expression =
            exclusive_or_expression
            inclusive_or_expression | exclusive_or_expression
    logical_and_expression =
            inclusive_or_expression
            logical_and_expression && inclusive_or_expression
    logical_or_expression =
            logical_and_expression
            logical_or_expression || logical_and_expression
    conditional_expression = logical_or_expression >> ! (ch_p('?') >> expression >> ch_p(':') >> assignment_expression);
    assignment_expression =
            conditional_expression |
            logical_or_expression >> assignment_operator >> assignment_expression |
            throw_expression;
    assignment_operator = one of
            =  *=  /=  %=   +=  -=  >>=  <<=  &=  ^=  |=
    expression =
            assignment_expression
            expression , assignment_expression
    constant_expression =
            conditional_expression

//1.5  Statements
    statement =
            labeled_statement
            expression_statement
            compound_statement
            selection_statement
            iteration_statement
            jump_statement
            declaration_statement
            try_block

    labeled_statement =
            identifier : statement
            case constant_expression : statement
            default : statement
    expression_statement =
            ! expression ;
    compound_statement =
             { ! statement_seq }
    statement_seq =
            statement
            statement_seq statement
    selection_statement =
            if ( condition ) statement
            if ( condition ) statement else statement
            switch ( condition ) statement
    condition =
            expression
            type_specifier_seq declarator = assignment_expression
    iteration_statement =
            while ( condition ) statement
            do statement  while ( expression ) ;
            for ( for_init_statement ! condition ; ! expression ) statement
    for_init_statement =
            expression_statement
            simple_declaration
    jump_statement =
            break ;
            continue ;
            return ! expression ;
            goto identifier ;
    declaration_statement =
            block_declaration

    //1.6  Declarations
    declaration_seq = + declaration;
    declaration = block_declaration |
                  function_definition |
                  template_declaration |
                  explicit_instantiation |
                  explicit_specialization |
                  linkage_specification |
                  namespace_definition;
    block_declaration =
            simple_declaration
            asm_definition
            namespace_alias_definition
            using_declaration
            using_directive
    simple_declaration =
            ! decl_specifier_seq ! init_declarator_list ;

    decl_specifier =
            storage_class_specifier
            type_specifier
            function_specifier
            friend
            typedef
    decl_specifier_seq =
            ! decl_specifier_seq decl_specifier
    storage_class_specifier =
            auto
            register
            static
            extern
            mutable
    function_specifier =
            inline
            virtual
            explicit
    typedef_name =
            identifier
    type_specifier =
            simple_type_specifier
            class_specifier
            enum_specifier
            elaborated_type_specifier
            cv_qualifier
    simple_type_specifier =
            ::opt ! nested_name_specifier type_name
            char
            wchar_t
            bool
            short
            int
            long
            signed
            unsigned
            float
            double
            void
    type_name =
            class_name
            enum_name
            typedef_name
    elaborated_type_specifier =
            class_key ::opt ! nested_name_specifier identifier
            enum ::opt ! nested_name_specifier identifier
            typename ::opt nested_name_specifier identifier
            typename ::opt nested_name_specifier identifier < template_argument_list >
    enum_name =
            identifier
    enum_specifier =
            enum ! identifier { ! enumerator_list }

    enumerator_list =
            enumerator_definition
            enumerator_list , enumerator_definition
    enumerator_definition =
            enumerator
            enumerator = constant_expression
    enumerator =
            identifier
    namespace_name =
            original_namespace_name
            namespace_alias
    original_namespace_name =
            identifier

    namespace_definition =
            named_namespace_definition
            unnamed_namespace_definition

    named_namespace_definition =
            original_namespace_definition
            extension_namespace_definition

    original_namespace_definition =
            namespace identifier { namespace_body }

    extension_namespace_definition =
            namespace original_namespace_name  { namespace_body }

    unnamed_namespace_definition =
            namespace { namespace_body }

    namespace_body =
            ! declaration_seq
    namespace_alias =
            identifier

    namespace_alias_definition =
            namespace identifier = qualified_namespace_specifier ;

    qualified_namespace_specifier =
            ::opt ! nested_name_specifier namespace_name
    using_declaration =
            using ! typename ::opt nested_name_specifier unqualified_id ;
            using ::  unqualified_id ;
    using_directive =
            using  namespace ::opt ! nested_name_specifier namespace_name ;
    asm_definition =
            asm ( string_literal ) ;
    linkage_specification =
            extern string_literal { ! declaration_seq }
            extern string_literal declaration

//1.7  Declarators
    init_declarator_list =
            init_declarator
            init_declarator_list , init_declarator
    init_declarator =
            declarator ! initializer
    declarator =
            direct_declarator
            ptr_operator declarator
    direct_declarator =
            declarator_id
            direct_declarator ( parameter_declaration_clause ) ! cv_qualifier_seq ! exception_specification
            direct_declarator [ ! constant_expression ]
            ( declarator )
    ptr_operator =
            * ! cv_qualifier_seq
            &
            ::opt nested_name_specifier * ! cv_qualifier_seq
    cv_qualifier_seq =
            cv_qualifier ! cv_qualifier_seq
    cv_qualifier =
            const
            volatile
    declarator_id =
            ::opt id_expression
            ::opt ! nested_name_specifier type_name
    type_id =
            type_specifier_seq ! abstract_declarator
    type_specifier_seq =
            type_specifier ! type_specifier_seq
    abstract_declarator =
            ptr_operator ! abstract_declarator
            direct_abstract_declarator
    direct_abstract_declarator =
            ! direct_abstract_declarator ( parameter_declaration_clause ) ! cv_qualifier_seq ! exception_specification
            ! direct_abstract_declarator [ ! constant_expression ]
            ( abstract_declarator )
    parameter_declaration_clause =
            ! parameter_declaration_list ...opt
            parameter_declaration_list , ...
    parameter_declaration_list =
            parameter_declaration
            parameter_declaration_list , parameter_declaration
    parameter_declaration =
            decl_specifier_seq declarator
            decl_specifier_seq declarator = assignment_expression
            decl_specifier_seq ! abstract_declarator
            decl_specifier_seq ! abstract_declarator = assignment_expression
    function_definition =
            ! decl_specifier_seq declarator ! ctor_initializer function_body
            ! decl_specifier_seq declarator function_try_block

    function_body =
            compound_statement

    initializer =
            = initializer_clause
            ( expression_list )
    initializer_clause =
            assignment_expression
            { initializer_list ,opt }
            { }
    initializer_list =
            initializer_clause
            initializer_list , initializer_clause

//1.8  Classes
    class_name =
            identifier
            template_id
    class_specifier =
            class_head { ! member_specification }
    class_head =
            class_key ! identifier ! base_clause
            class_key nested_name_specifier identifier ! base_clause
    class_key =
            class
            struct
            union
    member_specification =
            member_declaration ! member_specification
            access_specifier : ! member_specification
    member_declaration =
            ! decl_specifier_seq ! member_declarator_list ;
            function_definition ;opt
            qualified_id ;
            using_declaration
            template_declaration
    member_declarator_list =
            member_declarator
            member_declarator_list , member_declarator
    member_declarator =
            declarator ! pure_specifier
            declarator ! constant_initializer
            ! identifier : constant_expression
    pure_specifier =
             = 0
    constant_initializer =
             = constant_expression

//1.9  Derived classes
    base_clause =
            : base_specifier_list
    base_specifier_list =
            base_specifier
            base_specifier_list , base_specifier

    base_specifier =
            ::opt ! nested_name_specifier class_name
            virtual ! access_specifier ::opt ! nested_name_specifier class_name
            access_specifier ! virtual ::opt ! nested_name_specifier class_name
    access_specifier =
            private
            protected
            public

//1.10  Special member functions
    conversion_function_id =
            op conversion_type_id
    conversion_type_id =
            type_specifier_seq ! conversion_declarator
    conversion_declarator =
            ptr_operator ! conversion_declarator
    ctor_initializer =
            : mem_initializer_list
    mem_initializer_list =
            mem_initializer
            mem_initializer , mem_initializer_list
    mem_initializer =
            mem_initializer_id ( ! expression_list )
    mem_initializer_id =
            ::opt ! nested_name_specifier class_name
            identifier

//1.11  Overloading
    operator_function_id =
            kw_p("operator") >> op
    op = kw_p("new")
            new  delete    new[]     delete[]
            +    -    *    /    %    ^    &    |    ~
            !    =    <    >    +=   -=   *=   /=   %=
            ^=   &=   |=   <<   >>   >>=  <<=  ==   !=
            <=   >=   &&   ||   ++   --   ,    ->*  ->
            ()   []

//1.12  Templates
    template_declaration =
            ! export template < template_parameter_list > declaration
    template_parameter_list =
            template_parameter
            template_parameter_list , template_parameter
    template_parameter =
            type_parameter
            parameter_declaration
    type_parameter =
            class ! identifier
            class ! identifier = type_id
            typename ! identifier
            typename ! identifier = type_id
            template < template_parameter_list > class  ! identifier
            template < template_parameter_list > class  ! identifier = template_name

    template_id = template_name >> ch_p('<') >> template_argument_list >> ch_p('>');

    template_name = identifier;
    template_argument_list = (assignment_expression |
                              type_id |
                              template_name) % ch_p(',');

    explicit_instantiation =
            template_declaration
    explicit_specialization =
            template < > declaration

//1.13  Exception handling
    try_block =
             try compound_statement handler_seq
    function_try_block =
             try  ! ctor_initializer function_body handler_seq
    handler_seq =
            handler ! handler_seq
    handler =
            catch ( exception_declaration ) compound_statement
    exception_declaration =
            type_specifier_seq declarator
            type_specifier_seq abstract_declarator
            type_specifier_seq
            ...
    throw_expression =
            throw ! assignment_expression
    exception_specification =
            throw ( ! type_id_list )
    type_id_list =
            type_id
            type_id_list ,  type_id

//1.14  Preprocessing directives
    preprocessing_file =
            * group_part
    group_part =
            ! pp_tokens new_line
            if_section
            control_line
    if_section =
            if_group ! elif_groups ! else_group endif_line
    if_group =
            '#' "if"     constant_expression new_line ! group
            '#' "ifdef"  identifier new_line ! group
            '#' "ifndef" identifier new_line ! group

    elif_groups =
            elif_group
            elif_groups elif_group
    elif_group =
            '#' "elif"   constant_expression new_line ! group
    else_group =
            '#' "else"   new_line ! group
    endif_line =
            '#' "endif"  new_line
    control_line =
            '#' "include" pp_tokens new_line
            '#' "define"  identifier replacement_list new_line
            '#' "define"  identifier lparen ! identifier_list ')' replacement_list new_line
            '#' "undef"   identifier new_line
            '#' "line"    pp_tokens new_line
            '#' "error"   ! pp_tokens new_line
            '#' "pragma"  ! pp_tokens new_line
            '#'           new_line
    lparen =
            the left_parenthesis character without preceding white_space
    replacement_list = ! pp_tokens;
    pp_tokens = + preprocessing_token;
    new_line = '\n';

