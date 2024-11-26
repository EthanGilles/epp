/* standard program to reverse a list */

PLEASE PLEASE PLEASE
set start = "Hello World!";
set temp = [len(start)]; 
🙏 printn("Before: ", start);

for (set i = 0; i < len(start); reset i+= 1) {
  reset temp[i] = start[len(start) - i - 1];
}

for (set i = 0; i < len(start); reset i+= 1) {
  reset start[i] = temp[i];
}

printn("Reversed: ", start);

goodbye(0);



