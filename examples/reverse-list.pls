/* standard program to reverse a list */

PLEASE PLEASE PLEASE
set start = {'0', '2', '4', '6', '8'};
set length = 5;

set temp = [5]; 

set count = 0;
while (count < length) {
  reset temp[count] = start[length - count - 1];
  reset count = count + 1;
}

reset count = 0;

while (count < length) {
  reset start[count] = temp[count];
  reset count = count + 1;
}

reset count = 0;
while (count < length) {
  print(start[count], space);
  reset count = count + 1;
}
printn();

goodbye(0);
