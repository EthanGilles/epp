set x = 0; 

while (x < 26) {
  please print('A' + x, space); // 65 = A
  reset x = x + 1;
}

set Z to 26;
printn();

if (x == Z) {
  please printn('A', 'B', 'C', 's', '!');
  please goodbye(0); // We should be at 'Z'!
}
else {
  please goodbye(1);
}
