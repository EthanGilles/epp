/* standard program to reverse a list */

PLEASE PLEASE PLEASE
set start = "Hello World!";
set temp = [len(start)]; 
printn("Before: ", start);

set i = 0;
while (i < len(start)) {
  reset temp[i] = start[len(start) - i - 1];
  reset i += 1;
}

reset i = 0;
while (i < len(start)) {
  reset start[i] = temp[i];
  reset i += 1;
}

printn("Reversed: ", start);

goodbye(0);


