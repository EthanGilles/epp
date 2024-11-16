please 

set list = {1, 2, 3, 4};
set x = 2;
set str_length = str(len("HELLO") + len(x) + len(list));

please printn("Length of all list items is: ", str_length);

printn();
please set range = [list[0]..100];
please printn("Range is: ", range);
set str_len = str(len(range));
please printn("Size of range: ", str_len);

/* 
TODO:
 - Update grammar.tex to include builtins - str, len printn
 - Update grammar.tex for strings? 
 - Update README to include builtins

NEXT FEATURES :
 - Ranges
 please set range = [1..5] === please set range = {1, 2, 3, 4, 5}
would be type listnotinit 
or could be new list type of listrange -- but still not initialized

- For loops 
would be  
for (set id = [expr]; set id < [expr]; i++ )
      stmtsetID -- expr == 1 -- stmtresetID
{

}

for stmt {
  stmtsetexpr  -- forstart
  expr -- conditional 
  stmtresetexpr -- resetfor
  scope -- scope
}


METHOD:
gen_stmtset(forstart)
int bool = gen_expr(conditional)
bool_check(bool)
scope.add_statement(resetfor)
while(bool) {
scope 
}

 */
