please

set list = {1, 2, 3, 4};
set x = 2;
set str_length = str(len("HELLO") + len(x) + len(list));

please printn("Length of all list items is: ", str_length);

please set range = ['a'..'z'];
please printn("Range is: ", range[25]);
set str_len = str(len(range));
please printn("Size of range: ", str_len);


please set a = 16;
please reset a %= 15;

if (a == 1) {
  please printn("pass");
}

set str_a = str(a);
please printn("a = ", str_a);

/* 
TODO:
 - push next release to main

NEXT FEATURES :
 - Functions?

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
