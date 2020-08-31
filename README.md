KFTamang's C Compiler
=====================

KFC is a simple C language compiler, written by KFTamang.
KFC is intented to be simple, naive in its implementation.

It takes a text file written in the subset of C language as an input, and spits out assembly launguage file for the x86_64 processor.

KFC is currently under development, partially supporting C language grammar. 
More and more syntax will be supported in the future.
One milestone is self-hosting itself(i.e. compiling the source file of the compiler to reproduce itself).

# Currently Supported Syntax
```
program = global_dec*
global_dec = type_dec ident "(" lvar_dec? ")" "{" stmt* "}"
                 | type_dec ident ";"
                 | type_dec ";"
                 | type_def ";"
stmt = expr ";" | "return" expr ";"
           | "{" stmt* "}"
           | "if" "(" expr ")" stmt ("else" stmt)?
           | "while" "(" expr ")" stmt
           | "for" "(" expr? ";" expr? ";" expr? ")" stmt
           | var_dec ";"
           | type_def ";"
           | "switch" "(" expr "){" ( "case" num ":" stmt )* "}"
           | "break" ";"
type_dec = ("int" | "char" | struct_dec | enum_dec) "*"*
var_dec = type_dec var ("[" num "]")?
              | type_dec var ("[" num "]")? = expr
              | type_dec
expr = assign
assign = logical ("=" assign)?
logical = equality ( "&&" equality ) *
equality = relational ( "==" relational | "!=" relatinal ) *
relational = add ("<" add | "<=" add | ">" add | ">=" add) *
add = mul ( "+" mul | "-" mul )*
mul = unary ( "*" unary | "/" unary )*
primary = ( expr ) | num | ident | string-literal
postfix = prefix
              | postfix "[" expr "]"
              | postfix "." ident
              | postfix "->" ident
prefix = primary
             | "++" primary
ident = var | func
func = ident ( "(" (expr (, expr)*)? ")" )
unary = "+"? postfix
            | "-"  postfix
            | "!"  postfix
            | "&"  unary
            | "*"  unary
            | "sizeof" (unary | type_dec | "(" type_dec ")" )
string-literal = '"' (character | num ) '"'
var = character* (character | num)*
num = ("0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9")*
character = ("a"-"z" | "A"-"Z" | "_")* 
```
# reference
* [低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook)

* [rui314/8cc](https://github.com/rui314/8cc)