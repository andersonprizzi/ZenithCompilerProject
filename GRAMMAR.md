# Formal grammar of the C programming language
This formal grammar was developed by Anderson Pastore Rizzi and is based on the ANSI C grammar presented by Jeff Lee.



## Grammar rules for declaration and type analysis

```bnf
storage_class_specifier 
    : TokenAuto
    | TokenExtern
    | TokenRegister
    | TokenStatic
    | TokenTypedef

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

composite_type_specifier 
    : TokenStruct
    | TokenUnion
    | TokenEnum

type_qualifier 
    : TokenConst
    | TokenVolatile
    | TokenRestrict
    | TokenAtomic
```
