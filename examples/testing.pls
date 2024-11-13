PLEASE please
/* scope testing */
set list = {'A', 'B', 'C'};

set x = "3";
set y = "20";
set z = "ABC"; 

printn(x[0]);
printn(y[0]);
printn(z[1]);

goodbye(0);

/* NEXT THING TO IMPLEMENT */
// set x = str(1+2) == set x = '3';
// set x = str(10 * 2) == set x = {'2', '0'};
// set x = "3" == set x = '3';
// set x = "20" == set x = {'2', '0'};
// set x = "ABC" == set x = {'A', 'B', 'C'};

/* THEN - PRINT */
/* set x = "hello world"; => print(x); AKA print([list])*/
/* print(str(1+2)); == print('3');     AKA print(str[expr]) where expr is single digit*/ 
/* print(str(10 * 2)); == print("20"); == print('2', '0');  
-- print(str[expr]) where expr is multi digit so [expr] is a [list] */

/* print("ABC"); == print('A', 'B', 'C'); 
-- print([list]) where [list] is parsed from "ABC" where every element is treated as a char */

/* look at -- parse_list */
