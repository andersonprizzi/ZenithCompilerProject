# Formal grammar of the C programming language
This formal grammar was developed by Anderson Pastore Rizzi and is based on the ANSI C grammar originally presented by Jeff Lee. The initial production rule is translation_unit, which serves as the entry point for parsing. The symbol ε represents an empty production. For a complete list of tokens used in this grammar, refer to the TOKENS.md file.



## Grammar rule for the fundamental unit of analysis
```bnf
translation_unit
    : external_declaration translation_unit_sequence
    ;
```



## Grammar rules for program external structure
```bnf
translation_unit_sequence
    : external_declaration translation_unit_sequence
    | ε
    ;


external_declaration
    : external_definition
    | function_definition
    ;


function_definition
    : declaration_specifiers declarator compound_statement
    ;


external_definition
    : declaration_specifiers declaration_suffix
    ;
```



## Grammar rules for declaration
```bnf
declaration_specifier
    : storage_specifier
    | type_specifier
    | type_qualifier
    ;


declaration_specifiers
    : declaration_specifier declaration_specifiers_sequence
    ;


declaration_suffix
    : TokenSemicolon
    | declaration_list TokenSemicolon
    ;


declaration_specifiers_sequence
    : declaration_specifier declaration_specifiers_sequence
    | ε
    ;


declaration_list
    : init_declarator declaration_list_sequence
    ;


declaration_list_sequence
    : TokenComma init_declarator declaration_list_sequence
    | ε
    ;


init_declarator
    : declarator init_declarator_suffix
    ;


init_declarator_suffix
    : TokenAssign initializer
    | ε
    ;
```



## Grammar rules for declarators
```bnf
declarator
    : pointer_declarator
    | direct_declarator
    ;


pointer_declarator
    : pointer direct_declarator
    ;


direct_declarator
    : direct_declarator_base direct_declarator_sequence
    ;


direct_declarator_base
    : TokenIdentifier
    | TokenOpenParentheses declarator TokenCloseParentheses
    ;


direct_declarator_sequence
    : direct_declarator_suffix direct_declarator_sequence
    | ε
    ;


direct_declarator_suffix
    : TokenOpenBracket constant_expression TokenCloseBracket
    | TokenOpenBracket TokenCloseBracket
    | TokenOpenParentheses parameter_type_list TokenCloseParentheses
    | TokenOpenParentheses identifier_list TokenCloseParentheses
    | TokenOpenParentheses TokenCloseParentheses
    ;


abstract_declarator
    : pointer abstract_declarator_sequence
    | direct_abstract_declarator
    ;


abstract_declarator_sequence
    : direct_abstract_declarator
    | ε
    ;


direct_abstract_declarator
    : direct_abstract_declarator_base direct_abstract_declarator_sequence
    ;


direct_abstract_declarator_base
    : TokenOpenParentheses abstract_declarator TokenCloseParentheses
    | TokenOpenBracket TokenCloseBracket
    | TokenOpenBracket constant_expression TokenCloseBracket
    | TokenOpenParentheses TokenCloseParentheses
    | TokenOpenParentheses parameter_type_list TokenCloseParentheses
    ;


direct_abstract_declarator_sequence
    : direct_abstract_declarator_suffix direct_abstract_declarator_sequence
    | ε
    ;


direct_abstract_declarator_suffix
    : TokenOpenBracket TokenCloseBracket
    | TokenOpenBracket constant_expression TokenCloseBracket
    | TokenOpenParentheses TokenCloseParentheses
    | TokenOpenParentheses parameter_type_list TokenCloseParentheses
    ;


pointer
    : TokenAsterisk pointer_suffix
    ;


pointer_suffix
    : type_qualifier_list pointer
    | type_qualifier_list
    | pointer
    | ε
    ;
```



## Grammar rules for parameters
```bnf
parameter_type_list
    : parameter_list parameter_type_sequence
    ;


parameter_type_sequence
    : TokenComma TokenEllipsis
    | ε
    ;


parameter_list
    : parameter_declaration parameter_sequence
    ;


parameter_sequence
    : TokenComma parameter_declaration parameter_sequence
    | ε
    ;


parameter_declaration
    : declaration_specifiers parameter_declaration_suffix
    ;


parameter_declaration_suffix
    : declarator
    | abstract_declarator
    | ε
    ;
```



## Grammar rules for identifiers and initializers
```bnf
identifier_list
    : TokenIdentifier identifier_list_sequence
    ;

identifier_list_sequence
    : TokenComma TokenIdentifier identifier_list_sequence
    | ε
    ;

initializer
    : assignment_expression
    | TokenOpenBrace initializer_list initializer_suffix
    ;

initializer_suffix
    : TokenComma TokenCloseBrace
    | TokenCloseBrace
    ;

initializer_list
    : initializer initializer_sequence
    ;

initializer_sequence
    : TokenComma initializer initializer_sequence
    | ε
    ;
```



## Grammar rules for structures and enumerations
```bnf
struct_or_union_specifier
    : TokenStruct struct_or_union_body
    | TokenUnion struct_or_union_body
    ;


struct_or_union_body
    : TokenIdentifier TokenOpenBrace field_declaration_list TokenCloseBrace
    | TokenOpenBrace field_declaration_list TokenCloseBrace
    | TokenIdentifier
    ;


field_declaration_list
    : struct_declaration field_declaration_list_sequence
    ;


field_declaration_list_sequence
    : struct_declaration field_declaration_list_sequence
    | ε
    ;


struct_declaration
    : specifier_qualifier_list struct_declarator_list TokenSemicolon
    ;


enumerator_specifier
    : TokenEnum enumerator_body
    ;


enumerator_body
    : TokenOpenBrace enumerator_list TokenCloseBrace
    | TokenIdentifier TokenOpenBrace enumerator_list TokenCloseBrace
    ;


enumerator_list
    : enumerator enumerator_list_sequence
    ;


enumerator_list_sequence
    : TokenComma enumerator enumerator_list_sequence
    | ε
    ;


enumerator
    : TokenIdentifier enumerator_suffix
    ;


enumerator_suffix
    : TokenAssign constant_expression
    | ε
    ;
```



## Grammar rules for statements
```bnf
compound_statement
    : TokenOpenBrace TokenCloseBrace
    | TokenOpenBrace statement_list TokenCloseBrace
    | TokenOpenBrace declaration_list TokenCloseBrace
    | TokenOpenBrace declaration_list statement_list TokenCloseBrace
    ;


statement_list
    : statement statement_list_sequence
    ;


statement_list_sequence
    : statement statement_list_sequence
    | ε
    ;


statement
    : labeled_statement
    | compound_statement
    | expression_statement
    | selection_statement
    | iteration_statement
    | jump_statement
    ;


labeled_statement
    : TokenIdentifier TokenColon statement
    | TokenCase constant_expression TokenColon statement
    | TokenDefault TokenColon statement
    ;


declaration_list
    : declaration declaration_list_sequence
    ;


declaration_list_sequence
    : declaration declaration_list_sequence
    | ε
    ;


expression_statement
    : TokenSemicolon
    | expression TokenSemicolon
    ;


selection_statement
    : TokenIf TokenOpenParentheses expression TokenCloseParentheses statement selection_statement_else_opt
    | TokenSwitch TokenOpenParentheses expression TokenCloseParentheses statement
    ;


selection_statement_else_opt
    : TokenElse statement
    | ε
    ;


iteration_statement
    : TokenWhile TokenOpenParentheses expression TokenCloseParentheses statement
    | TokenDo statement TokenWhile TokenOpenParentheses expression TokenCloseParentheses TokenSemicolon
    | TokenFor TokenOpenParentheses expression_statement expression_statement TokenCloseParentheses statement
    | TokenFor TokenOpenParentheses expression_statement expression_statement expression TokenCloseParentheses statement
    ;


jump_statement
    : TokenGoto TokenIdentifier TokenSemicolon
    | TokenContinue TokenSemicolon
    | TokenBreak TokenSemicolon
    | TokenReturn TokenSemicolon
    | TokenReturn expression TokenSemicolon
    ;
```



## Grammar rules for expressions
```bnf
constant_expression
    : conditional_expression
    ;


argument_expression_list
    : assignment_expression argument_expression_sequence
    ;


argument_expression_sequence
    : ',' assignment_expression argument_expression_sequence
    | ε
    ;


assignment_expression 
    : conditional_expression
    | unary_expression assignment_operator assignment_expression
    ;


conditional_expression
    : logical_or_expression conditional_expression_suffix
    ;


conditional_expression_suffix
    : TokenQuestionMark expression TokenColon conditional_expression
    | ε
    ;


logical_or_expression
    : logical_and_expression logical_or_expression_suffix
    ;


logical_or_expression_suffix
    : TokenLogicalOr logical_and_expression logical_or_expression_suffix
    | ε
    ;


logical_and_expression
    : inclusive_or_expression logical_and_expression_suffix
    ;


logical_and_expression_suffix
    : TokenLogicalAnd inclusive_or_expression logical_and_expression_suffix
    | ε
    ;


inclusive_or_expression
    : exclusive_or_expression inclusive_or_expression_suffix
    ;


inclusive_or_expression_suffix
    : TokenBitwiseOr exclusive_or_expression inclusive_or_expression_suffix
    | ε
    ;


exclusive_or_expression
    : and_expression exclusive_or_expression_suffix
    ;


exclusive_or_expression_suffix
    : TokenBitwiseXor and_expression exclusive_or_expression_suffix
    | ε
    ;


and_expression
    : equality_expression and_expression_suffix
    ;


and_expression_suffix
    : TokenBitwiseAnd_AddressOf equality_expression and_expression_suffix
    | ε
    ;


equality_expression
    : relational_expression equality_expression_suffix
    ;


equality_expression_suffix
    : TokenEqual relational_expression equality_expression_suffix
    | TokenNotEqual relational_expression equality_expression_suffix
    | ε
    ;


relational_expression
    : shift_expression relational_expression_suffix
    ;


relational_expression_suffix
    : TokenLessThan shift_expression relational_expression_suffix
    | TokenGreaterThan shift_expression relational_expression_suffix
    | TokenLessEqual shift_expression relational_expression_suffix
    | TokenGreaterEqual shift_expression relational_expression_suffix
    | ε
    ;


shift_expression
    : additive_expression shift_expression_suffix
    ;


shift_expression_suffix
    : TokenLeftShift additive_expression shift_expression_suffix
    | TokenRightShift additive_expression shift_expression_suffix
    | ε
    ;


additive_expression
    : multiplicative_expression additive_expression_suffix
    ;


additive_expression_suffix
    : TokenPlus multiplicative_expression additive_expression_suffix
    | TokenMinus multiplicative_expression additive_expression_suffix
    | ε
    ;


multiplicative_expression
    : cast_expression multiplicative_expression_suffix
    ;


multiplicative_expression_suffix
    : TokenAsterisk cast_expression multiplicative_expression_suffix
    | TokenDivision cast_expression multiplicative_expression_suffix
    | TokenMod cast_expression multiplicative_expression_suffix
    | ε
    ;


cast_expression
    : unary_expression
    | TokenOpenParentheses type_name TokenCloseParentheses cast_expression
    ;


unary_expression
    : postfix_expression
    | TokenIncrement unary_expression
    | TokenDecrement unary_expression
    | unary_operator cast_expression
    | TokenSizeof unary_expression
    | TokenSizeof TokenOpenParentheses type_name TokenCloseParentheses
    ;


postfix_expression
    : primary_expression postfix_expression_sequence
    ;


postfix_expression_sequence
    : postfix_expression_suffix postfix_expression_sequence
    | ε
    ;


postfix_expression_suffix
    : TokenOpenBracket expression TokenCloseBracket
    | TokenOpenParentheses TokenCloseParentheses
    | TokenOpenParentheses argument_expression_list TokenCloseParentheses
    | TokenDot TokenIdentifier
    | TokenArrow TokenIdentifier
    | TokenIncrement
    | TokenDecrement
    ;


primary_expression
    : TokenIdentifier
    | TokenIntConst
    | TokenFloatConst
    | TokenString
    | TokenOpenParentheses expression TokenCloseParentheses
    ;
```



## Grammar rules for type specifiers, qualifiers, and operators
```bnf
type_specifier
    : primitive_type_specifier
    | composite_type_specifier
    ;


primitive_type_specifier
    : TokenVoid
    | TokenChar
    | TokenShort
    | TokenInt
    | TokenLong
    | TokenFloat
    | TokenDouble
    | TokenBool
    | TokenComplex
    ;


composite_type_specifier
    : struct_or_union_specifier
    | enumerator_specifier
    ;


storage_specifier
    : TokenAuto
    | TokenExtern
    | TokenRegister
    | TokenStatic
    | TokenTypedef
    ;


type_qualifier
    : TokenConst
    | TokenVolatile
    | TokenRestrict
    | TokenAtomic
    ;


type_qualifier_list
    : type_qualifier type_qualifier_list_sequence
    ;


type_qualifier_list_sequence
    : type_qualifier type_qualifier_list_sequence
    | ε
    ;


specifier_qualifier_list
    : specifier_qualifier specifier_qualifier_list_sequence
    ;


specifier_qualifier_list_sequence
    : specifier_qualifier specifier_qualifier_list_sequence
    | ε
    ;


specifier_qualifier
    : type_specifier
    | type_qualifier
    ;


type_name
    : specifier_qualifier_list type_name_suffix
    ;


type_name_suffix
    : abstract_declarator
    | ε
    ;


unary_operator
    : TokenBitwiseAnd_AddressOf
    | TokenAsterisk
    | TokenPlus
    | TokenMinus
    | TokenBitwiseNot
    | TokenLogicalNot
    ;


assignment_operator
    : TokenAssign
    | TokenMulAssign
    | TokenDivAssign
    | TokenModAssign
    | TokenPlusAssign
    | TokenMinusAssign
    | TokenLeftShiftAssign
    | TokenRightShiftAssign
    | TokenAndAssign
    | TokenXorAssign
    | TokenOrAssign
    ;
```



## Grammar precedence table

| Production rule             | Precende level       | Associativity       |
|-----------------------------|----------------------|---------------------|
| assignment_expression       | 15                   | right-to-left       |
| conditional_expression      | 14                   | right-to-left       |
| logical_or_expression       | 13                   | left-to-right       |
| logical_and_expression      | 12                   | left-to-right       |
| inclusive_or_expression     | 11                   | left-to-right       |
| exclusive_or_expression     | 10                   | left-to-right       |
| and_expression              | 9                    | left-to-right       |
| equality_expression         | 8                    | left-to-right       |
| relational_expression       | 7                    | left-to-right       |
| shift_expression            | 6                    | left-to-right       |
| additive_expression         | 5                    | left-to-right       |
| multiplicative_expression   | 4                    | left-to-right       |
| cast_expression             | 3                    | right-to-left       |
| unary_expression            | 2                    | right-to-left       |
| postfix_expression          | 1                    | left-to-right       |



## Grammar observations

| Production rule           | Observations                                                               |
|---------------------------|----------------------------------------------------------------------------|
| assignment_expression     | Requires backtracking (ambiguity with conditional expression)              |
| unary_expression          | Requires backtracking (ambiguity with postfix_expression and use of sizeof)|
