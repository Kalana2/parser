<program> -> <stmt_list>
<stmt_list> -> <stmt> <stmt_list>
<stmt> -> <decl_stmt> | <assign_stmt> | <print_stmt>
<decl_stmt> -> int <id> = <expr>; | int <id>;
<assign_stmt> -> <id> = <expr>;
<print_stmt> -> print(<id>);
<expr> -> <id> | <id> + <id> | <number>
<id> -> x | y | z
<number> -> 5 | 20
