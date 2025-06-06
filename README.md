## ZUST

### Introduction
Zust is programming language. Zust is a compiled langauge run on a Zust Machine.
Zust Machine is a Virtual Machine.

Zust is tiny, easy to use.


# Grammer

<LETTER>         ::= 'A' | 'B' | ... | 'Z' | 'a' | 'b' | ... | 'z' | '_'
<DIGIT>          ::= '0' | '1' | ... | '9'
<HEX_DIGIT>      ::= '0' | ... | '9' | 'A' | ... | 'F' | 'a' | ... | 'f'

<IDENT>          ::= <LETTER> { <LETTER> | <DIGIT> | '_' }

# Int Literal
<INT_LITERAL>    ::= <DIGIT> { <DIGIT> }
                   | '0' ( 'x' | 'X' ) <HEX_DIGIT> { <HEX_DIGIT> }

# Char Literal
<CHAR_LITERAL>   ::= '\'' ( <ESC_CHAR> | <NORMAL_CHAR> ) '\''

# String Literal
<STRING_LITERAL> ::= '"' { <ESC_CHAR> | <NORMAL_CHAR_EXCEPT_QUOTE> } '"'

# Byte Literal
<BYTE_LITERAL>   ::= '0' ( 'x' | 'X' ) <HEX_DIGIT> <HEX_DIGIT>

# Boolean Literal
<BOOL_LITERAL>   ::= 'true' | 'false'

# Escape Character
<ESC_CHAR>       ::= '\' ( 'n' | 't' | 'r' | '\\' | '\'' | '"' | '0' )

# Normal Character
<NORMAL_CHAR>            ::= any character except '\'' and '\''
<NORMAL_CHAR_EXCEPT_QUOTE>::= any character except '"' and '\'

# Keyword:
'let'      'fn'      'if'       'else'      'while'    'for'     'foreach'
'switch'   'case'    'default'  'break'     'continue' 'return'  'true'
'false'    'class'   'struct'   'namespace' 'import'   'try'     'catch'
'throw'    'const'     'static'   'public'  'private'  'new'
'protected' 'extends' 'enum'    'void'      'bool'     'char'    'short'
'int'      'long'    'float'    'double'    'byte'     'this'    # 등

# Operator:
'+'   '-'   '*'   '/'   '%'     # 산술
'&'   '|'   '^'   '~'   '<<'  '>>' # 비트
'=='  '!='  '<'   '>'   '<='  '>=' # 비교
'&&'  '||'  '!'                  # 논리
'='   '+='  '-='  '*='  '/='     # 할당
'%='  '&='  '|='  '^='  '<<='     # 복합 할당

# Delimiter:
';'   ','   '.'   '('   ')'   '{'   '}'   '['   ']'   ':'   '?'   '#'

# 3. Program total Structure
<Program> ::= { <TopLevelDecl> }

<TopLevelDecl> ::= <VarDecl> ';'
                | <FuncDecl>
                | <ClassDecl>
                | <StructDecl>
                | <NamespaceDecl>
                | <ImportDecl> ';'
                | <EnumDecl>
                | <StaticDecl> ';'
                | <EmptyDecl>

# Empty Declaration
<EmptyDecl> ::= ';'

# 4. Declaration & Definition

# 4.1. Variable Definition
##    Ex: let x : int = 5;
<VarDecl> ::= 'let' <IDENT> [ ':' <Type> ] [ '=' <Expression> ]

# 4.2. static Declaration
<StaticDecl> ::= 'const' <IDENT> ':' <Type> '=' <Expression>

# 4.3. function Declaration & Definition
##    Ex: fn foo(a : int, b : float) : int { ... }
<FuncDecl> ::= 'fn' <IDENT> '(' [ <ParamList> ] ')' [ ':' <Type> ] <Block>

<ParamList> ::= <Param> { ',' <Param> }
<Param>     ::= <IDENT> ':' <Type>

# 4.4. Class Definition
##    Ex: class MyClass extends Base { ... }
<ClassDecl> ::= 'class' <IDENT> [ 'extends' <IDENT> ] <Block>

# 4.7. Module & Import
## Ex: import std.io;
<ImportDecl> ::= 'import' <ModulePath>
<ModulePath> ::= <IDENT> { '.' <IDENT> }

# 5. Type System

<Type> ::= <PrimitiveType>
         | <IDENT>
         | <PrimitiveType> '[' ']'

<PrimitiveType> ::= 'bool'
                  | 'char'
                  | 'short'
                  | 'int'
                  | 'long'
                  | 'float'
                  | 'double'
                  | 'byte'
                  | 'void'

# 6. Statement &  Control Flow

<Stmt> ::= <Block>
         | <VarDecl> ';'
         | <ExprStmt> ';'
         | <IfStmt>
         | <WhileStmt>
         | <ForStmt>
         | <ForeachStmt>
         | <SwitchStmt>
         | <ReturnStmt>
         | <BreakStmt>
         | <ContinueStmt>
         | <TryCatchStmt>
         | <ThrowStmt> ';'
         | ';'                   # Empty statement

# 6.1. Block 
<Block> ::= '{' { <Stmt> } '}'

# 6.2. Expression
<ExprStmt> ::= <Expression>

# 6.3. if / else
<IfStmt> ::= 'if' '(' <Expression> ')' <Block> [ 'else' <Block> ]

# 6.4. while
<WhileStmt> ::= 'while' '(' <Expression> ')' <Block>

# 6.5. for
<ForStmt> ::= 'for' '(' [ <ForInit> ] ';' [ <Expression> ] ';' [ <Expression> ] ')' <Block>
<ForInit> ::= <VarDecl> | <ExprStmt>

# 6.6. foreach 
#    Ex: foreach ( item in collection ) { ... }
<ForeachStmt> ::= 'foreach' '(' <IDENT> 'in' <Expression> ')' <Block>

# 6.7. switch / case / default
<SwitchStmt> ::= 'switch' '(' <Expression> ')' '{' { <CaseBlock> } [ <DefaultBlock> ] '}'

<CaseBlock>    ::= 'case' <Expression> ':' { <Stmt> } [ 'break' ';' ]
<DefaultBlock> ::= 'default' ':' { <Stmt> } [ 'break' ';' ]

# 6.8. return
<ReturnStmt> ::= 'return' [ <Expression> ] ';'

# 6.9. break / continue
<BreakStmt>   ::= 'break' ';'
<ContinueStmt>::= 'continue' ';'

# 6.10. Except Calculation (C++ 스타일)
<TryCatchStmt> ::= 'try' <Block> { <CatchClause> }
<CatchClause>  ::= 'catch' '(' [ <IDENT> [ ':' <Type> ] ] ')' <Block>

<ThrowStmt>    ::= 'throw' <Expression>

# 7. Expression & Operator Priority

<Expression> ::= <Assignment>

# 7.1. Assign
<Assignment> ::= <Conditional> { <AssignOp> <Conditional> }
<AssignOp>   ::= '=' | '+=' | '-=' | '*=' | '/=' | '%=' | '&=' | '|=' | '^=' | '<<=' | '>>='

# 7.2. Conditional Operator
<Conditional> ::= <LogicalOr> [ '?' <Expression> ':' <Expression> ]

# 7.3. OR
<LogicalOr> ::= <LogicalAnd> { '||' <LogicalAnd> }

# 7.4. AND
<LogicalAnd> ::= <Equality> { '&&' <Equality> }

# 7.5. Equality Comparison
<Equality> ::= <Relational> { ( '==' | '!=' ) <Relational> }

# 7.6. Relational Operator
<Relational> ::= <BitOr> { ( '<' | '>' | '<=' | '>=' ) <BitOr> }

# 7.7. OR
<BitOr> ::= <BitXor> { '|' <BitXor> }

# 7.8. XOR
<BitXor> ::= <BitAnd> { '^' <BitAnd> }

# 7.9. AND
<BitAnd> ::= <Shift> { '&' <Shift> }

# 7.10. Shift
<Shift> ::= <Additive> { ( '<<' | '>>' ) <Additive> }

# 7.11. Add/Minus
<Additive> ::= <Multiplicative> { ( '+' | '-' ) <Multiplicative> }

# 7.12. Times/Divide/Other
<Multiplicative> ::= <Unary> { ( '*' | '/' | '%' ) <Unary> }

# 7.13. unary operator
<Unary> ::= ( '+' | '-' | '!' | '~' ) <Unary>
          | <Postfix>

# 7.14. Postfix Operator 
<Postfix> ::= <Primary> { <PostfixTail> }
<PostfixTail> ::= '(' [ <ArgList> ] ')'
               | '[' <Expression> ']'
               | '.' <IDENT> 

<ArgList> ::= <Expression> { ',' <Expression> }

# 7.15. Basic things
<Primary> ::= <IDENT>
            | <INT_LITERAL>
            | <CHAR_LITERAL>
            | <STRING_LITERAL>
            | <BYTE_LITERAL>
            | <BOOL_LITERAL>
            | '(' <Expression> ')'
            | <NewExpr>

<NewExpr>    ::= 'new' <Type> [ '[' <Expression> ']' ] 

<ClassMember> ::= <VarDecl> ';'
                | <FuncDecl>
                | <AccessModifier> ':'

<AccessModifier> ::= 'public' | 'private' | 'protected'


<StructMember> ::= <VarDecl> ';'
                 | <FuncDecl>
                 | <AccessModifier> ':'

<COMMENT> ::= '#' { any character except newline } newline****
