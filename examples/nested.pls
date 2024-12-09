/* Example of nested for loops */

please set a to 10;
please set b to 5;

for (set i = 0; i < a; reset i += 1) {
  please set str_i to str(i);
  print("i: ", str_i, " -- j: ");
  for (set j = b-1; j >= 0; reset j -= 1) {
    please set str_j to str(j);
    please print(str_j, space);
  }
  printn();
}

